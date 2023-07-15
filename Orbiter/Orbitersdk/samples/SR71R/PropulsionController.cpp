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

PropulsionController::PropulsionController(bco::power_provider& pwr, bco::BaseVessel& vessel) :
	power_(pwr),
	vessel_(vessel),
	mainFuelLevel_(0.0),
	rcsFuelLevel_(0.0)
	//slotThrottleLimit_(	[&](bool v) { SetThrustLevel((v) ? ENGINE_THRUST : ENGINE_THRUST_AB); }),
	//slotFuelDump_(		[&](bool v) { }), 
	//slotTransferSel_(	[&](bool v) { }),
	//slotFuelValveOpen_(	[&](bool b) { ToggleFill(); slotFuelValveOpen_.set(); })
{
	maxMainFlow_ = (ENGINE_THRUST / THRUST_ISP) * 2;

	vessel.AddControl(&switchFuelDump_);
	vessel.AddControl(&switchThrustLimit_);
	vessel.AddControl(&gaugeFuelFlow_);
	vessel.AddControl(&gaugeFuelMain_);
	vessel.AddControl(&gaugeFuelRCS_);
	vessel.AddControl(&btnFuelValveOpen_);
	vessel.AddControl(&lightFuelValveOpen_);
	vessel.AddControl(&lightFuelAvail_);

	switchThrustLimit_.attach([&]() { 
		SetThrustLevel(switchThrustLimit_.is_on() ? ENGINE_THRUST : ENGINE_THRUST_AB); 
		});

	btnFuelValveOpen_.attach([&]() { ToggleFill(); });
}

void PropulsionController::ToggleFill()
{
	if (sigIsFuelValveOpen_.current())
	{
		sigIsFuelValveOpen_.fire(false);
	}
	else
	{
		if (IsPowered() && sigIsFuelAvail_.current())
		{
			sigIsFuelValveOpen_.fire(true);
		}
	}
}

void PropulsionController::handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd)
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
	sigIsFuelAvail_.fire(IsPowered() && vessel_.IsStoppedOrDocked());

	if (!sigIsFuelAvail_.current())
	{
		sigIsFuelValveOpen_.fire(false);
	}

	HandleTransfer(deltaUpdate); // <- this sets the current levels.

    // Main flow
    auto flow = vessel_.GetPropellantFlowrate(vessel_.MainPropellant());
//    auto trFlow = (flow / maxMainFlow_) * (PI2 * 0.75);	// 90.718 = 200lbs per hour : 270 deg.
	if ((flow < 0.0) || switchFuelDump_.is_on()) flow = 0.0;	 // Don't report flow if dumping.
	sigFuelFlowRate_.fire(flow / maxMainFlow_);	 // Converted to 0-1 range.

    // Main level
//    auto trMain = GetMainFuelLevel() * (PI2 * 0.71111);	// 256 deg.
	sigMainFuelLevel_.fire(mainFuelLevel_);

    // RCS
//    auto trRCS = GetRcsFuelLevel() * (PI2 * 0.733);	// 264 deg.
	sigRCSFuelLevel_.fire(rcsFuelLevel_);
}

