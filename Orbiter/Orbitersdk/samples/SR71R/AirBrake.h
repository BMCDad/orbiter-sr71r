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
	double AirBrake::GetAirBrakeState();

    bco::slot<bool>&    IncreaseSlot()          { return increaseSlot_; }
    bco::slot<bool>&    DecreaseSlot()          { return decreaseSlot_; }
    bco::slot<double>&  HydraulicPressSlot()    { return hydraulicPressSlot_; }

private:

    bco::slot<bool>     increaseSlot_;
    bco::slot<bool>     decreaseSlot_;
    bco::slot<double>   hydraulicPressSlot_;

	double					dragFactor_;
    double                  position_{ 0.0 };

	const char*				ConfigKey = "AIRBRAKE";

    // Animations:  animSurface is only active when we have hydraulic power, the external surface animations
    //              key off of that, as well as the drag factor.  animSwitch will show the desired state regardless
    //              of hydraulic power.  The vc and panel switches key off of that.

    bco::Animation          animBrakeSurface_;
    bco::Animation          animBrakeSwitch_    {   2.0 };
    bco::Animation			animAirBrake_       {   2.0 };

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

    // Panel
    const VECTOR3 sTrans { bm::pnl::pnlSpeedBrakeFull_location - bm::pnl::pnlSpeedBrakeOff_location };
};