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

AirBrake::AirBrake(bco::Vessel& Vessel, bco::HydraulicProvider& apu) : dragFactor_(0.0), apu_(apu) {
  Vessel.AddControl(&btnDecreaseAirbrake_);
  Vessel.AddControl(&btnIncreaseAirbrake_);
  Vessel.AddControl(&status_);

  btnIncreaseAirbrake_.Attach([&]() { IncreaseDrag(); });
  btnDecreaseAirbrake_.Attach([&]() { DecreaseDrag(); });
}

void AirBrake::HandlePostStep(bco::Vessel& Vessel, double simt, double simdt, double mjd) {
  // Note:  The animAirBrake can only move if there is hydraulic power, that
  // is the actual air brake animation.  The animAirBrakeHandle_ is the air brake
  // handle in the cockpit and can move regardless of power, therefore it must
  // always get a piece of the time step.

  if (apu_.Level() > 0.8) {
    animAirBrake_.Step(position_, simdt);
  }

  animBrakeSwitch_.Step(position_, simdt);

  // Update drag.
  dragFactor_ = animAirBrake_.GetState();
  //	sprintf(oapiDebugString(), "air brake: %+4.2f", dragFactor_);

  // This needs to be put into a switch statement eventually
  bco::TranslateMesh(Vessel.GetpanelMeshHandle0(), bm::pnl::pnlAirBrake_id, bm::pnl::pnlAirBrake_vrt, sTrans * animBrakeSwitch_.GetState());

  status_.set_state(dragFactor_ > 0.05 ? bco::status_display::status::warn : bco::status_display::status::off);
}

bool AirBrake::HandleLoadState(bco::Vessel& Vessel, const std::string& line) {
  // [a b] : a: air brake switch position.   b: air brake actual position (they can differ)

  std::istringstream in(line);
  in >> position_ >> animAirBrake_;
	
  Vessel.SetAnimationState(animAirBrake_);
  return true;
}

std::string AirBrake::HandleSaveState(bco::Vessel& Vessel) {
  std::ostringstream os;
  os << position_ << " " << animAirBrake_;
  return os.str();
}

void AirBrake::HandleSetClassCaps(bco::Vessel& Vessel) {
  // Setup VC animation
  auto vcIdx = Vessel.GetVCMeshIndex();
  auto aid = Vessel.CreateVesselAnimation(&animBrakeSwitch_, 2.0);
  animBrakeSurface_.VesselId(aid);
  Vessel.AddVesselAnimationComponent(aid, vcIdx, &gpBrakeHandle_);

  // Setup external animation   
  auto exIdx = Vessel.GetMainMeshIndex();
  aid = Vessel.CreateVesselAnimation(&animAirBrake_, 0.4);
  Vessel.AddVesselAnimationComponent(aid, exIdx, &gpLeftTop_);
  Vessel.AddVesselAnimationComponent(aid, exIdx, &gpLeftBottom_);
  Vessel.AddVesselAnimationComponent(aid, exIdx, &gpRightTop_);
  Vessel.AddVesselAnimationComponent(aid, exIdx, &gpRightBottom_);
  animAirBrake_.VesselId(aid);

  // Setup drag.
  Vessel.CreateVariableDragElement(animAirBrake_.GetStatePtr(), 10.0, bm::main::BrakeDragPoint_loc);
}
