//  VCAnimation - bco Orbiter Library
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

#include "control.h"
#include "signal.h"

namespace bc_orbiter {

    using anim_ids = std::initializer_list<UINT>;

    /**
    VCAnimation
    Provides an input only control, such as a physical toggle switch.  VC UI is an animation group, so
    a rotation axis and angle must be provided.  The Panel input is a texture, so the vertex and offset
    are required.
    Since much of the metadata will be the same, many of the metrics are provided via a control_data structure
    that can be reused for many control. A bank of switches for example.
    */
    template<typename T = double>
    class VCAnimation : public control, public vc_animation, public vc_event_target
    {
    public:
        VCAnimation(
            anim_ids const& vcAnimGroupIds,
            const VECTOR3& vcLocation,
            const VECTOR3& vcAxisLocation,
            const double animRotation,
            const double animSpeed
        ) :
            vcAnimGroup_(
                vcAnimGroupIds,
                vcLocation, vcAxisLocation,
                animRotation,
                0.0, 1.0),
            animSpeed_(animSpeed),
            animVC_(animSpeed)
        { }

        // vc_animation
        animation_group*    vc_animation_group()        override { return &vcAnimGroup_; }
        double              vc_animation_speed() const  override { return animSpeed_; }

        double vc_step(double simdt) override {
            double st = static_cast<T>(state_.value());
            animVC_.Step(st, simdt);
            return animVC_.GetState();
        }

        slot<T>& Slot() { return state_; }

    private:
        animation_group     vcAnimGroup_;
        animation_target    animVC_;
        double              animSpeed_;
        slot<T>             state_;
    };
}