void PropulsionController::HandleTransfer(double deltaUpdate)
{
	// Determine fuel levels:
	auto mainFuelLevel  = vessel_.GetPropellantMass(vessel_.MainPropellant());

	mainFuelLevel_  = mainFuelLevel / vessel_.GetPropellantMaxMass(vessel_.MainPropellant());

	rcsFuelLevel_   = vessel_.GetPropellantMass(vessel_.RcsPropellant()) / vessel_.GetPropellantMaxMass(vessel_.RcsPropellant());


	// Dumping and filling of the main tank will happen regardless of the transfer switch position.
	if (sigIsFuelValveOpen_.current()) // cannot be enabled if external fuel is unavailable.
	{
		// Add to main.
		auto actualFill = FillMainFuel(FUEL_FILL_RATE * deltaUpdate);
		if (actualFill <= 0.0)
		{
			sigIsFuelValveOpen_.fire(false);
		}
	}

	if (IsPowered() && switchFuelDump_.is_on())
	{
		auto actualDump = DrawMainFuel(FUEL_DUMP_RATE * deltaUpdate);
	}
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

void PropulsionController::handle_set_class_caps(bco::BaseVessel& vessel)
{
	//	Start with max thrust (ENGINE_THRUST) this will change base on the max thrust selector.
	mainThrustHandles_[0] = vessel.CreateThruster(
        bm::main::ThrusterP_location,  
        _V(0, 0, 1), 
        ENGINE_THRUST, 
        vessel.MainPropellant(), 
        THRUST_ISP);

	mainThrustHandles_[1] = vessel.CreateThruster(
        bm::main::ThrusterS_location, 
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
	vessel.AddExhaust(th_att_rot[0], 0.6 , 0.078, bm::main::RCS_FL_DOWN_location, _V(0, -1, 0));
	vessel.AddExhaust(th_att_rot[0], 0.6 , 0.078, bm::main::RCS_FR_DOWN_location, _V(0, -1, 0));
	vessel.AddExhaust(th_att_rot[1], 0.79, 0.103, bm::main::RCS_RL_UP_location,   _V(0,  1, 0));
	vessel.AddExhaust(th_att_rot[1], 0.79, 0.103, bm::main::RCS_RR_UP_location,   _V(0,  1, 0));
	vessel.AddExhaust(th_att_rot[2], 0.6 , 0.078, bm::main::RCS_FL_UP_location,   _V(0,  1, 0));
	vessel.AddExhaust(th_att_rot[2], 0.6 , 0.078, bm::main::RCS_FR_UP_location,   _V(0,  1, 0));
	vessel.AddExhaust(th_att_rot[3], 0.79, 0.103, bm::main::RCS_RL_DOWN_location, _V(0, -1, 0));
	vessel.AddExhaust(th_att_rot[3], 0.79, 0.103, bm::main::RCS_RR_DOWN_location, _V(0, -1, 0));


	// Yaw left and right simplified at 10 meters.
	th_att_rot[0] = th_att_lin[0] = vessel.CreateThruster(_V(0, 0,  10), _V(-1, 0, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[1] = th_att_lin[3] = vessel.CreateThruster(_V(0, 0, -10), _V( 1, 0, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[2] = th_att_lin[2] = vessel.CreateThruster(_V(0, 0,  10), _V( 1, 0, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[3] = th_att_lin[1] = vessel.CreateThruster(_V(0, 0, -10), _V(-1, 0, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);

	vessel.CreateThrusterGroup(th_att_rot    , 2, THGROUP_ATT_YAWLEFT);
	vessel.CreateThrusterGroup(th_att_rot + 2, 2, THGROUP_ATT_YAWRIGHT);
	vessel.CreateThrusterGroup(th_att_lin    , 2, THGROUP_ATT_LEFT);
	vessel.CreateThrusterGroup(th_att_lin + 2, 2, THGROUP_ATT_RIGHT);

	vessel.AddExhaust(th_att_rot[0], 0.6 , 0.078, bm::main::RCS_FR_RIGHT_location, _V( 1, 0, 0));
	vessel.AddExhaust(th_att_rot[1], 0.94, 0.122, bm::main::RCS_RL_LEFT_location,  _V(-1, 0, 0));
	vessel.AddExhaust(th_att_rot[2], 0.6 , 0.078, bm::main::RCS_FL_LEFT_location,  _V(-1, 0, 0));
	vessel.AddExhaust(th_att_rot[3], 0.94, 0.122, bm::main::RCS_RR_RIGHT_location, _V( 1, 0, 0));

	// Bank left and right.

	th_att_rot[0] = vessel.CreateThruster(_V( 5, 0, 0), _V(0,  1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[1] = vessel.CreateThruster(_V(-5, 0, 0), _V(0, -1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[2] = vessel.CreateThruster(_V(-5, 0, 0), _V(0,  1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_rot[3] = vessel.CreateThruster(_V( 5, 0, 0), _V(0, -1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);

	vessel.CreateThrusterGroup(th_att_rot    , 2, THGROUP_ATT_BANKLEFT);
	vessel.CreateThrusterGroup(th_att_rot + 2, 2, THGROUP_ATT_BANKRIGHT);

	vessel.AddExhaust(th_att_rot[0], 1.03, 0.134, bm::main::RCS_L_TOP_location,    _V(0,  1, 0));
	vessel.AddExhaust(th_att_rot[1], 1.03, 0.134, bm::main::RCS_R_BOTTOM_location, _V(0, -1, 0));
	vessel.AddExhaust(th_att_rot[2], 1.03, 0.134, bm::main::RCS_R_TOP_location,    _V(0,  1, 0));
	vessel.AddExhaust(th_att_rot[3], 1.03, 0.134, bm::main::RCS_L_BOTTOM_location, _V(0, -1, 0));


	// Forward and back
	th_att_lin[0] = vessel.CreateThruster(_V(0, 0, -7), _V(0, 0,  1), 2 * RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
	th_att_lin[1] = vessel.CreateThruster(_V(0, 0,  7), _V(0, 0, -1), 2 * RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);

	vessel.CreateThrusterGroup(th_att_lin    , 1, THGROUP_ATT_FORWARD);
	vessel.CreateThrusterGroup(th_att_lin + 1, 1, THGROUP_ATT_BACK);

	vessel.AddExhaust(th_att_lin[0], 0.6, 0.078, bm::main::RCS_RL_LEFT_location,   _V(0, 0, -1));
	vessel.AddExhaust(th_att_lin[0], 0.6, 0.078, bm::main::RCS_RR_RIGHT_location,  _V(0, 0, -1));
	vessel.AddExhaust(th_att_lin[1], 0.6, 0.078, bm::main::RCS_L_FORWARD_location, _V(0, 0,  1));
	vessel.AddExhaust(th_att_lin[1], 0.6, 0.078, bm::main::RCS_R_FORWARD_location, _V(0, 0,  1));

	// swThrustLimit_.SetOff();
	
//	areaId_ = GetBaseVessel()->RegisterVCRedrawEvent(this);
}

bool PropulsionController::handle_load_state(const std::string& line)
{
	int limit;
	int transMode_ = 0; // TODO
	int fill = 0;
	int transfer = 0;
	int dump = 0;

//	sscanf_s(configLine + 10, "%i%i%i%i%i", &limit, &transMode_, &fill, &transfer, &dump);

	// Thrust limit.
	// TODO swThrustLimit_.SetState((limit == 0) ? 0.0 : 1.0);

	// Transfer switch.
	// TODO transMode_ = (transMode_ == 0) ? TransMode::RCS : TransMode::Main;
	// TODO swSelectTransfer_.SetState((transMode_ == TransMode::RCS) ? 0 : 1);

	// Pumps
	// TODO isFilling_ = (fill != 0);   <--fuel valve open
	// TODO isTransfering_ = (transfer != 0);
	// TODO isDumping_ = (dump != 0);

    return true;
}

std::string PropulsionController::handle_save_state()
{
	//char cbuf[256];
	//auto val = 0; // TODO (swThrustLimit_.GetState() == 0.0) ? 0 : 1;

	//auto step = 0; // TODO (swSelectTransfer_.GetState() == 0.0) ? 0 : 1;
	//auto iFill = 0; // TODO (isFilling_) ? 1 : 0;
	//auto iTrans = 0; // TODO (isTransfering_) ? 1 : 0;
	//auto iDump = 0; // TODO (isDumping_) ? 1 : 0;
 //   
	//sprintf_s(cbuf, "%i %i %i %i %i", val, step, iFill, iTrans, iDump);
	//oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
	return std::string();
}

void PropulsionController::SetVesselMainThrustLevel(double level)
{
	vessel_.SetThrusterGroupLevel(THGROUP_MAIN, level);
}

void PropulsionController::SetAttitudeRotLevel(Axis axis, double level)
{
	vessel_.SetAttitudeRotLevel((int)axis, level);
}

void PropulsionController::SetThrustLevel(double newLevel)
{
	maxThrustLevel_ = newLevel;
	vessel_.SetThrusterMax0(mainThrustHandles_[0], maxThrustLevel_);
	vessel_.SetThrusterMax0(mainThrustHandles_[1], maxThrustLevel_);
}

void PropulsionController::handle_draw_hud(bco::BaseVessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
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
    char* LB = nullptr;
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
