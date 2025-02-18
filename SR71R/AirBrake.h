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

#include "../bc_orbiter/Animation.h"
#include "../bc_orbiter/control.h"
#include "../bc_orbiter/SimpleEvent.h"
#include "../bc_orbiter/Vessel.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"
#include "SR71rPanelRight_mesh.h"

#include "Common.h"

class VESSEL;

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

/**
Model the air brake and its controls.

The air brake requires hydraulic pressure to function which comes from the APU.  See
the APU instructions on how to start the APU.

Configuration:
AIRBRAKE a
a = 0.0 position, 0.0 closed, 1.0 fully open
*/
class AirBrake :
	  public bco::VesselComponent
    , public bco::SetClassCaps 
    , public bco::PostStep
    , public bco::ManageState
{
public:
	AirBrake(bco::Vessel& vessel, bco::HydraulicProvider& apu);

    // SetClassCaps
    void HandleSetClassCaps(bco::Vessel& vessel) override;

    // PostStep
    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override;

    // ManageState
    bool HandleLoadState(bco::Vessel& vessel, const std::string& line) override;
    std::string HandleSaveState(bco::Vessel& vessel) override;

    void IncreaseDrag() { position_ = min(1.0, position_ + 0.33); }
    void DecreaseDrag() { position_ = max(0.0, position_ - 0.33); }

private:

    bco::HydraulicProvider& apu_;

	double					dragFactor_;
    double                  position_           { 0.0 };

    // Animations:  animSurface is only active when we have hydraulic power, the external surface animations
    //              key off of that, as well as the drag factor.  animSwitch will show the desired state regardless
    //              of hydraulic power.  The vc and panel switches key off of that.

    bco::AnimationTarget   animBrakeSurface_;
    bco::AnimationTarget   animBrakeSwitch_    {   2.0 };
    bco::AnimationTarget   animAirBrake_       {   2.0 };

    bco::AnimationGroup    gpBrakeHandle_     {   { bm::vc::AirBrakeLever_id },
                                                    bm::vc::SpBrakeAxisRight_loc, bm::vc::SpBrakeAxisLeft_loc,
                                                    (58 * RAD),
                                                    0.0, 1.0 };

    bco::AnimationGroup     gpLeftTop_         {   { bm::main::ElevonPIT_id },
                                                    bm::main::AirBrakeAxisPTO_loc, bm::main::AirBrakeAxisPTI_loc,
                                                    (70 * RAD),
                                                    0.0, 1.0 
                                                };

    bco::AnimationGroup     gpLeftBottom_      {   { bm::main::ElevonPIB_id },
                                                    bm::main::AirBrakeAxisPTI_loc, bm::main::AirBrakeAxisPTO_loc,
                                                    (70 * RAD),
                                                    0.0, 1.0 
                                                };

    bco::AnimationGroup     gpRightTop_        {   { bm::main::ElevonSIT_id },
                                                    bm::main::AirBrakeAxisSTO_loc, bm::main::AirBrakeAxisSTI_loc,
                                                    (70 * RAD),
                                                    0.0, 1.0 
                                                };

    bco::AnimationGroup     gpRightBottom_     {   { bm::main::ElevonSIB_id },
                                                    bm::main::AirBrakeAxisSBI_loc, bm::main::AirBrakeAxisSBO_loc,
                                                    (70 * RAD),
                                                    0.0, 1.0 
                                                };

    // Panel
    const VECTOR3 sTrans { bm::pnl::pnlSpeedBrakeFull_loc - bm::pnl::pnlSpeedBrakeOff_loc };

    bco::SimpleEvent<>		btnDecreaseAirbrake_ {  
        bm::vc::ABTargetDecrease_loc,
        0.01,
        0,
        bm::pnl::pnlAirBrakeDecrease_RC,
        0
    };

    bco::SimpleEvent<>		btnIncreaseAirbrake_{ 
        bm::vc::ABTargetIncrease_loc,
        0.01,
        0,
        bm::pnl::pnlAirBrakeIncrease_RC,
        0
    };

    bco::VesselTextureElement       status_ {
        bm::vc::MsgLightSpeedBrake_id,
        bm::vc::MsgLightSpeedBrake_vrt,
        cmn::vc::main,
        bm::pnl::pnlMsgLightSpeedBrake_id,
        bm::pnl::pnlMsgLightSpeedBrake_vrt,
        cmn::panel::main
    };
};