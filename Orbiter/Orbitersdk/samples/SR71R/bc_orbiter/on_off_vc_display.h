//	on_off_vc_display - bco Orbiter Library
//	Copyright(C) 2024  Blake Christensen
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

#include "control.h"
#include "signal.h"
#include <functional>

namespace bc_orbiter {

/*
OnOffVCDisplay
Creates a visual only binary state control that can be used in VC UIs. This class takes an NTVERTEX 
array that is assumed to have 4 entries and represents a rectangle.  The UV texture for that rectangle 
is shifted to the right in the 'x' axis 'offset' amount to move from OFF to ON.
The state of the UI is control via a slot input.
**/
class OnOffVCDisplay : public Control, public VCEventTarget {
public:
  OnOffVCDisplay(const UINT vcGroupId, const NTVERTEX* vcVerts, double offset, signal<bool>& sig) :
      Control(-1),
      vcGroupId_(vcGroupId),
      vcVerts_(vcVerts),
      offset_(offset),
      sl_state_([&](bool v) { oapiTriggerRedrawArea(0, 0, GetId()); }) { 
    sig.attach(sl_state_);
  }

void OnVCRedraw(DEVMESHHANDLE vcMesh) override {
  NTVERTEX* delta = new NTVERTEX[4];

  TransformUV2d(
    vcVerts_,
    delta, 4,
    _V(sl_state_.value() ? offset_ : 0.0, 0.0, 0.0),
    0.0);

  GROUPEDITSPEC change{};
  change.flags = GRPEDIT_VTXTEX;
  change.nVtx = 4;
  change.vIdx = NULL; //Just use the mesh order
  change.Vtx = delta;
  auto res = oapiEditMeshGroup(vcMesh, vcGroupId_, &change);
  delete[] delta;
}

int VCMouseFlags() { return PANEL_MOUSE_IGNORE; } // Visual only, no mouse input.
int VCRedrawFlags() { return PANEL_REDRAW_USER; }

private:
  UINT vcGroupId_;
  const NTVERTEX* vcVerts_;
  double offset_{ 0.0 };
  slot<bool> sl_state_;
};
} // namespace bc_orbiter