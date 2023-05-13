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
	virtual void OnSetClassCaps() override;
	virtual bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf) override { return false; }
	virtual bool OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	virtual void OnSaveConfiguration(FILEHANDLE scn) const override;

    void Step(double simt, double simdt, double mjd);

    // *** AirBrake ***
	void SetAPU(APU* apu) { apu_ = apu; }
	bco::RotarySwitch& AirBrakeSwitch();
	double AirBrake::GetAirBrakeState();

private:
	bco::RotarySwitch		airBrakeSwitch_;
    bco::VCEventTarget      eventIncreaseBrake_ { bm::vc::ABTargetIncrease_location, 0.01 };
    bco::VCEventTarget      eventDecreaseBrake_ { bm::vc::ABTargetDecrease_location, 0.01 };

	APU*					apu_;

	double					dragFactor_;

	const char*				ConfigKey = "AIRBRAKE";

    bco::Animation			animAirBrake_       {   &airBrakeSwitch_, 2.0 };

    bco::AnimationGroup     gpBrakeHandle_      {   { bm::vc::AirBrakeLever_id },
                                                    bm::vc::SpBrakeAxisRight_location, bm::vc::SpBrakeAxisLeft_location,
                                                    (58 * RAD),
                                                    0.0, 1.0 };

    bco::AnimationGroup     gpLeftTop_          {   { bm::main::ElevonPIT_id },
                                                    bm::main::AirBrakeAxisPTO_location, bm::main::AirBrakeAxisPTI_location,
                                                    (70 * RAD),
                                                    0.0, 1.0 
                                                };

    bco::AnimationGroup     gpLeftBottom_       {   { bm::main::ElevonPIB_id },
                                                    bm::main::AirBrakeAxisPTI_location, bm::main::AirBrakeAxisPTO_location,
                                                    (70 * RAD),
                                                    0.0, 1.0 
                                                };

    bco::AnimationGroup     gpRightTop_         {   { bm::main::ElevonSIT_id },
                                                    bm::main::AirBrakeAxisSTO_location, bm::main::AirBrakeAxisSTI_location,
                                                    (70 * RAD),
                                                    0.0, 1.0 
                                                };

    bco::AnimationGroup     gpRightBottom_      {   { bm::main::ElevonSIB_id },
                                                    bm::main::AirBrakeAxisSBI_location, bm::main::AirBrakeAxisSBO_location,
                                                    (70 * RAD),
                                                    0.0, 1.0 
                                                };
};