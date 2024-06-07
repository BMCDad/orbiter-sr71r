//  on_off_display_event - bco Orbiter Library
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

#include <functional>

namespace bc_orbiter {

    /*
    on_off_display_event
    Creates a hibrid visual and mouse event binary state control that can be used in VC and panel UIs.
    For both VC and Panel, the class takes an NTVERTEX array that is assumed to have 4 entries
    and represents a rectangle.  The UV texture for that rectangle is shifted to the right in the
    'x' axis 'offset' amount to move from OFF to ON.
    The state of the UI is control via a slot input.
    **/
    class on_off_display_event : 
        public control, 
        public vc_event_target, 
        public panel_event_target {
    public:
        on_off_display_event(
            int ctrlId,
            const UINT vcGroupId,
            const VECTOR3& vcLocation,
            const NTVERTEX* vcVerts,
            const double vcRadius,
            const UINT pnlGroupId,
            const NTVERTEX* pnlVerts,
            const RECT& pnlRect,
            double offset
        ):  control(ctrlId),
            vcGroupId_(vcGroupId),
            vcLocation_(vcLocation),
            vcVerts_(vcVerts),
            vcRadius_(vcRadius),
            pnlGroupId_(pnlGroupId),
            pnlVerts_(pnlVerts),
            pnlRect_(pnlRect),
            offset_(offset),
            slotState_([&](double v) {
                if (state_ != v) {
                    state_ = v;
                    oapiTriggerRedrawArea(0, 0, get_id());
                }
            }
            ) { }

            // vc_event_target
            VECTOR3     vc_event_location()	override { return vcLocation_; }
            int         vc_mouse_flags()	override { return PANEL_MOUSE_LBDOWN; }
            int         vc_redraw_flags()	override { return PANEL_REDRAW_USER; }
            double      vc_event_radius()	override { return vcRadius_; }

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

            // panel_event_target
            void on_panel_redraw(MESHHANDLE meshPanel) override {
                DrawPanelOnOff(meshPanel, pnlGroupId_, pnlVerts_, state_, offset_);
            }

            RECT panel_rect()           override { return pnlRect_; }
            int panel_mouse_flags()     override { return PANEL_MOUSE_LBDOWN; }
            int panel_redraw_flags()    override { return PANEL_REDRAW_USER; }

            // event_target
            bool on_event(int id, int event) override {
                // Note: Event here does not alter state, that comes from the slot.
                // State is only altered through the slot.

                signal_.fire(!state_);
                return true;
            }

            // signal
            signal<bool>& Signal() { return signal_; }
            slot<bool>& Slot() { return slotState_; }
    private:
	    UINT			vcGroupId_;
	    VECTOR3			vcLocation_;
	    const NTVERTEX* vcVerts_;
	    double			vcRadius_;
	    UINT			pnlGroupId_;
	    const NTVERTEX* pnlVerts_;
	    RECT			pnlRect_;
	    bool			state_{ false };
	    double			offset_{ 0.0 };
	    slot<bool>		slotState_;
	    signal<bool>	signal_;
    };
}
