//	state_display - bco Orbiter Library
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

#include <functional>

namespace bc_orbiter {

    /*
    state_display
    Controls a display only state (no event) for 2D panels and VCs.  By convention state is the
    offset in the texture to the right.  Which means a state of 0 is the 'rest' state.
    A state of 1 moves the 'U' setting of the texture to the right one 'width' of the the
    rectangle defined by NTVERTEX.  This control also assumes the same texture will be used for
    both panel and VC, meaning the offset that is calculated for the VC will also be used for
    the panel.
    **/
    class state_display :
        public control,
        public vc_event_target,
        public panel_event_target
    {
    public:
        state_display(
            const UINT vcGroupId,
            const NTVERTEX* vcVerts,
            const int vcId,
            const UINT pnlGroupId,
            const NTVERTEX* pnlVerts,
            const int panelId
        ) : vcGroupId_(vcGroupId),
            vcVerts_(vcVerts),
            pnlGroupId_(pnlGroupId),
            pnlVerts_(pnlVerts),
            offset_(UVOffset(vcVerts)),
            vcId_(vcId),
            pnlId_(panelId)
        { }

        void on_vc_redraw(DEVMESHHANDLE vcMesh) override {
            DrawVCOffset(vcMesh, vcGroupId_, vcVerts_, state_ * offset_);
        }

        void on_panel_redraw(MESHHANDLE meshPanel) override {
            DrawPanelOffset(meshPanel, pnlGroupId_, pnlVerts_, offset_ * state_);
        }

        int vc_mouse_flags()        override { return PANEL_MOUSE_IGNORE; }
        int vc_redraw_flags()       override { return PANEL_REDRAW_USER; }
        int vc_id()                 override { return vcId_; }
        int panel_mouse_flags()     override { return PANEL_MOUSE_IGNORE; }
        int panel_redraw_flags()    override { return PANEL_REDRAW_USER; }
        int panel_id()              override { return pnlId_; }

        void set_state(VESSEL4& vessel, bool s) {
            if (set_state(s))
                vessel.TriggerRedrawArea(pnlId_, vcId_, get_id());
        }

        void set_status(VESSEL4& vessel, double s) {
            if (set_state(s))
                vessel.TriggerRedrawArea(pnlId_, vcId_, get_id());
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

        UINT            pnlGroupId_;
        const NTVERTEX* pnlVerts_;
        UINT            pnlId_;

        double          state_{ 0.0 };
        double          offset_{ 0.0 };
    };
}

