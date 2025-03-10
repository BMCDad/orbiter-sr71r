//	transform_display - bco Orbiter Library
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

    class transform_display :
        public control,
        public vc_tex_animation,
        public panel_animation {
    public:
        transform_display(
            const UINT vcGroupId,
            const NTVERTEX* vcVerts,
            const UINT pnlGroupId,
            const NTVERTEX* pnlVerts,
            int pnlId = 0)
            :
            control(0),       // id not used
            vcGroup_(vcGroupId),
            vcVerts_(vcVerts),
            pnlGroup_(pnlGroupId),
            pnlVerts_(pnlVerts),
            pnlId_(pnlId)
        {
        }

        // vc_tex_animation
        void vc_step(DEVMESHHANDLE mesh, double simdt) override {
//            RotateMesh<DEVMESHHANDLE>(mesh, vcGroup_, vcVerts_, angle_);
            TransformUV<DEVMESHHANDLE>(mesh, vcGroup_, vcVerts_, angle_, vecTrans_);
        }

        // panel_animation
        void panel_step(MESHHANDLE mesh, double simdt) override {
//            RotateMesh<MESHHANDLE>(mesh, pnlGroup_, pnlVerts_, angle_);
            TransformUV<MESHHANDLE>(mesh, pnlGroup_, pnlVerts_, angle_, vecTrans_);
        }

        int panel_id() override { return pnlId_; }

        void SetAngle(double a) { angle_ = a; }
        void SetTransform(double x, double y) { vecTrans_.x = x; vecTrans_.y = y; }

    private:
        double          angle_      { 0.0 };
        VECTOR3         vecTrans_   { 0.0, 0.0, 0.0 };
        UINT			vcGroup_    { 0 };
        const NTVERTEX* vcVerts_;
        UINT			pnlGroup_   { 0 };
        const NTVERTEX* pnlVerts_;
        int             pnlId_;
    };
}