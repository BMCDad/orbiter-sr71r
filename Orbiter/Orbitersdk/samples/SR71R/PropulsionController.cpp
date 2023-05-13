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

PropulsionController::PropulsionController(bco::BaseVessel* vessel, double amps) :
PoweredComponent(vessel, amps, 5.0),
mainFuelLevel_(0.0),
rcsFuelLevel_(0.0),
transMode_(TransMode::None),
lightFuelSupplyAvailable_(	bm::vc::FuelSupplyOnLight_verts,		bm::vc::FuelSupplyOnLight_id),
lightFuelSupplyValveOpen_(	bm::vc::FuelValveOpenSwitch_verts,	bm::vc::FuelValveOpenSwitch_id),
visTranferPumpOn_(			bm::vc::FuelTransferSwitch_verts,		bm::vc::FuelTransferSwitch_id)
{
	maxMainFlow_ = (ENGINE_THRUST / THRUST_ISP) * 2;

	// Setup switch functions:
	// Limit switch:
	swThrustLimit_.OnFunction([this]{SetThrustLevel(ENGINE_THRUST); });
	swThrustLimit_.OffFunction([this]{SetThrustLevel(ENGINE_THRUST_AB); });

	// Transfer switch:
	swSelectTransfer_.OnFunction([this] { transMode_ = TransMode::Main; });
    swSelectTransfer_.OffFunction([this] { transMode_ = TransMode::RCS; });

	// Pump switches:
	switchFuelSupplyValveOpen_.SetPressedFunc([this] {ToggleFill(); });
    vessel->RegisterVCEventTarget(&switchFuelSupplyValveOpen_);

	swTransferPump_.SetPressedFunc([this] {ToggleTransfer(); });
	vessel->RegisterVCEventTarget(&swTransferPump_);

	swDumpFuel_.OnFunction([this] {SetDumpOn(true); });
    swDumpFuel_.OffFunction([this] {SetDumpOn(false); });
}

void PropulsionController::ToggleFill()
{
	if (isFilling_)
	{
		isFilling_ = false;
	}
	else
	{
		if (	HasPower() && 
				isAvailable_ && 
				(transMode_ != TransMode::None))
		{
			isFilling_ = true;
		}
	}
}
void PropulsionController::ToggleTransfer()
{
	if (isTransfering_)
	{
		isTransfering_ = false;
	}
	else
	{
		if (	HasPower() &&
				(transMode_ != TransMode::None))
		{
			isTransfering_ = true;
		}
	}
}

void PropulsionController::SetDumpOn(bool mode)
{
	if (!mode)
	{
		isDumping_ = false;
	}
	else
	{
		// You can only dump from Main.
		if (HasPower())
		{
			isDumping_ = true;
		}
	}
}


void PropulsionController::Step(double simt, double simdt, double mjd)
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
	isAvailable_ = HasPower() ? GetBaseVessel()->IsStoppedOrDocked() : false;

	if (!isAvailable_)
	{
		isFilling_ = false;
	}

	HandleTransfer(deltaUpdate);

    auto vessel = GetBaseVessel();

    // Main flow
    auto flow = vessel->GetPropellantFlowrate(vessel->MainPropellant());
//    auto trFlow = (flow / maxMainFlow_) * (PI2 * 0.75);	// 90.718 = 200lbs per hour : 270 deg.
	if (flow < 0.0) flow = 0.0;
    gaFuelFlow_.SetState(flow / maxMainFlow_);

    // Main level
//    auto trMain = GetMainFuelLevel() * (PI2 * 0.71111);	// 256 deg.
    gaFuelMain_.SetState(GetMainFuelLevel());

    // RCS
//    auto trRCS = GetRcsFuelLevel() * (PI2 * 0.733);	// 264 deg.
    gaFuelRCS_.SetState(GetRcsFuelLevel());

	vessel->UpdateUIArea(areaId_);
}

