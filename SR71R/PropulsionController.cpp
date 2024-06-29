//	PropulsionController - SR-71r Orbiter Addon
//	Copyright(C) 2015  Blake Christensen
//
//	This program is free software : you can redistribute it and / or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see <http://www.gnu.org/licenses/>.

#include "StdAfx.h"

#include "Orbitersdk.h"

#include "PropulsionController.h"
#include "ShipMets.h"
#include "SR71r_mesh.h"

#include <assert.h>

PropulsionController::PropulsionController(bco::power_provider& pwr, bco::vessel& vessel) :
	power_(pwr),
	vessel_(vessel),
	mainFuelLevel_(0.0),
	rcsFuelLevel_(0.0)
	//slotThrottleLimit_(	[&](bool v) { SetThrustLevel((v) ? ENGINE_THRUST : ENGINE_THRUST_AB); }),
	//slotFuelDump_(		[&](bool v) { }), 
	//slotTransferSel_(	[&](bool v) { }),
	//slotFuelValveOpen_(	[&](bool b) { ToggleFill(); slotFuelValveOpen_.set(); })
{
	power_.attach_consumer(this);

	maxMainFlow_ = (ENGINE_THRUST / THRUST_ISP) * 2;

	vessel.AddControl(&switchFuelDump_);
	vessel.AddControl(&switchThrustLimit_);
	vessel.AddControl(&gaugeFuelFlow_);
	vessel.AddControl(&gaugeFuelMain_);
	vessel.AddControl(&gaugeFuelRCS_);
	vessel.AddControl(&btnFuelValveOpen_);
	vessel.AddControl(&lightFuelValveOpen_);
	vessel.AddControl(&lightFuelAvail_);
	vessel.AddControl(&lightRCSAvail_);

	vessel.AddControl(&btnRCSValveOpen_);
	vessel.AddControl(&lightRCSValveOpen_);

	vessel.AddControl(&statusFuel_);
	vessel.AddControl(&statusLimiter_);

	switchThrustLimit_.attach([&]() { SetThrustLevel(switchThrustLimit_.is_on() ? ENGINE_THRUST : ENGINE_THRUST_AB); });

	btnFuelValveOpen_.attach([&]() { ToggleFill(); });
	btnRCSValveOpen_.attach([&]() { ToggleRCSFill(); });
}

void PropulsionController::ToggleFill()
{
	if (isFilling_)
	{
		isFilling_ = false;
	}
	else
	{
		if (IsPowered() && isExternAvail_)
		{
			isFilling_ = true;
		}
	}
}

void PropulsionController::ToggleRCSFill()
{
	if (isRCSFilling_)
	{
		isRCSFilling_ = false;
	}
	else
	{
		if (IsPowered() && isExternAvail_)
		{
			isRCSFilling_ = true;
		}
	}
}

void PropulsionController::handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd)
{
	// Limit how often we run
	if (fabs(simt - prevTime_) > 0.1)
	{
		Update((simt - prevTime_));
		prevTime_ = simt;
	}
}

void PropulsionController::Update(double deltaUpdate)
{
	isExternAvail_ = (IsPowered() && vessel_.IsStoppedOrDocked());
	lightFuelAvail_.set_state(isExternAvail_);
	lightRCSAvail_.set_state(isExternAvail_);

	if (!isExternAvail_)
	{
		isFilling_ = false;
		isRCSFilling_ = false;
	}

	lightFuelValveOpen_.set_state(isFilling_);
	lightRCSValveOpen_.set_state(isRCSFilling_);

	HandleTransfer(deltaUpdate); // <- this sets the current levels.

    // Main flow
    auto flow = vessel_.GetPropellantFlowrate(vessel_.MainPropellant());
//    auto trFlow = (flow / maxMainFlow_) * (PI2 * 0.75);	// 90.718 = 200lbs per hour : 270 deg.
	if ((flow < 0.0) || switchFuelDump_.is_on()) flow = 0.0;	 // Don't report flow if dumping.
//	sigFuelFlowRate_.fire(flow / maxMainFlow_);	 // Converted to 0-1 range.
	gaugeFuelFlow_.set_state(flow / maxMainFlow_);

	statusLimiter_.set_state(
		(!IsPowered() || switchThrustLimit_.is_on())
		?	bco::status_display::status::off
		:	bco::status_display::status::on
	);


    // Main level
//    auto trMain = GetMainFuelLevel() * (PI2 * 0.71111);	// 256 deg.
//	sigMainFuelLevel_.fire(mainFuelLevel_);

    // RCS
//    auto trRCS = GetRcsFuelLevel() * (PI2 * 0.733);	// 264 deg.
//	sigRCSFuelLevel_.fire(rcsFuelLevel_);
}

