//  VesselEvent - bco Orbiter Library
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
#include "VCEvent.h"
#include "PanelEvent.h"

namespace bc_orbiter {

    

    /**
    */
    class VesselEvent
      : public Control,
        public VCEvent,
        public PanelEvent
    {
    public:
        VesselEvent(
            const VECTOR3& vcLocation,
            double vcRadius,
            int vcId,
            const RECT& pnlRect,
            int pnlId)
          : Control(-1),
            VCEvent(vcLocation, vcRadius, vcId),
            PanelEvent(pnlRect, pnlId)
        {}

        void Attach(funcVesselEvent event) {
            VCEvent::Attach(event);
            PanelEvent::Attach(event);
        }

    private:
    };
}
