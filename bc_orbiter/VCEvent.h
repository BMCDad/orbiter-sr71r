//  VCEvent - bco Orbiter Library
//  Copyright(C) 2025  Blake Christensen
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

#include "Control.h"

namespace bc_orbiter {

    /**
    */
    class VCEvent : public VCEventTarget
    {
    public:
        VCEvent(const VECTOR3& vcLocation, double vcRadius, int vcId)
          : vcLocation_(vcLocation),
            vcRadius_(vcRadius),
            vcId_(vcId)
        {}

        // VCEventTarget
        VECTOR3     VCEventLocation()   override { return vcLocation_; }
        double      VCEventRadius()     override { return vcRadius_; }
        int         VCMouseFlags()      override { return PANEL_MOUSE_LBDOWN; }
        int         VCRedrawFlags()     override { return PANEL_REDRAW_NEVER; }
        int         VCId()              override { return vcId_; }

        // MouseEventTarget
        bool OnMouseClick(VESSEL4& vessel, int id, int event) override {
            if (func_) func_(vessel);
            return true;
        }

        void Attach(funcVesselEvent event) {
            func_ = event;
        }

    private:
        VECTOR3     vcLocation_;
        double      vcRadius_;
        int         vcId_{ 0 };
        funcVesselEvent   func_{ nullptr };
    };
}
