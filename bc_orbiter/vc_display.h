//  VCDisplay - bco Orbiter Library
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

#include "Control.h"

#include <functional>

namespace bc_orbiter {

    /*
    vc_display
    **/
    class vc_display :
        public vc_event_target
    {
    public:
        vc_display(
            const UINT vcGroupId,
            const NTVERTEX* vcVerts,
            const int vcId
        ) : vcGroupId_(vcGroupId),
            vcVerts_(vcVerts),
            offset_(UVOffset(vcVerts)),
            vcId_(vcId)
        {
        }

        void on_vc_redraw(DEVMESHHANDLE vcMesh) override {
            DrawVCOffset(vcMesh, vcGroupId_, vcVerts_, state_ * offset_);
        }

        int vc_mouse_flags()        override { return PANEL_MOUSE_IGNORE; }
        int vc_redraw_flags()       override { return PANEL_REDRAW_USER; }
        int vc_id()                 override { return vcId_; }

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

        /// <summary>
        /// Set the state of the control.
        /// </summary>
        /// <param name="state">New state</param>
        /// <returns>True if the state changed</returns>
        bool set_state(bool s) {
            return set_state(s ? 1.0 : 0.0);
        }

        void trigger_redraw(VESSEL4& vessel, int ctrlId) const {
            // TriggerVCRedrawArea has not been made public yet...
            vessel.TriggerRedrawArea(-1, vcId_, ctrlId);
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

        /// <summary>
        /// Set the state of the control.
        /// </summary>
        /// <param name="state">New state</param>
        /// <returns>True if the state changed</returns>
        bool set_state(bool s) {
            return set_state(s ? 1.0 : 0.0);
        }

    private:
        UINT            vcGroupId_;
        const NTVERTEX* vcVerts_;
        UINT            vcId_{ 0 };

        double          state_{ 0.0 };
        double          offset_{ 0.0 };
    };
}

