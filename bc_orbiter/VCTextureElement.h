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
    A texture based visual element for virtual cockpits.  This Control takes the mesh id and NTVERTEX coordinates
    of a mesh group that has been UV mapped to a texture.  The texture mapping is updated based on a 'state'.
    By convention, the rest state of the element is state '0'.  State '1' would be 1 width of the mesh group
    to the right on the mapped texture.  A state of 2 would be two widths.  The width is determined by finding
    the difference between the max and min UV values found in pnlVerts.  This class can be paired with a VC
    element to Control the same Vessel Control in both the VC and 2-D panels.  To facilitate this the Control
    state is maintained outside of the class and is retrieved using the passed in funcState function.
    **/
    class VCTextureElement : public VCEventTarget
    {
    public:
        VCTextureElement(const UINT vcGroupId, const NTVERTEX* vcVerts, const int vcId, funcState func) 
          : vcGroupId_(vcGroupId),
            vcVerts_(vcVerts),
            offset_(UVOffset(vcVerts)),
            vcId_(vcId),
            funcState_(func)
        {}

        void OnVCRedraw(DEVMESHHANDLE vcMesh) override {
            DrawVCOffset(vcMesh, vcGroupId_, vcVerts_, funcState_() * offset_);
        }

        int VCMouseFlags()        override { return PANEL_MOUSE_IGNORE; }
        int VCRedrawFlags()       override { return PANEL_REDRAW_USER; }
        int VCId()                 override { return vcId_; }

    protected:

        void triggerRedraw(VESSEL4& vessel, int ctrlId) const {
            // TriggerVCRedrawArea has not been made public yet...
            vessel.TriggerRedrawArea(-1, vcId_, ctrlId);
        }

    private:
        UINT            vcGroupId_;
        const NTVERTEX* vcVerts_;
        UINT            vcId_{ 0 };

        funcState       funcState_{ [&] {return 0.0; } };
        double          offset_{ 0.0 };
    };
}