void PropulsionController::HandleTransfer(double deltaUpdate)
{
	// Determine fuel levels:
	auto vessel = GetBaseVessel();

	auto mainFuelLevel  = vessel->GetPropellantMass(vessel->MainPropellant());

	mainFuelLevel_  = mainFuelLevel / vessel->GetPropellantMaxMass(vessel->MainPropellant());

	rcsFuelLevel_   = vessel->GetPropellantMass(vessel->RcsPropellant()) / vessel->GetPropellantMaxMass(vessel->RcsPropellant());


	// Dumping and filling of the main tank will happen regardless of the transfer switch position.
	if (isFilling_)
	{
		// Add to main.
		auto actualFill = FillMainFuel(FUEL_FILL_RATE * deltaUpdate);
		if (actualFill <= 0.0)
		{
			isFilling_ = false;
		}
	}

	if (isDumping_)
	{
		// Remove from main.
		auto actualDump = DrawMainFuel(FUEL_DUMP_RATE * deltaUpdate);
		if (actualDump <= 0.0)
		{
			isDumping_ = false;
		}
	}

	switch (transMode_)
	{
	case TransMode::Main:

		if (isTransfering_)
		{
			// Move from Main to RCS.
			// Get the requested amount to move.
			auto transferAmount = FUEL_TRANFER_RATE * deltaUpdate;
			auto actualDrawn = DrawRCSFuel(transferAmount);

			auto actual = FillMainFuel(actualDrawn);

			if (actual != actualDrawn)
			{
				isTransfering_ = false;
			}
		}
		break;

	case TransMode::None:
		// Turn everything off.
		isFilling_ = false;
		isTransfering_ = false;
		isDumping_ = false;
		break;

	case TransMode::RCS:
		
		// RCS can only be filled by transfering fuel from the main tank.

		if (isTransfering_)
		{
			// Move from Main to RCS.
			// Get the requested amount to move.
			auto transferAmount = FUEL_TRANFER_RATE * deltaUpdate;
			auto actualDrawn = DrawMainFuel(transferAmount);

			auto actual = FillRCSFuel(actualDrawn);
			

			if (actual != actualDrawn)
			{
				isTransfering_ = false;
			}
		}

		// You cannot dump from RCS
		break;
	}
}

double PropulsionController::GetVesselMainThrustLevel()
{
	return GetBaseVessel()->GetThrusterGroupLevel(THGROUP_MAIN);
}

double PropulsionController::DrawMainFuel(double amount)
{
    auto mainProp = GetBaseVessel()->MainPropellant();

	// Subtract the minimum fuel allowed to determine what available current is.
	auto current = GetBaseVessel()->GetPropellantMass(mainProp);
	auto currentAvail = current - FUEL_MINIMUM_DUMP;

	if (currentAvail <= 0.0)
	{
		// Not enough fuel to dump.
		return 0.0;
	}

	auto actual = min(amount, currentAvail);

	GetBaseVessel()->SetPropellantMass(mainProp, (current - actual));
	return actual;
}

double PropulsionController::FillMainFuel(double amount)
{
    auto mainProp = GetBaseVessel()->MainPropellant();

	auto current = GetBaseVessel()->GetPropellantMass(mainProp);
	auto topOff = MAX_FUEL - current;

	auto result = min(amount, topOff);
	GetBaseVessel()->SetPropellantMass(mainProp, (current + result));
	return result;
}

double PropulsionController::DrawRCSFuel(double amount)
{
    auto rcsProp = GetBaseVessel()->RcsPropellant();

	auto current = GetBaseVessel()->GetPropellantMass(rcsProp);

	auto result = min(amount, current);
	GetBaseVessel()->SetPropellantMass(rcsProp, (current - result));
	return result;
}

double PropulsionController::FillRCSFuel(double amount)
{
    auto rcsProp = GetBaseVessel()->RcsPropellant();

	auto current = GetBaseVessel()->GetPropellantMass(rcsProp);
	auto topOff = MAX_RCS_FUEL - current;

	auto result = min(amount, topOff);
	GetBaseVessel()->SetPropellantMass(rcsProp, (current + result));
	return result;
}

