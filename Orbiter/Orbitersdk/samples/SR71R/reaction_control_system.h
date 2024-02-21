//	reaction_control_system - SR-71r Orbiter Addon
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

#include "SR71r_mesh.h"

#include "bc_orbiter/control.h"
#include "bc_orbiter/simple_event.h"
#include "bc_orbiter/on_off_vc_display.h"
#include "bc_orbiter/on_off_panel_display.h"
#include "bc_orbiter/signals.h"

class VESSEL3;

class ReactionControlSystem : public bc_orbiter::VesselComponent, public bc_orbiter::PowerConsumer {
 public:
  ReactionControlSystem(bc_orbiter::Vessel& Vessel, bc_orbiter::PowerProvider& pwr);

  // Callback:
  void OnRCSMode(int mode);

  double AmpDraw() const override { return IsPowered() ? 2.0 : 0.0; }
  void OnChange(double v) override { }

 private:
  bool IsPowered() const { return power_.VoltsAvailable() > 24; }
  void OnChanged(int mode);
  void ActiveChanged(bool isActive);

  const double kTexOffset = 0.0352;

  bc_orbiter::Vessel& vessel_;
  bc_orbiter::PowerProvider& power_;

  bc_orbiter::Signal<bool> sig_linear_;
  bc_orbiter::SimpleEvent<> btnLinear_ { bm::vc::vcRCSLin_loc, 0.01, bm::pnl::pnlRCSLin_RC };
  bc_orbiter::OnOffPanelDisplay pnl_lightLinear_ { bm::pnl::pnlRCSLin_id, bm::pnl::pnlRCSLin_vrt, kTexOffset, sig_linear_ };
  bc_orbiter::OnOffVCDisplay vc_lightLinear_ { bm::vc::vcRCSLin_id, bm::vc::vcRCSLin_vrt, kTexOffset, sig_linear_ };

  bc_orbiter::Signal<bool> sig_rotate_;
  bc_orbiter::SimpleEvent<> btnRotate_ { bm::vc::vcRCSRot_loc, 0.01, bm::pnl::pnlRCSRot_RC };
  bc_orbiter::OnOffPanelDisplay pnl_lightRotate_ { bm::pnl::pnlRCSRot_id, bm::pnl::pnlRCSRot_vrt, kTexOffset, sig_rotate_ };
  bc_orbiter::OnOffVCDisplay vc_lightRotate_ { bm::vc::vcRCSRot_id, bm::vc::vcRCSRot_vrt, kTexOffset, sig_rotate_ };
};