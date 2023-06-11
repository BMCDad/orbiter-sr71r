//	rotary_display - bco Orbiter Library
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
namespace bc_orbiter {


    /**
    * rotary_display
    * A rotational gauge control.  It takes the following:
    * VC animation group id list.
    * Two VECTOR3s that define the postion and angle of rotation for VCs.
    * Panel group ID
    * Panel verts
    * rotary_display angle of movement in radians
    * Animation speed, both panel and VC.
    * Signal transform.  Transforms the incoming signal to a 0 - 1 range needed by animation.
    *
    * rotary_display exposes a slot that will take the driving signal.  The transform function, if needed, should
    * convert the signal to a 0 to 1 value.  The angle and speed for both are the same.
    */
    class rotary_display : 
        public control, 
        public vc_animation, 
        public panel_animation, 
        public IAnimationState {
    public:
        rotary_display(
            std::initializer_list<UINT> const& vcAnimGroupIds,
            const VECTOR3& vcLocation, const VECTOR3& vcAxisLocation,
            const UINT pnlGroupId,
            const NTVERTEX* pnlVerts,
            double angle,
            double speed,
            std::function<double(double)> trans)
            : control(0.0),       // id not used for gauges.
            vcAnimGroup_(
                vcAnimGroupIds,
                vcLocation, vcAxisLocation,
                angle,
                0.0, 1.0),
            pnlGroup_(pnlGroupId),
            pnlVerts_(pnlVerts),
            animSpeed_(speed),
            angle_(angle),
            transform_(trans)
        {

        }

        // vc_animation
        AnimationGroup*     vc_animation_group() override { return &vcAnimGroup_; }
        IAnimationState*    vc_animation_state() override { return this; }
        double              vc_animation_speed() const override { return animSpeed_; }

        // IAnimationState
        double GetState() const { return state_; }

        // panel_animation
        void panel_step(MESHHANDLE mesh, double simdt) override {
            animPnl_.Step(state_, simdt);
            RotateMesh(mesh, pnlGroup_, pnlVerts_, (animPnl_.GetState() * -angle_));
        }

        slot<double>& Slot() { return slotState_; }
    private:
        std::function<double(double)> transform_;
        slot<double>    slotState_{ [&](double d) { this->state_ = transform_(d); } };
        AnimationGroup	vcAnimGroup_;
        double          animSpeed_{ 0.0 };
        UINT			pnlGroup_{ 0 };
        const NTVERTEX* pnlVerts_;
        double          state_{ 0.0 };
        double          angle_{ 0.0 };
        Animation       animPnl_{ animSpeed_ };

        void OnUpdate(double d) { state_ = d; }
    };
}