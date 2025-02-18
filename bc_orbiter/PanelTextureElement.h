//	PanelTextureElement - bco Orbiter Library
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

#include "control.h"
#include "Tools.h"

namespace bc_orbiter {

    /*
    A texture based visual element for 2-D panels.  This Control takes the mesh id and NTVERTEX coordinates
    of a mesh group that has been UV mapped to a texture.  The texture mapping is updated based on a 'state'.
    By convention, the rest state of the element is state '0'.  State '1' would be 1 width of the mesh group
    to the right on the mapped texture.  A state of 2 would be two widths.  The width is determined by finding
    the difference between the max and min UV values found in pnlVerts.  This class can be paired with a VC
    element to Control the same Vessel Control in both the VC and 2-D panels.  To facilitate this the Control
    state is maintained outside of the class and is retrieved using the passed in funcState function.
    **/
    class PanelTextureElement :
        public PanelEventTarget {
    public:
        PanelTextureElement(const UINT pnlGroupId, const NTVERTEX* pnlVerts, const int panelId, funcState func) 
          : pnlGroupId_(pnlGroupId),
            pnlVerts_(pnlVerts),
            offset_(UVOffset(pnlVerts)),
            pnlId_(panelId),
            funcState_(func)
        {}

        void OnPanelRedraw(MESHHANDLE meshPanel) override {
            DrawPanelOffset(meshPanel, pnlGroupId_, pnlVerts_, offset_ * funcState_());
        }

        int PanelMouseFlags()   override { return PANEL_MOUSE_IGNORE; }
        int PanelRedrawFlags()  override { return PANEL_REDRAW_USER; }
        int PanelId()           override { return pnlId_; }

    protected:
        void triggerRedraw(VESSEL4& vessel, int ctrlId) const {
            vessel.TriggerPanelRedrawArea(pnlId_, ctrlId);
        }

    private:
        UINT            pnlGroupId_;
        const NTVERTEX* pnlVerts_;
        UINT            pnlId_;

        funcState       funcState_{ [&] {return 0.0; } };
        double          offset_{ 0.0 };
    };

    using PanelTextureControl = ElementControlBase<PanelTextureElement>;
}
