//	VCGauge - bco Orbiter Library
//	Copyright(C) 2017  Blake Christensen
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

#include "bco.h"
#include <functional>
#include "IAnimationState.h"
#include "OrbiterAPI.h"
#include "BaseVessel.h"
#include "Animation.h"

#include <assert.h>

namespace bc_orbiter
{
    /**	VCGaugeBase
		Base class to define an animation based VC gauge.
    */
    template<typename AT>
    class VCGaugeBase : public IAnimationState
    {
    public:
		/**	VCGaugeBase
			Constructor defining a rotating gauge.
			param grp The mesh groups that will be part of the animation.
			param axBase The mesh location that will serve as both the 'hit' target and the base of rotation.
			param axis The location that along with axBase will define the rotation axis.
			param angle The angle of rotation.
			param speed The speed of the animation.
		*/
        VCGaugeBase(
            std::initializer_list<UINT> const &grp,
            VECTOR3 axBase,         // The event target and the base for rotation
            VECTOR3 axis,           // vector that along with target defines the rotation axis
            double angle,           // rotation angle, default 90 deg (RAD*20)
            double speed) :         // Default speed
            group_(grp, axBase, axis, angle, 0.0, 1.0),
            speed_(speed)
        {
        }

        /**
        Setup
        Called from SetClassCaps, this can probably be moved into BaseVessel (call Setup from BaseVessel::SetClassCaps)
        */
        void Setup(BaseVessel* vessel);
        void Setup2(BaseVessel* vessel, UINT meshIndex, ANIMATIONCOMPONENT_HANDLE parent = nullptr);
        void SetAnimation2(BaseVessel* vessel, double state)
        {
            vessel->SetAnimation(animId_, state);
        }

        void SetState(double state) 
		{
			state_ = state; 
		}

        virtual double GetState() const override { return state_; }
    private:

        double                      state_{ 0.0 };
        AnimationGroup              group_;
        double                      speed_{ 1.0 };
        UINT                        animId_{ 0 };
    };

    template<typename AT>
    inline void VCGaugeBase<AT>::Setup(BaseVessel* vessel)
    {
        auto aid = vessel->CreateVesselAnimation<AT>(this, speed_);
        vessel->AddVesselAnimationComponent(aid, vessel->GetVCMeshIndex(), &group_);
    }

    template<typename AT>
    inline void VCGaugeBase<AT>::Setup2(BaseVessel* vessel, UINT meshIndex, ANIMATIONCOMPONENT_HANDLE parent)
    {
        animId_ = vessel->CreateAnimation(0.0);
        group_.transform_->mesh = meshIndex;
        vessel->AddAnimationComponent(
            animId_,
            group_.start_,
            group_.stop_,
            group_.transform_.get(),
            parent);
    }

    using VCGauge       = VCGaugeBase<Animation>;
    using VCGaugeWrap   = VCGaugeBase<AnimationWrap>;


    /**
    * Gauge
    * A rotational gauge control.  It takes the following:
    * StatProvider:  This provides the state for the gauge.  Some other component will provide this.
    * Control id:   Not really used here, but needed by Control so we provide it.
    * VC animation group id list.
    * Two VECTOR3s that define the postion and angle of rotation for VCs.
    * Panel group ID
    * Panel verts
    * Panel offset
    * 
    * The state of the gauge is provided by a StateProvider that exposes a 0 to 1 value.  The inputs
    * then define the VC animation which is a standard Orbiter animation, and the verts needed to manually
    * animate the gauge in panel mode.  The angle and speed for both are the same.
    */
    class Gauge : public Control<double>, public IVCAnimate, public IPNLAnimate, public IAnimationState {
    public:
        Gauge(
            StateProvider& provider,
            std::initializer_list<UINT> const& vcAnimGroupIds,
            const VECTOR3& vcLocation, const VECTOR3& vcAxisLocation,
            const UINT pnlGroupId,
            const NTVERTEX* pnlVerts,
            double angle,
            double speed) :
            provider_(provider),
            Control<double>(0.0),       // id not used for gauges.
            vcAnimGroup_(
                vcAnimGroupIds,
                vcLocation, vcAxisLocation,
                angle,
                0.0, 1.0),
            pnlGroup_(pnlGroupId),
            pnlVerts_(pnlVerts),
            animSpeed_(speed),
            angle_(angle)
        {
            provider_.Subscribe([&](double d) {state_ = d; });
        }

        // IVCAnimate
        AnimationGroup* GetVCAnimationGroup() override { return &vcAnimGroup_; }
        IAnimationState* GetVCAnimationStateController() override { return this; }
        double GetVCAnimationSpeed() const override { return animSpeed_; }

        // IAnimationState
        double GetState() const { return state_; }

        // IPNLAnimate
        void PanelStep(MESHHANDLE mesh, double simdt) override {
            RotateMesh(mesh, pnlGroup_, pnlVerts_, (state_ * -angle_));
        }

    private:
        StateProvider&  provider_;
        AnimationGroup	vcAnimGroup_;
        double          animSpeed_{ 0.0 };
        UINT			pnlGroup_{ 0 };
        const NTVERTEX* pnlVerts_;
        double          state_{ 0.0 };
        double          angle_{ 0.0 };
    };

}