void PropulsionController::OnSetClassCaps()
{
	auto vessel = GetBaseVessel();

    swThrustLimit_.Setup(vessel);
    swSelectTransfer_.Setup(vessel);
    swDumpFuel_.Setup(vessel);
    
    gaFuelFlow_.Setup(vessel);
    gaFuelMain_.Setup(vessel);
    gaFuelRCS_.Setup(vessel);


	//	Start with max thrust (ENGINE_THRUST) this will change base on the max thrust selector.
	mainThrustHandles_[0] = vessel->CreateThruster(
        bm::main::ThrusterP_location,  
        _V(0, 0, 1), 
        ENGINE_THRUST, 
        vessel->MainPropellant(), 
        THRUST_ISP);

	mainThrustHandles_[1] = vessel->CreateThruster(
        bm::main::ThrusterS_location, 
        _V(0, 0, 1), 
        ENGINE_THRUST, 
        vessel->MainPropellant(), 
        THRUST_ISP);

	vessel->CreateThrusterGroup(mainThrustHandles_, 2, THGROUP_MAIN);

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

	for (auto i = 0; i < 2; i++) vessel->AddExhaust(es_main + i);
	vessel->AddExhaustStream(mainThrustHandles_[0], _V(-4.6230, 0, -11), &exhaust_main);
	vessel->AddExhaustStream(mainThrustHandles_[1], _V( 4.6230, 0, -11), &exhaust_main);

	THRUSTER_HANDLE th_att_rot[4], th_att_lin[4];

	// Pitch up/down attack points are simplified 8 meters forward and aft.
	th_att_rot[0] = th_att_lin[0] = vessel->CreateThruster(_V(0, 0,  8), _V(0,  1, 0), RCS_THRUST, vessel->RcsPropellant(), THRUST_ISP);
	th_att_rot[1] = th_att_lin[3] = vessel->CreateThruster(_V(0, 0, -8), _V(0, -1, 0), RCS_THRUST, vessel->RcsPropellant(), THRUST_ISP);
	th_att_rot[2] = th_att_lin[2] = vessel->CreateThruster(_V(0, 0,  8), _V(0, -1, 0), RCS_THRUST, vessel->RcsPropellant(), THRUST_ISP);
	th_att_rot[3] = th_att_lin[1] = vessel->CreateThruster(_V(0, 0, -8), _V(0,  1, 0), RCS_THRUST, vessel->RcsPropellant(), THRUST_ISP);

	vessel->CreateThrusterGroup(th_att_rot    , 2, THGROUP_ATT_PITCHUP);
	vessel->CreateThrusterGroup(th_att_rot + 2, 2, THGROUP_ATT_PITCHDOWN);
	vessel->CreateThrusterGroup(th_att_lin    , 2, THGROUP_ATT_UP);
	vessel->CreateThrusterGroup(th_att_lin + 2, 2, THGROUP_ATT_DOWN);

	// Exhaust visuals
	vessel->AddExhaust(th_att_rot[0], 0.6 , 0.078, bm::main::RCS_FL_DOWN_location, _V(0, -1, 0));
	vessel->AddExhaust(th_att_rot[0], 0.6 , 0.078, bm::main::RCS_FR_DOWN_location, _V(0, -1, 0));
	vessel->AddExhaust(th_att_rot[1], 0.79, 0.103, bm::main::RCS_RL_UP_location,   _V(0,  1, 0));
	vessel->AddExhaust(th_att_rot[1], 0.79, 0.103, bm::main::RCS_RR_UP_location,   _V(0,  1, 0));
	vessel->AddExhaust(th_att_rot[2], 0.6 , 0.078, bm::main::RCS_FL_UP_location,   _V(0,  1, 0));
	vessel->AddExhaust(th_att_rot[2], 0.6 , 0.078, bm::main::RCS_FR_UP_location,   _V(0,  1, 0));
	vessel->AddExhaust(th_att_rot[3], 0.79, 0.103, bm::main::RCS_RL_DOWN_location, _V(0, -1, 0));
	vessel->AddExhaust(th_att_rot[3], 0.79, 0.103, bm::main::RCS_RR_DOWN_location, _V(0, -1, 0));


	// Yaw left and right simplified at 10 meters.
	th_att_rot[0] = th_att_lin[0] = vessel->CreateThruster(_V(0, 0,  10), _V(-1, 0, 0), RCS_THRUST, vessel->RcsPropellant(), THRUST_ISP);
	th_att_rot[1] = th_att_lin[3] = vessel->CreateThruster(_V(0, 0, -10), _V( 1, 0, 0), RCS_THRUST, vessel->RcsPropellant(), THRUST_ISP);
	th_att_rot[2] = th_att_lin[2] = vessel->CreateThruster(_V(0, 0,  10), _V( 1, 0, 0), RCS_THRUST, vessel->RcsPropellant(), THRUST_ISP);
	th_att_rot[3] = th_att_lin[1] = vessel->CreateThruster(_V(0, 0, -10), _V(-1, 0, 0), RCS_THRUST, vessel->RcsPropellant(), THRUST_ISP);

	vessel->CreateThrusterGroup(th_att_rot    , 2, THGROUP_ATT_YAWLEFT);
	vessel->CreateThrusterGroup(th_att_rot + 2, 2, THGROUP_ATT_YAWRIGHT);
	vessel->CreateThrusterGroup(th_att_lin    , 2, THGROUP_ATT_LEFT);
	vessel->CreateThrusterGroup(th_att_lin + 2, 2, THGROUP_ATT_RIGHT);

	vessel->AddExhaust(th_att_rot[0], 0.6 , 0.078, bm::main::RCS_FR_RIGHT_location, _V( 1, 0, 0));
	vessel->AddExhaust(th_att_rot[1], 0.94, 0.122, bm::main::RCS_RL_LEFT_location,  _V(-1, 0, 0));
	vessel->AddExhaust(th_att_rot[2], 0.6 , 0.078, bm::main::RCS_FL_LEFT_location,  _V(-1, 0, 0));
	vessel->AddExhaust(th_att_rot[3], 0.94, 0.122, bm::main::RCS_RR_RIGHT_location, _V( 1, 0, 0));

	// Bank left and right.

	th_att_rot[0] = vessel->CreateThruster(_V( 5, 0, 0), _V(0,  1, 0), RCS_THRUST, vessel->RcsPropellant(), THRUST_ISP);
	th_att_rot[1] = vessel->CreateThruster(_V(-5, 0, 0), _V(0, -1, 0), RCS_THRUST, vessel->RcsPropellant(), THRUST_ISP);
	th_att_rot[2] = vessel->CreateThruster(_V(-5, 0, 0), _V(0,  1, 0), RCS_THRUST, vessel->RcsPropellant(), THRUST_ISP);
	th_att_rot[3] = vessel->CreateThruster(_V( 5, 0, 0), _V(0, -1, 0), RCS_THRUST, vessel->RcsPropellant(), THRUST_ISP);

	vessel->CreateThrusterGroup(th_att_rot    , 2, THGROUP_ATT_BANKLEFT);
	vessel->CreateThrusterGroup(th_att_rot + 2, 2, THGROUP_ATT_BANKRIGHT);

	vessel->AddExhaust(th_att_rot[0], 1.03, 0.134, bm::main::RCS_L_TOP_location,    _V(0,  1, 0));
	vessel->AddExhaust(th_att_rot[1], 1.03, 0.134, bm::main::RCS_R_BOTTOM_location, _V(0, -1, 0));
	vessel->AddExhaust(th_att_rot[2], 1.03, 0.134, bm::main::RCS_R_TOP_location,    _V(0,  1, 0));
	vessel->AddExhaust(th_att_rot[3], 1.03, 0.134, bm::main::RCS_L_BOTTOM_location, _V(0, -1, 0));


	// Forward and back
	th_att_lin[0] = vessel->CreateThruster(_V(0, 0, -7), _V(0, 0,  1), 2 * RCS_THRUST, vessel->RcsPropellant(), THRUST_ISP);
	th_att_lin[1] = vessel->CreateThruster(_V(0, 0,  7), _V(0, 0, -1), 2 * RCS_THRUST, vessel->RcsPropellant(), THRUST_ISP);

	vessel->CreateThrusterGroup(th_att_lin    , 1, THGROUP_ATT_FORWARD);
	vessel->CreateThrusterGroup(th_att_lin + 1, 1, THGROUP_ATT_BACK);

	vessel->AddExhaust(th_att_lin[0], 0.6, 0.078, bm::main::RCS_RL_LEFT_location,   _V(0, 0, -1));
	vessel->AddExhaust(th_att_lin[0], 0.6, 0.078, bm::main::RCS_RR_RIGHT_location,  _V(0, 0, -1));
	vessel->AddExhaust(th_att_lin[1], 0.6, 0.078, bm::main::RCS_L_FORWARD_location, _V(0, 0,  1));
	vessel->AddExhaust(th_att_lin[1], 0.6, 0.078, bm::main::RCS_R_FORWARD_location, _V(0, 0,  1));

	swThrustLimit_.SetOff();
	
	areaId_ = GetBaseVessel()->RegisterVCRedrawEvent(this);
}

