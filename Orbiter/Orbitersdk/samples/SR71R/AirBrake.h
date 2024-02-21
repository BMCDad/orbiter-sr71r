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

#include "bc_orbiter/vessel.h"
#include "bc_orbiter/Component.h"
#include "bc_orbiter/Animation.h"
#include "bc_orbiter/control.h"
#include "bc_orbiter/simple_event.h"
#include "bc_orbiter/status_display.h"

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
class AirBrake : public bco::VesselComponent, public bco::HandlesSetClassCaps , public bco::HandlesPostStep, public bco::HandlesState {
 public:
  AirBrake(bco::Vessel& Vessel, bco::HydraulicProvider& apu);

  // set_class_caps
  void HandleSetClassCaps(bco::Vessel& Vessel) override;

  // post_step
  void HandlePostStep(bco::Vessel& Vessel, double simt, double simdt, double mjd) override;

  // manage_state
  bool HandleLoadState(bco::Vessel& Vessel, const std::string& line) override;
  std::string HandleSaveState(bco::Vessel& Vessel) override;

  void IncreaseDrag() { position_ = min(1.0, position_ + 0.33); }
  void DecreaseDrag() { position_ = max(0.0, position_ - 0.33); }

 private:
  bco::HydraulicProvider& apu_;

  double dragFactor_;
  double position_ { 0.0 };

  // Animations:  animSurface is only active when we have hydraulic power, the external surface animations
  //              key off of that, as well as the drag factor.  animSwitch will show the desired state regardless
  //              of hydraulic power.  The vc and panel switches key off of that.

  bco::animation_target animBrakeSurface_;
  bco::animation_target animBrakeSwitch_ { 2.0 };
  bco::animation_target animAirBrake_ { 2.0 };

  bco::AnimationGroup gpBrakeHandle_ {
      { bm::vc::AirBrakeLever_id },
      bm::vc::SpBrakeAxisRight_loc, bm::vc::SpBrakeAxisLeft_loc,
      (58 * RAD),
      0.0, 1.0 
  };

  bco::AnimationGroup gpLeftTop_ {
      { bm::main::ElevonPIT_id },
      bm::main::AirBrakeAxisPTO_loc, bm::main::AirBrakeAxisPTI_loc,
      (70 * RAD),
      0.0, 1.0 
  };

  bco::AnimationGroup gpLeftBottom_ {
      { bm::main::ElevonPIB_id },
      bm::main::AirBrakeAxisPTI_loc, bm::main::AirBrakeAxisPTO_loc,
      (70 * RAD),
      0.0, 1.0
  };

  bco::AnimationGroup gpRightTop_ {
      { bm::main::ElevonSIT_id },
      bm::main::AirBrakeAxisSTO_loc, bm::main::AirBrakeAxisSTI_loc,
      (70 * RAD),
      0.0, 1.0
  };

  bco::AnimationGroup gpRightBottom_{
      { bm::main::ElevonSIB_id },
      bm::main::AirBrakeAxisSBI_loc, bm::main::AirBrakeAxisSBO_loc,
      (70 * RAD),
      0.0, 1.0
  };

  // Panel
  const VECTOR3 sTrans { bm::pnl::pnlSpeedBrakeFull_loc - bm::pnl::pnlSpeedBrakeOff_loc };

  bco::SimpleEvent<> btnDecreaseAirbrake_ {
      bm::vc::ABTargetDecrease_loc,
      0.01,
      bm::pnl::pnlAirBrakeDecrease_RC
  };

  bco::SimpleEvent<> btnIncreaseAirbrake_{
      bm::vc::ABTargetIncrease_loc,
      0.01,
      bm::pnl::pnlAirBrakeIncrease_RC
  };

  bco::status_display status_  {
      bm::vc::MsgLightSpeedBrake_id,
      bm::vc::MsgLightSpeedBrake_vrt,
      bm::pnl::pnlMsgLightSpeedBrake_id,
      bm::pnl::pnlMsgLightSpeedBrake_vrt,
      0.0361
  };
};