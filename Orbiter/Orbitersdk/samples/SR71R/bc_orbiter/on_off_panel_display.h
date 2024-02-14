//	on_off_panel_display - bco Orbiter Library
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
OnOffPanelDisplay
Creates a visual only binary state control that can be used in a 2d panel UI.
Takes an NTVERTEX array that is assumed to have 4 entries and represents a rectangle.  
The UV texture for that rectangle is shifted to the right in the 'x' axis 'offset' amount 
to move from OFF to ON.
The state of the UI is control via a slot input.
**/
class OnOffPanelDisplay : public Control, public PanelEventTarget
{
  public:
  OnOffPanelDisplay(const UINT pnlGroupId, const NTVERTEX* pnlVerts, double offset, signal<bool>& sig) : 
      Control(-1),
      pnlGroupId_(pnlGroupId),
      pnlVerts_(pnlVerts),
      offset_(offset),
      sl_state_([&](bool v) { oapiTriggerRedrawArea(0, 0, GetId()); }) {	
    sig.attach(sl_state_);
  }

  void OnPanelRedraw(MESHHANDLE meshPanel) override {
    DrawPanelOnOff(meshPanel, pnlGroupId_, pnlVerts_, sl_state_.value(), offset_);
  }

  int PanelMouseFlags() { return PANEL_MOUSE_IGNORE; }  // visual only, no mouse input.
  int PanelRedrawFlags() { return PANEL_REDRAW_USER; }

  private:
  UINT pnlGroupId_;
  const NTVERTEX* pnlVerts_;
  double offset_{ 0.0 };
  slot<bool> sl_state_;
};
} // namespace bc_orbiter