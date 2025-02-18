//	RotaryDisplay - bco Orbiter Library
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
    * RotaryDisplay
    * A rotational gauge Control.  It takes the following:
    *   - VC Animation group id list.
    *   - Two VECTOR3s that define the postion and angle of rotation for VCs.
    *   - Panel group ID
    *   - Panel verts
    *   - Rotary_display angle of movement in radians
    *   - Animation_target speed, both panel and VC.
    *
    * RotaryDisplay is updated as part of step, and is not part of the redraw cycle.  This
    * is true for both VC and panel.
    */
    template<typename Tanim>
    class RotaryDisplay 
      : public Control,
        public VCAnimation,
        public PanelAnimation
    {
    public:
        RotaryDisplay(
            const UINT vcAnimGroupIds,
            const VECTOR3& vcLocation, const VECTOR3& vcAxisLocation,
            const UINT pnlGroupId,
            const NTVERTEX* pnlVerts,
            double angle,
            double speed,
            int pnlId = 0)
          : Control(-1),       // id not used for gauges.
            vcAnimGroup_(
                { vcAnimGroupIds },
                vcLocation, vcAxisLocation,
                angle,
                0.0, 1.0),
            pnlGroup_(pnlGroupId),
            pnlVerts_(pnlVerts),
            animSpeed_(speed),
            angle_(angle),
            panel_id_(pnlId)
        {}

        // VCAnimation
        AnimationGroup*     VCAnimationGroup() override { return &vcAnimGroup_; }
//        IAnimationState*    vc_animation_state() override { return this; }
        double              VCAnimationSpeed() const override { return animSpeed_; }
        double VCStep(double simdt) override {
            anim_.Step(state_, simdt);
            return anim_.GetState();
        }

        // IAnimationState
//        double GetState() const { return state_; }

        // PanelAnimation
        void PanelStep(MESHHANDLE mesh, double simdt) override {
            anim_.Step(state_, simdt);
            RotateMesh(mesh, pnlGroup_, pnlVerts_, (anim_.GetState() * -angle_));
        }

        int PanelID() override { return panel_id_; }

        void set_state(double d) { 
            state_ = d;
        }

    private:
        AnimationGroup	vcAnimGroup_;
        double          animSpeed_{ 0.0 };
        UINT            pnlGroup_{ 0 };
        const NTVERTEX* pnlVerts_;
        double          state_{ 0.0 };
        double          angle_{ 0.0 };
        Tanim           anim_{ animSpeed_ };
        int             panel_id_;
    };

    using RotaryDisplayTarget = RotaryDisplay<AnimationTarget>;
    using RotaryDisplayWrap   = RotaryDisplay<AnimationWrap>;
}