void PropulsionController::HandleTransfer(double deltaUpdate)
{
	// Determine fuel levels:
	mainFuelLevel_  = vessel_.GetPropellantMass(vessel_.MainPropellant()) / 
					  vessel_.GetPropellantMaxMass(vessel_.MainPropellant());

	rcsFuelLevel_   = vessel_.GetPropellantMass(vessel_.RcsPropellant()) / 
					  vessel_.GetPropellantMaxMass(vessel_.RcsPropellant());

	gaugeFuelMain_.set_state(mainFuelLevel_);
	gaugeFuelRCS_.set_state(rcsFuelLevel_);

	// Dumping and filling of the main tank will happen regardless of the transfer switch position.
	if (isFilling_) // cannot be enabled if external fuel is unavailable.
	{
		// Add to main.
		auto actualFill = FillMainFuel(FUEL_FILL_RATE * deltaUpdate);
		if (actualFill <= 0.0)
		{
			isFilling_ = false;
		}
	}

	if (isRCSFilling_) {
		auto actualFill = FillRCSFuel(FUEL_FILL_RATE * deltaUpdate);
		if (actualFill <= 0.0) {
			isRCSFilling_ = false;
		}
	}

	if (IsPowered() && switchFuelDump_.is_on())
	{
		auto actualDump = DrawMainFuel(FUEL_DUMP_RATE * deltaUpdate);
	}

	signalMainFuelLevel_.fire(mainFuelLevel_);

	statusFuel_.set_state(
		(mainFuelLevel_ > 0.2) || !IsPowered()
		?	bco::status_display::status::off
		:	mainFuelLevel_ == 0.0
			?	bco::status_display::status::error
			:	bco::status_display::status::warn);
}

double PropulsionController::GetVesselMainThrustLevel()
{
	return vessel_.GetThrusterGroupLevel(THGROUP_MAIN);
}

double PropulsionController::DrawMainFuel(double amount)
{
    auto mainProp = vessel_.MainPropellant();

	// Subtract the minimum fuel allowed to determine what available current is.
	auto current = vessel_.GetPropellantMass(mainProp);
	auto currentAvail = current - FUEL_MINIMUM_DUMP;

	if (currentAvail <= 0.0)
	{
		// Not enough fuel to dump.
		return 0.0;
	}

	auto actual = min(amount, currentAvail);

	vessel_.SetPropellantMass(mainProp, (current - actual));
	return actual;
}

double PropulsionController::FillMainFuel(double amount)
{
    auto mainProp = vessel_.MainPropellant();

	auto current = vessel_.GetPropellantMass(mainProp);
	auto topOff = MAX_FUEL - current;

	auto result = min(amount, topOff);
	vessel_.SetPropellantMass(mainProp, (current + result));
	return result;
}

double PropulsionController::DrawRCSFuel(double amount)
{
    auto rcsProp = vessel_.RcsPropellant();

	auto current = vessel_.GetPropellantMass(rcsProp);

	auto result = min(amount, current);
	vessel_.SetPropellantMass(rcsProp, (current - result));
	return result;
}

double PropulsionController::FillRCSFuel(double amount)
{
    auto rcsProp = vessel_.RcsPropellant();

	auto current = vessel_.GetPropellantMass(rcsProp);
	auto topOff = MAX_RCS_FUEL - current;

	auto result = min(amount, topOff);
	vessel_.SetPropellantMass(rcsProp, (current + result));
	return result;
}

