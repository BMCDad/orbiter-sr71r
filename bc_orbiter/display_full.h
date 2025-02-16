//	display_full - bco Orbiter Library
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
#include "display_vc.h"
#include "display_panel.h"
#include <functional>

namespace bc_orbiter {

    /*
    display_full
    Controls a display only state (no event) for 2D panels and VCs.  By convention state is the
    offset in the texture to the right.  Which means a state of 0 is the 'rest' state.
    A state of 1 moves the 'U' setting of the texture to the right one 'width' of the the
    rectangle defined by NTVERTEX.  This control also assumes the same texture will be used for
    both panel and VC, meaning the offset that is calculated for the VC will also be used for
    the panel.
    **/
    class display_full :
        public control,
        public display_vc,
        public display_panel
    {
    public:
        display_full(
            const UINT vcGroupId,
            const NTVERTEX* vcVerts,
            const int vcId,
            const UINT pnlGroupId,
            const NTVERTEX* pnlVerts,
            const int panelId
        ) : display_vc(vcGroupId, vcVerts, vcId, [&] {return state_; }),
            display_panel(pnlGroupId, pnlVerts, panelId, [&] {return state_; })
        {
        }
        void set_state(VESSEL4& vessel, bool s) {
            if (set_state(s ? 1.0 : 0.0)) {
                trigger(vessel);
            }
        }

        void set_state(VESSEL4& vessel, double s) {
            if (set_state(s))
                trigger(vessel);
        }

    protected:

        /// <summary>
        /// Set the state of the control.
        /// </summary>
        /// <param name="state">New state</param>
        /// <returns>True if the state changed</returns>
        bool set_state(double state) {
            if (state_ != state) {
                state_ = state;
                return true;
            }
            state_ = state;
            return false;
        }

    private:
        void trigger(VESSEL4& vessel) {
            display_vc::trigger_redraw(vessel, get_id());
            display_panel::trigger_redraw(vessel, get_id());
        }

        double  state_{ 0.0 };
    };
}


