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

namespace bc_orbiter {

    /*
    VCDisplay
    A four state status display control intended for the status display panel.
    **/
    template<typename T = double>
    class VCDisplay : public control, public vc_event_target
    {
    public:

        VCDisplay(const UINT vcGroupId, const NTVERTEX* vcVerts) :
            control(-1),
            vcGroupId_(vcGroupId),
            vcVerts_(vcVerts),
            offset_(UVOffset(vcVerts))
        { }

        void on_vc_redraw(DEVMESHHANDLE vcMesh) override {
            NTVERTEX* delta = new NTVERTEX[4];

            TransformUV2d(
                vcVerts_,
                delta, 4,
                _V(state_.value() * offset_,
                    0.0,
                    0.0),
                0.0);

            GROUPEDITSPEC change{};
            change.flags = GRPEDIT_VTXTEX;
            change.nVtx = 4;
            change.vIdx = NULL; //Just use the mesh order
            change.Vtx = delta;
            auto res = oapiEditMeshGroup(vcMesh, vcGroupId_, &change);
            delete[] delta;
        }

        int vc_mouse_flags() { return PANEL_MOUSE_IGNORE; }
        int vc_redraw_flags() { return PANEL_REDRAW_USER; }

        slot<T>& Slot() { return state_; }

    private:
        UINT            vcGroupId_;
        const NTVERTEX* vcVerts_;
        double          offset_;
        slot<T>         state_{ [&](T v) { oapiTriggerRedrawArea(-1, 0, get_id()); } };
    };
}

