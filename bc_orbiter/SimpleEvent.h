//  SimpleEvent - bco Orbiter Library
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
    * SimpleEvent
    * Provides a simple event only Control.  No state is retained, and no UI change is made.
    * For the VC, a VECTOR position and hit radius are required.  For panel, a RECT is required.
    * The simple event is template driven and can specify the type of signal to fire, and the data fired.
    * This can be an id or data structure.
    */
    template<typename T = bool>
    class SimpleEvent 
      : public Control,
        public VCEventTarget,
        public PanelEventTarget
    {
    public:
        SimpleEvent(
            const VECTOR3& vcLocation,
            double vcRadius,
            int vcId,
            const RECT& pnlRect,
            int pnlId,
            T data = T())
          : Control(-1),
            vcLocation_(vcLocation),
            vcRadius_(vcRadius),
            vcId_(vcId),
            pnlRect_(pnlRect),
            data_(data),
            pnlId_(pnlId)
        { }

        // VCEventTarget
        VECTOR3     VCEventLocation()   override { return vcLocation_; }
        double      VCEventRadius()     override { return vcRadius_; }
        int         VCMouseFlags()      override { return PANEL_MOUSE_LBDOWN; }
        int         VCRedrawFlags()     override { return PANEL_REDRAW_NEVER; }
        int         VCId()              override { return vcId_; }

        // PanelEventTarget
        RECT        PanelRect()         override { return pnlRect_; }
        int         PanelMouseFlags()   override { return PANEL_MOUSE_LBDOWN; }
        int         PanelRedrawFlags()  override { return PANEL_REDRAW_NEVER; }
        int         PanelId()           override { return pnlId_; }

        // MouseEventTarget
        bool OnMouseClick(int id, int event) override {
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