//  PanelDisplay - bco Orbiter Library
//  Copyright(C) 2024  Blake Christensen
//  
//  This program is free software : you can redistribute it and / or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "control.h"
#include "signal.h"

namespace bc_orbiter {

    /**
    PanelDisplay
    Provides a panel display that will alter its visual by translating the 'U' uv value.
    By convention the UV is laid out left to right, each visual offset the same as the 
    UV 'width' from a quad NVERTEX array input.  This means 0 is the unwrapped state,
    1 is one width to the right, 2 is two widths etc.
    */
    template<typename T = double>
    class PanelDisplay : public control, public panel_event_target
    {
    public:
        PanelDisplay(const UINT pnlGroup, const NTVERTEX* pnlVerts, const int pnlId) :
            pnlGroup_(pnlGroup),
            pnlVerts_(pnlVerts),
            pnlId_(pnlId),
            offset_(UVOffset(pnlVerts))
        { }

        // panel_event_target
        int panel_mouse_flags()     override { return PANEL_MOUSE_IGNORE; }
        int panel_redraw_flags()    override { return PANEL_REDRAW_USER; }
        int panel_id()              override { return pnlId_; }

        void on_panel_redraw(MESHHANDLE meshPanel) override {
            double offs = static_cast<T>(state_.value());
            DrawPanelOffset(meshPanel, pnlGroup_, pnlVerts_, offs * offset_);
        }

        slot<T>& Slot() { return state_; }
    private:
        const UINT      pnlGroup_;
        const NTVERTEX* pnlVerts_;
        int             pnlId_;
        double          offset_;
        slot<T>         state_{ [&](T v) { oapiTriggerPanelRedrawArea(pnlId_, get_id()); } };
    };
}
