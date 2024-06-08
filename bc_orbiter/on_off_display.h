//	on_off_display - bco Orbiter Library
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

#include "Control.h"

#include <functional>

namespace bc_orbiter {

    /*
    on_off_display
    Creates a visual only binary state control that can be used in VC and panel UIs.  
    For both VC and Panel, the class takes an NTVERTEX array that is assumed to have 4 entries
    and represents a rectangle.  The UV texture for that rectangle is shifted to the right in the 
    'x' axis 'offset' amount to move from OFF to ON.
    The state of the UI is control via a slot input.
    **/
    class on_off_display :
        public control,
        public vc_event_target,
        public panel_event_target
    {
    public:
        on_off_display(
            const UINT vcGroupId,
            const NTVERTEX* vcVerts,
            const UINT pnlGroupId,
            const NTVERTEX* pnlVerts,
            double offset,
            int pnlId = 0
        ):  
            control(-1),
            vcGroupId_(vcGroupId),
            vcVerts_(vcVerts),
            pnlGroupId_(pnlGroupId),
            pnlVerts_(pnlVerts),
            offset_(offset),
            pnlId_(pnlId)
        { }

        void on_vc_redraw(DEVMESHHANDLE vcMesh) override {
            NTVERTEX* delta = new NTVERTEX[4];

            TransformUV2d(vcVerts_, delta, 4, _V(state_ ? offset_ : 0.0, 0.0, 0.0), 0.0);

            GROUPEDITSPEC change{};
            change.flags = GRPEDIT_VTXTEX;
            change.nVtx = 4;
            change.vIdx = NULL; //Just use the mesh order
            change.Vtx = delta;
            auto res = oapiEditMeshGroup(vcMesh, vcGroupId_, &change);
            delete[] delta;
        }

        void on_panel_redraw(MESHHANDLE meshPanel) override {
            DrawPanelOnOff(meshPanel, pnlGroupId_, pnlVerts_, state_, offset_);
        }

        int vc_mouse_flags() { return PANEL_MOUSE_IGNORE; }
        int vc_redraw_flags() { return PANEL_REDRAW_USER; }
        int panel_mouse_flags() { return PANEL_MOUSE_IGNORE; }
        int panel_redraw_flags() { return PANEL_REDRAW_USER; }

        void set_state(bool s) {
            state_ = s;
            oapiTriggerRedrawArea(pnlId_, 0, get_id());
        }

        int panel_id() override { return pnlId_; }
    private:
        UINT                vcGroupId_;
        const NTVERTEX*     vcVerts_;
        UINT                pnlGroupId_;
        int                 pnlId_;
        const NTVERTEX*     pnlVerts_;
        bool                state_{ false };
        double              offset_{ 0.0 };
    };
}