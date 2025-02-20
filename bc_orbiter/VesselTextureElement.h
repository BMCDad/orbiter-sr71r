//	VesselTextureElement - bco Orbiter Library
//	Copyright(C) 2025  Blake Christensen
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

#include "Control.h"
#include "VCTextureElement.h"
#include "PanelTextureElement.h"
#include <functional>

namespace bc_orbiter {

    /*
    Combines a VCTextureElement and PanelTextureElement into a single Control.  This allows the Control
    state to be shared between VC and panel cockpits.
    **/
    class VesselTextureElement :
        public Control,
        public VCTextureElement,
        public PanelTextureElement
    {
    public:
        VesselTextureElement(
            const UINT vcGroupId,
            const NTVERTEX* vcVerts,
            const int vcId,
            const UINT pnlGroupId,
            const NTVERTEX* pnlVerts,
            const int panelId
        ) : VCTextureElement(vcGroupId, vcVerts, vcId, [&] {return state_; }),
            PanelTextureElement(pnlGroupId, pnlVerts, panelId, [&] {return state_; })
        {
        }
        void SetState(VESSEL4& vessel, bool s) {
            if (SetState(s ? 1.0 : 0.0)) {
                trigger(vessel);
            }
        }

        void SetState(VESSEL4& vessel, double s) {
            if (SetState(s))
                trigger(vessel);
        }

    protected:

        /// <summary>
        /// Set the state of the Control.
        /// </summary>
        /// <param name="state">New state</param>
        /// <returns>True if the state changed</returns>
        bool SetState(double state) {
            if (state_ != state) {
                state_ = state;
                return true;
            }
            state_ = state;
            return false;
        }

    private:
        void trigger(VESSEL4& vessel) {
            VCTextureElement::triggerRedraw(vessel, GetId());
            PanelTextureElement::triggerRedraw(vessel, GetId());
        }

        double  state_{ 0.0 };
    };
}


