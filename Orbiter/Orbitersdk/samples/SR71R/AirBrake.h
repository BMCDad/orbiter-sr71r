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

#pragma once

#include "bc_orbiter\Component.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\RotarySwitch.h"
#include "APU.h"

#include "SR71r_mesh.h"

class VESSEL;

namespace bco = bc_orbiter;

/**
Model the air brake and its controls.

The air brake requires hydraulic pressure to function which comes from the APU.  See
the APU instructions on how to start the APU.

Configuration:
AIRBRAKE a
a = 0.0 position, 0.0 closed, 1.0 fully open
*/
class AirBrake :
	public bco::Component
{
public:
	AirBrake(bco::BaseVessel* vessel);

    // *** Component ***
	virtual void SetClassCaps() override;
	virtual bool VCRedrawEvent(int id, int event, SURFHANDLE surf) override { return false; }
	virtual bool LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	virtual void SaveConfiguration(FILEHANDLE scn) const override;

    void Step(double simt, double simdt, double mjd);

    // *** AirBrake ***
	void SetAPU(APU* apu) { apu_ = apu; }
	bco::RotarySwitch& AirBrakeSwitch();
	double AirBrake::GetAirBrakeState();

private:
	void SetSound();

	bco::RotarySwitch		airBrakeSwitch_ { _V(0.0, 0.0, 0.0), 0.0, GetBaseVessel() };
    bco::EventTarget        eventIncreaseBrake_ { bt_mesh::SR71rVC::ABTargetIncrease_location, 0.01 };
    bco::EventTarget        eventDecreaseBrake_ { bt_mesh::SR71rVC::ABTargetDecrease_location, 0.01 };

	APU*					apu_;

	double					dragFactor_;

	const char*				ConfigKey = "AIRBRAKE";

    bco::Animation			animAirBrake_       {   &airBrakeSwitch_, 2.0 };

    bco::AnimationGroup     gpBrakeHandle_      {   { bt_mesh::SR71rVC::AirBrakeLever_id },
                                                    bt_mesh::SR71rVC::SpBrakeAxisRight_location, bt_mesh::SR71rVC::SpBrakeAxisLeft_location,
                                                    (58 * RAD),
                                                    0.0, 1.0 };

    bco::AnimationGroup     gpLeftTop_          {   { bt_mesh::SR71r::ElevonPIT_id },
                                                    bt_mesh::SR71r::AirBrakeAxisPTO_location, bt_mesh::SR71r::AirBrakeAxisPTI_location,
                                                    (70 * RAD),
                                                    0.0, 1.0 
                                                };

    bco::AnimationGroup     gpLeftBottom_       {   { bt_mesh::SR71r::ElevonPIB_id },
                                                    bt_mesh::SR71r::AirBrakeAxisPTI_location, bt_mesh::SR71r::AirBrakeAxisPTO_location,
                                                    (70 * RAD),
                                                    0.0, 1.0 
                                                };

    bco::AnimationGroup     gpRightTop_         {   { bt_mesh::SR71r::ElevonSIT_id },
                                                    bt_mesh::SR71r::AirBrakeAxisSTO_location, bt_mesh::SR71r::AirBrakeAxisSTI_location,
                                                    (70 * RAD),
                                                    0.0, 1.0 
                                                };

    bco::AnimationGroup     gpRightBottom_      {   { bt_mesh::SR71r::ElevonSIB_id },
                                                    bt_mesh::SR71r::AirBrakeAxisSBI_location, bt_mesh::SR71r::AirBrakeAxisSBO_location,
                                                    (70 * RAD),
                                                    0.0, 1.0 
                                                };
};