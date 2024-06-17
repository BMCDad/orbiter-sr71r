//  PanelEvent - bco Orbiter Library
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

namespace bc_orbiter {

    /**
    PanelEvent
    Provides a panel event.  An event requires a RECT which defines the hit location,
    the panel ID, and a function to respond with.
    */
    class PanelEvent :
        public control,
        public panel_event_target,
        public signaller
    {
    public:
        PanelEvent(const RECT& pnl, const int pnlId) :
            pnlRect_(pnl),
            pnlId_(pnlId)
        { }

        // panel_event_target
        RECT                panel_rect()                override { return pnlRect_; }
        int                 panel_mouse_flags()         override { return PANEL_MOUSE_LBDOWN; }
        int                 panel_redraw_flags()        override { return PANEL_REDRAW_NEVER; }
        int                 panel_id()                  override { return pnlId_; }

        // event_target
        bool on_event(int id, int event) override {
            fire();
            return true;
        }

    private:
        RECT                pnlRect_;
        int                 pnlId_;
    };
}
