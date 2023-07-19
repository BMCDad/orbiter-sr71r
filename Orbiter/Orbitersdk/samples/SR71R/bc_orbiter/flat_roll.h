//	flat_roll - bco Orbiter Library
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

#include <functional>

namespace bc_orbiter {

    /**
    * flat_roll
    * Transforms the UV values of a texture in a loop to achieve the effect of a barrel number wheel.
    * The texture transforms in the 'v' or 'y' axis.  This can be parameterized if needed.
    */
    class flat_roll :
        public control,
        public vc_tex_animation,
        public panel_animation {
    public:
        flat_roll(
            const UINT vcGroupId,
            const NTVERTEX* vcVerts,
            const UINT pnlGroupId,
            const NTVERTEX* pnlVerts,
            const double texOffset,
            std::function<double(double)> trans)
            :
            control(0),
            vcGroup_(vcGroupId),
            vcVerts_(vcVerts),
            pnlGroup_(pnlGroupId),
            pnlVerts_(pnlVerts),
            texOffset_(texOffset),
            transform_(trans)
        {
        }

        void vc_step(DEVMESHHANDLE mesh, double simdt) override {
            anim_.Step(targetState_, simdt);
            vecTrans_.y = texOffset_ * anim_.GetState();
            TransformUV<DEVMESHHANDLE>(mesh, vcGroup_, vcVerts_, 0.0, vecTrans_);
        }

        // panel_animation
        void panel_step(MESHHANDLE mesh, double simdt) override {
            anim_.Step(targetState_, simdt);
            vecTrans_.y = texOffset_ * anim_.GetState();
            TransformUV<MESHHANDLE>(mesh, pnlGroup_, pnlVerts_, 0.0, vecTrans_);
            //            sprintf(oapiDebugString(), "T: %+4.4f  Anim: %+4.4f  Slot: %+4.4f", targetState_, anim_.GetState(), (double)slotTransform_.value());
        }

        // .0184 : 222 / 2048
        slot<double>& SlotTransform() { return slotTransform_; }

    private:
        slot<double>    slotTransform_{ [&](double d) { targetState_ = transform_(d); } };
        std::function<double(double)> transform_;

        double          targetState_{ 0.0 };
        double          texOffset_{ 0.0 };
        AnimationWrap   anim_{ 1.0 };
        VECTOR3         vecTrans_{ 0.0, 0.0, 0.0 };
        UINT			pnlGroup_{ 0 };
        const NTVERTEX* pnlVerts_{ nullptr };
        UINT			vcGroup_{ 0 };
        const NTVERTEX* vcVerts_{ nullptr };
    };
}
