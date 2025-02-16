//	panel_display - bco Orbiter Library
//	Copyright(C) 2023  Blake Christensen
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

#include "control.h"
#include "Tools.h"

namespace bc_orbiter {

    /*
    panel_display
    Controls a display only state (no event) for 2D panels.  By convention state is the
    offset in the texture to the right.  Which means a state of 0 is the 'rest' state.
    A state of 1 moves the 'U' setting of the texture to the right one 'width' of the the
    rectangle defined by NTVERTEX.
    **/
    class display_panel :
        public panel_event_target {
    public:
        display_panel(
            const UINT pnlGroupId,
            const NTVERTEX* pnlVerts,
            const int panelId,
            funcState func
        ) : pnlGroupId_(pnlGroupId),
            pnlVerts_(pnlVerts),
            offset_(UVOffset(pnlVerts)),
            pnlId_(panelId),
            funcState_(func)
        {
        }

        void on_panel_redraw(MESHHANDLE meshPanel) override {
            DrawPanelOffset(meshPanel, pnlGroupId_, pnlVerts_, offset_ * funcState_());
        }

        int panel_mouse_flags()     override { return PANEL_MOUSE_IGNORE; }
        int panel_redraw_flags()    override { return PANEL_REDRAW_USER; }
        int panel_id()              override { return pnlId_; }

    protected:
        void trigger_redraw(VESSEL4& vessel, int ctrlId) const {
            vessel.TriggerPanelRedrawArea(pnlId_, ctrlId);
        }

    private:
        UINT            pnlGroupId_;
        const NTVERTEX* pnlVerts_;
        UINT            pnlId_;

        funcState       funcState_{ [&] {return 0.0; } };
        double          offset_{ 0.0 };
    };


    using display_panel_control = display_control_base<display_panel>;

    //class panel_display_control :
    //    public control,
    //    public display_panel
    //{
    //public:
    //    panel_display_control(
    //        const UINT pnlGroupId,
    //        const NTVERTEX* pnlVerts,
    //        const int panelId
    //    ) : display_panel(
    //        pnlGroupId, 
    //        pnlVerts, 
    //        panelId, 
    //        [&] {return state_; }
    //    )
    //    { }

    //    void set_state(VESSEL4& vessel, bool s) {
    //        if (set_state(s))
    //            display_panel::trigger_redraw(vessel, get_id());
    //    }

    //protected:

    //    /// <summary>
    //    /// Set the state of the control.
    //    /// </summary>
    //    /// <param name="state">New state</param>
    //    /// <returns>True if the state changed</returns>
    //    bool set_state(double state) {
    //        if (state_ != state) {
    //            state_ = state;
    //            return true;
    //        }
    //        state_ = state;
    //        return false;
    //    }

    //    /// <summary>
    //    /// Set the state of the control.
    //    /// </summary>
    //    /// <param name="state">New state</param>
    //    /// <returns>True if the state changed</returns>
    //    bool set_state(bool s) {
    //        return set_state(s ? 1.0 : 0.0);
    //    }

    //private:
    //    double  state_{ 0.0 };
    //};
}