void PropulsionController::handle_set_class_caps(bco::vessel& vessel)
{
	//	Start with max thrust (ENGINE_THRUST) this will change base on the max thrust selector.
	mainThrustHandles_[0] = vessel.CreateThruster(
        bm::main::ThrusterP_loc,  
        _V(0, 0, 1), 
        ENGINE_THRUST, 
        vessel.MainPropellant(), 
        THRUST_ISP);

	mainThrustHandles_[1] = vessel.CreateThruster(
        bm::main::ThrusterS_loc, 
        _V(0, 0, 1), 
        ENGINE_THRUST, 
        vessel.MainPropellant(), 
        THRUST_ISP);

	vessel.CreateThrusterGroup(mainThrustHandles_, 2, THGROUP_MAIN);

	EXHAUSTSPEC es_main[2] =
	{
		{ mainThrustHandles_[0], NULL, NULL, NULL, 12, 1, 0, 0.1, NULL },
		{ mainThrustHandles_[1], NULL, NULL, NULL, 12, 1, 0, 0.1, NULL }
	};

	PARTICLESTREAMSPEC exhaust_main = {
		0, 2.0, 13, 150, 0.1, 0.2, 16, 1.0, PARTICLESTREAMSPEC::EMISSIVE,
		PARTICLESTREAMSPEC::LVL_SQRT, 0, 1,
		PARTICLESTREAMSPEC::ATM_PLOG, 1e-5, 0.1
	};

	for (auto i = 0; i < 2; i++) vessel.AddExhaust(es_main + i);
	vessel.AddExhaustStream(mainThrustHandles_[0], _V(-4.6230, 0, -11), &exhaust_main);
	vessel.AddExhaustStream(mainThrustHandles_[1], _V( 4.6230, 0, -11), &exhaust_main);

	THRUSTER_HANDLE th_att_rot[4], th_att_lin[4];

	// Pitch up/down attack points are simplified 8 meters forward and aft.
	th_att_rot[0] = th_att_lin[0] = vessel.CreateThruster(_V(0, 0,  8), _V(0,  1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[1] = th_att_lin[3] = vessel.CreateThruster(_V(0, 0, -8), _V(0, -1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[2] = th_att_lin[2] = vessel.CreateThruster(_V(0, 0,  8), _V(0, -1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[3] = th_att_lin[1] = vessel.CreateThruster(_V(0, 0, -8), _V(0,  1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);

	vessel.CreateThrusterGroup(th_att_rot    , 2, THGROUP_ATT_PITCHUP);
	vessel.CreateThrusterGroup(th_att_rot + 2, 2, THGROUP_ATT_PITCHDOWN);
	vessel.CreateThrusterGroup(th_att_lin    , 2, THGROUP_ATT_UP);
	vessel.CreateThrusterGroup(th_att_lin + 2, 2, THGROUP_ATT_DOWN);

	// Exhaust visuals
	vessel.AddExhaust(th_att_rot[0], 0.6 , 0.078, bm::main::RCS_FL_DOWN_loc, _V(0, -1, 0));
	vessel.AddExhaust(th_att_rot[0], 0.6 , 0.078, bm::main::RCS_FR_DOWN_loc, _V(0, -1, 0));
	vessel.AddExhaust(th_att_rot[1], 0.79, 0.103, bm::main::RCS_RL_UP_loc,   _V(0,  1, 0));
	vessel.AddExhaust(th_att_rot[1], 0.79, 0.103, bm::main::RCS_RR_UP_loc,   _V(0,  1, 0));
	vessel.AddExhaust(th_att_rot[2], 0.6 , 0.078, bm::main::RCS_FL_UP_loc,   _V(0,  1, 0));
	vessel.AddExhaust(th_att_rot[2], 0.6 , 0.078, bm::main::RCS_FR_UP_loc,   _V(0,  1, 0));
	vessel.AddExhaust(th_att_rot[3], 0.79, 0.103, bm::main::RCS_RL_DOWN_loc, _V(0, -1, 0));
	vessel.AddExhaust(th_att_rot[3], 0.79, 0.103, bm::main::RCS_RR_DOWN_loc, _V(0, -1, 0));


	// Yaw left and right simplified at 10 meters.
	th_att_rot[0] = th_att_lin[0] = vessel.CreateThruster(_V(0, 0,  10), _V(-1, 0, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[1] = th_att_lin[3] = vessel.CreateThruster(_V(0, 0, -10), _V( 1, 0, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[2] = th_att_lin[2] = vessel.CreateThruster(_V(0, 0,  10), _V( 1, 0, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[3] = th_att_lin[1] = vessel.CreateThruster(_V(0, 0, -10), _V(-1, 0, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);

	vessel.CreateThrusterGroup(th_att_rot    , 2, THGROUP_ATT_YAWLEFT);
	vessel.CreateThrusterGroup(th_att_rot + 2, 2, THGROUP_ATT_YAWRIGHT);
	vessel.CreateThrusterGroup(th_att_lin    , 2, THGROUP_ATT_LEFT);
	vessel.CreateThrusterGroup(th_att_lin + 2, 2, THGROUP_ATT_RIGHT);

	vessel.AddExhaust(th_att_rot[0], 0.6 , 0.078, bm::main::RCS_FR_RIGHT_loc, _V( 1, 0, 0));
	vessel.AddExhaust(th_att_rot[1], 0.94, 0.122, bm::main::RCS_RL_LEFT_loc,  _V(-1, 0, 0));
	vessel.AddExhaust(th_att_rot[2], 0.6 , 0.078, bm::main::RCS_FL_LEFT_loc,  _V(-1, 0, 0));
	vessel.AddExhaust(th_att_rot[3], 0.94, 0.122, bm::main::RCS_RR_RIGHT_loc, _V( 1, 0, 0));

	// Bank left and right.

	th_att_rot[0] = vessel.CreateThruster(_V( 5, 0, 0), _V(0,  1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[1] = vessel.CreateThruster(_V(-5, 0, 0), _V(0, -1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[2] = vessel.CreateThruster(_V(-5, 0, 0), _V(0,  1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[3] = vessel.CreateThruster(_V( 5, 0, 0), _V(0, -1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);

	vessel.CreateThrusterGroup(th_att_rot    , 2, THGROUP_ATT_BANKLEFT);
	vessel.CreateThrusterGroup(th_att_rot + 2, 2, THGROUP_ATT_BANKRIGHT);

	vessel.AddExhaust(th_att_rot[0], 1.03, 0.134, bm::main::RCS_L_TOP_loc,    _V(0,  1, 0));
	vessel.AddExhaust(th_att_rot[1], 1.03, 0.134, bm::main::RCS_R_BOTTOM_loc, _V(0, -1, 0));
	vessel.AddExhaust(th_att_rot[2], 1.03, 0.134, bm::main::RCS_R_TOP_loc,    _V(0,  1, 0));
	vessel.AddExhaust(th_att_rot[3], 1.03, 0.134, bm::main::RCS_L_BOTTOM_loc, _V(0, -1, 0));


	// Forward and back
	th_att_lin[0] = vessel.CreateThruster(_V(0, 0, -7), _V(0, 0,  1), 2 * RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_lin[1] = vessel.CreateThruster(_V(0, 0,  7), _V(0, 0, -1), 2 * RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);

	vessel.CreateThrusterGroup(th_att_lin    , 1, THGROUP_ATT_FORWARD);
	vessel.CreateThrusterGroup(th_att_lin + 1, 1, THGROUP_ATT_BACK);

	vessel.AddExhaust(th_att_lin[0], 0.6, 0.078, bm::main::RCS_RL_LEFT_loc,   _V(0, 0, -1));
	vessel.AddExhaust(th_att_lin[0], 0.6, 0.078, bm::main::RCS_RR_RIGHT_loc,  _V(0, 0, -1));
	vessel.AddExhaust(th_att_lin[1], 0.6, 0.078, bm::main::RCS_L_FORWARD_loc, _V(0, 0,  1));
	vessel.AddExhaust(th_att_lin[1], 0.6, 0.078, bm::main::RCS_R_FORWARD_loc, _V(0, 0,  1));

	// swThrustLimit_.SetOff();
	
//	areaId_ = GetBaseVessel()->RegisterVCRedrawEvent(this);
}

bool PropulsionController::handle_load_state(bco::vessel& vessel, const std::string& line)
{
	std::istringstream in(line);
	in >> switchThrustLimit_;
	return true;
}

std::string PropulsionController::handle_save_state(bco::vessel& vessel)
{
	std::ostringstream os;
	os << switchThrustLimit_;
	return os.str();
}

void PropulsionController::SetVesselMainThrustLevel(double level)
{
	vessel_.SetThrusterGroupLevel(THGROUP_MAIN, level);
}

void PropulsionController::SetAttitudeRotLevel(bco::Axis axis, double level)
{
	vessel_.SetAttitudeRotLevel((int)axis, level);
}

void PropulsionController::SetThrustLevel(double newLevel)
{
	maxThrustLevel_ = newLevel;
	vessel_.SetThrusterMax0(mainThrustHandles_[0], maxThrustLevel_);
	vessel_.SetThrusterMax0(mainThrustHandles_[1], maxThrustLevel_);
}

void PropulsionController::handle_draw_hud(bco::vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return;

    auto th = vessel.GetThrusterGroupLevel(THGROUP_MAIN);
    auto hv = vessel.GetThrusterGroupLevel(THGROUP_HOVER);
    auto rt = vessel.GetThrusterGroupLevel(THGROUP_RETRO);


    char cbuf[16];
    int lvl = 0;
    int xLeft = hps->W - 62;
    int yTop = hps->H - 90;

    int txX = hps->W - 56;
    int bxLeft = hps->W - 62;
    int bxRight = bxLeft + 45;
    int bxHeight = 25;
    int lbX = hps->W - 12;


    // Main thrust
    skp->Rectangle(
        bxLeft,
        yTop,
        bxRight,
        yTop + bxHeight);

    auto mt = 0.0;
    const char* LB = (th >= rt) ? "M" : "R";
    if (th >= rt)
    {
        mt = th;
        LB = "M";
    }
    else
    {
        mt = rt;
        LB = "R";
    }

    lvl = (int)(mt * 100);
    sprintf_s(cbuf, "%3i", lvl);
    skp->Text(txX, yTop + 1, cbuf, 3);
    skp->Text(lbX, yTop + 1, LB, 1);

    // Hover
    yTop += 30;
    skp->Rectangle(
        bxLeft,
        yTop,
        bxRight,
        yTop + bxHeight);

    lvl = (int)(hv * 100);
    sprintf_s(cbuf, "%3i", lvl);

    skp->Text(txX, yTop + 1, cbuf, 3);
    skp->Text(lbX, yTop + 1, "H", 1);
}
