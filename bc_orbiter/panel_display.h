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

    class panel_display :
        public control,
        public panel_animation {
    public:
        panel_display(
            const UINT pnlGroupId,
            const NTVERTEX* pnlVerts,
            double offset,
            int pnlId = 0)
            :
            control(0),       // id not used
            pnlGroup_(pnlGroupId),
            pnlVerts_(pnlVerts),
            offset_(offset),
            pnlId_(pnlId)
        {
        }

        // panel_animation
        void panel_step(MESHHANDLE mesh, double simdt) override {
            //            RotateMesh<MESHHANDLE>(mesh, pnlGroup_, pnlVerts_, angle_);
            TransformUV<MESHHANDLE>(mesh, pnlGroup_, pnlVerts_, 0.0, vecTrans_);
        }

        int panel_id() override { return pnlId_; }

        void set_position(int pos) {
            set_transform(pos * offset_, 0.0);
        }

        void set_transform(double x, double y) { vecTrans_.x = x; vecTrans_.y = y; }

    private:
        VECTOR3         vecTrans_{ 0.0, 0.0, 0.0 };
        UINT			pnlGroup_{ 0 };
        const NTVERTEX* pnlVerts_;
        double          offset_;
        int             pnlId_;
    };
}