bool PropulsionController::OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
	if (_strnicmp(key, ConfigKey, 10) != 0)
	{
		return false;
	}

	int limit;
	transMode_ = TransMode::None;
	int fill = 0;
	int transfer = 0;
	int dump = 0;

	sscanf_s(configLine + 10, "%i%i%i%i%i", &limit, &transMode_, &fill, &transfer, &dump);

	// Thrust limit.
	swThrustLimit_.SetState((limit == 0) ? 0.0 : 1.0);

	// Transfer switch.
	transMode_ = (transMode_ == 0) ? TransMode::RCS : TransMode::Main;
	swSelectTransfer_.SetState((transMode_ == TransMode::RCS) ? 0 : 1);

	// Pumps
	isFilling_ = (fill != 0);
	isTransfering_ = (transfer != 0);
	isDumping_ = (dump != 0);

    return true;
}

void PropulsionController::OnSaveConfiguration(FILEHANDLE scn) const
{
	char cbuf[256];
	auto val = (swThrustLimit_.GetState() == 0.0) ? 0 : 1;

	auto step = (swSelectTransfer_.GetState() == 0.0) ? 0 : 1;
	auto iFill = (isFilling_) ? 1 : 0;
	auto iTrans = (isTransfering_) ? 1 : 0;
	auto iDump = (isDumping_) ? 1 : 0;
    
	sprintf_s(cbuf, "%i %i %i %i %i", val, step, iFill, iTrans, iDump);
	oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}

