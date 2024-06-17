//  VCEvent - bco Orbiter Library
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
    VCEvent
    Provides a VC event.  Requires a VECTOR location and hit radius.
    */
    class VCEvent :
        public control,
        public vc_event_target,
        public signaller
    {
    public:
        VCEvent(const VECTOR3& vcLocation, double hitRadius) :
            location_(vcLocation),
            hitRadius_(hitRadius)
        { }

        // vc_event_target
        VECTOR3             vc_event_location()         override { return location_; }
        double              vc_event_radius()           override { return hitRadius_; }
        int                 vc_mouse_flags()            override { return PANEL_MOUSE_LBDOWN; }
        int                 vc_redraw_flags()           override { return PANEL_REDRAW_NEVER; }

        // event_target
        bool on_event(int id, int event) override {
            fire();
            return true;
        }

    private:
        const VECTOR3& location_;
        double hitRadius_;
    };
}
