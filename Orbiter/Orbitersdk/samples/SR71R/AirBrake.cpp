//	AirBrake - SR-71r Orbiter Addon
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

#include "AirBrake.h"
#include "SR71r_mesh.h"

AirBrake::AirBrake(bco::BaseVessel* vessel) :
	Component(vessel),
	apu_(nullptr),
	dragFactor_(0.0)
{
	airBrakeSwitch_.AddStopFunc(0.0,    [] { });
	airBrakeSwitch_.AddStopFunc(0.33,   [] { });
	airBrakeSwitch_.AddStopFunc(0.66,   [] { });
	airBrakeSwitch_.AddStopFunc(1.0,    [] { });

    eventIncreaseBrake_.SetLeftMouseDownFunc([this] {airBrakeSwitch_.Increment(); });
    vessel->RegisterVCEventTarget(&eventIncreaseBrake_);

    eventDecreaseBrake_.SetLeftMouseDownFunc([this] {airBrakeSwitch_.Decrement(); });
    vessel->RegisterVCEventTarget(&eventDecreaseBrake_);
}


bco::RotarySwitch& AirBrake::AirBrakeSwitch()
{
	return airBrakeSwitch_;
}

double AirBrake::GetAirBrakeState()
{
	return animAirBrake_.GetState();
}

void AirBrake::Step(double simt, double simdt, double mjd)
{
	// Note:  The animAirBrake can only move if there is hydraulic power, that
	// is the actual air brake animation.  The animAirBrakeHandle_ is the air brake
	// handle in the cockpit and can move regardless of power, therefore it must
	// always get a piece of the time step.

	if ((nullptr != apu_) && (apu_->GetHydraulicLevel() > 0.8))
	{
		animAirBrake_.Step(airBrakeSwitch_.GetState(), simdt);
	}

	// Update drag.
	dragFactor_ = animAirBrake_.GetState();
//	sprintf(oapiDebugString(), "air brake: %+4.2f", dragFactor_);
}

bool AirBrake::OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
	if (_strnicmp(key, ConfigKey, 4) != 0)
	{
		return false;
	}

	int step;

	sscanf_s(configLine + 8, "%i", &step);

	airBrakeSwitch_.SetStep(step);
	animAirBrake_.SetState(airBrakeSwitch_.GetPosition());

	return true;
}

void AirBrake::OnSaveConfiguration(FILEHANDLE scn) const
{
	char cbuf[256];
	auto val = airBrakeSwitch_.GetStep();

	sprintf_s(cbuf, "%i", val);
	oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}

void AirBrake::OnSetClassCaps()
{
    auto vessel = GetBaseVessel();
    auto vcIdx = vessel->GetVCMeshIndex();
    auto exIdx = vessel->GetMainMeshIndex();

    auto aid = vessel->CreateVesselAnimation(&airBrakeSwitch_, 2.0);

    vessel->AddVesselAnimationComponent(aid, vcIdx, &gpBrakeHandle_);

   
    aid = vessel->CreateVesselAnimation(&animAirBrake_, 0.4);
    
    vessel->AddVesselAnimationComponent(aid, exIdx, &gpLeftTop_);
    vessel->AddVesselAnimationComponent(aid, exIdx, &gpLeftBottom_);
    vessel->AddVesselAnimationComponent(aid, exIdx, &gpRightTop_);
    vessel->AddVesselAnimationComponent(aid, exIdx, &gpRightBottom_);

	// Setup drag.
	GetBaseVessel()->CreateVariableDragElement(animAirBrake_.GetStatePtr(), 10.0, bm::main::BrakeDragPoint_location);
}