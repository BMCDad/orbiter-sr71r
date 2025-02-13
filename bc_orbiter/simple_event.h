//  simple_event - bco Orbiter Library
//  Copyright(C) 2023  Blake Christensen
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
    * simple_event
    * Provides a simple event only control.  No state is retained, and no UI change is made.
    * For the VC, a VECTOR position and hit radius are required.  For panel, a RECT is required.
    * The simple event is template driven and can specify the type of signal to fire, and the data fired.
    * This can be an id or data structure.
    */
    template<typename T = bool>
    class simple_event :
        public control,
        public vc_event_target,
        public panel_event_target
    {
    public:
        simple_event(
            const VECTOR3& vcLocation,
            double vcRadius,
            int vcId,
            const RECT& pnlRect,
            int pnlId,
            T data = T()
        ) :
            control(-1),
            vcLocation_(vcLocation),
            vcRadius_(vcRadius),
            vcId_(vcId),
            pnlRect_(pnlRect),
            data_(data),
            pnlId_(pnlId)
        { }

        // vc_event_target
        VECTOR3     vc_event_location()     override { return vcLocation_; }
        double      vc_event_radius()       override { return vcRadius_; }
        int         vc_mouse_flags()        override { return PANEL_MOUSE_LBDOWN; }
        int         vc_redraw_flags()       override { return PANEL_REDRAW_NEVER; }
        int         vc_id()                 override { return vcId_; }

        // panel_event_target
        RECT        panel_rect()            override { return pnlRect_; }
        int         panel_mouse_flags()     override { return PANEL_MOUSE_LBDOWN; }
        int         panel_redraw_flags()    override { return PANEL_REDRAW_NEVER; }
        int         panel_id()              override { return pnlId_; }

        // event_target
        bool on_event(int id, int event) override {
            func_();
            return true;
        }

        void attach(funcEvent event) {
            func_ = event;
        }

    private:
        T           data_;
        VECTOR3     vcLocation_;
        double      vcRadius_;
        int         vcId_{ 0 };
        RECT        pnlRect_;
        signal<T>   signal_;
        int         pnlId_;
        funcEvent   func_{ [] {} };
    };
}