void PropulsionController::SetVesselMainThrustLevel(double level)
{
	GetBaseVessel()->SetThrusterGroupLevel(THGROUP_MAIN, level);
}

void PropulsionController::SetAttitudeRotLevel(Axis axis, double level)
{
    GetBaseVessel()->SetAttitudeRotLevel((int)axis, level);
}


bool PropulsionController::OnLoadVC(int id)
{
	// Redraw
	oapiVCRegisterArea(areaId_, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
    return true;
}

bool PropulsionController::OnVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	auto devMesh = GetBaseVessel()->GetVirtualCockpitMesh0();
	assert(devMesh != nullptr);




	const double availOffset = 0.0244;
	const double newBtnOffset = 0.0352;
	double trans = 0.0;

	trans = isAvailable_ ? availOffset : 0.0;
	lightFuelSupplyAvailable_.SetTranslate(_V(trans, 0.0, 0.0));
	lightFuelSupplyAvailable_.Draw(devMesh);

	trans = isFilling_ ? newBtnOffset : 0.0;
	lightFuelSupplyValveOpen_.SetTranslate(_V(trans, 0.0, 0.0));
	lightFuelSupplyValveOpen_.Draw(devMesh);

	trans = isTransfering_ ? newBtnOffset : 0.0;
	visTranferPumpOn_.SetTranslate(_V(trans, 0.0, 0.0));
	visTranferPumpOn_.Draw(devMesh);

	return true;
}

void PropulsionController::SetThrustLevel(double newLevel)
{
	maxThrustLevel_ = newLevel;
	GetBaseVessel()->SetThrusterMax0(mainThrustHandles_[0], maxThrustLevel_);
	GetBaseVessel()->SetThrusterMax0(mainThrustHandles_[1], maxThrustLevel_);
}

bool PropulsionController::DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return false;

    auto th = GetBaseVessel()->GetThrusterGroupLevel(THGROUP_MAIN);
    auto hv = GetBaseVessel()->GetThrusterGroupLevel(THGROUP_HOVER);
    auto rt = GetBaseVessel()->GetThrusterGroupLevel(THGROUP_RETRO);


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

    return true;
}
