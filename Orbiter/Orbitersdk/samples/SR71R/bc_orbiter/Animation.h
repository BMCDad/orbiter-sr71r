//	Animation - bco Orbiter Library
//	Copyright(C) 2015  Blake Christensen
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

#include "OrbiterAPI.h"
#include "IAnimationState.h"

#include <vector>
#include <functional>
#include <memory>


namespace bc_orbiter
{
    struct AnimationGroup
    {
        AnimationGroup(
            std::initializer_list<UINT> const& grp,
            const VECTOR3& locA, const VECTOR3& locB,
            double angle,
            double start, double stop) :
            group_(grp),
            start_(start),
            stop_(stop)
        {
            VECTOR3 axis = locB - locA;
            normalise(axis);
            transform_ = std::make_unique<MGROUP_ROTATE>(0, group_.data(), group_.size(), locA, axis, (float)angle);
        }

        AnimationGroup(
            std::initializer_list<UINT> const& grp,
            const VECTOR3& translate,
            double start, double stop) :
            group_(grp),
            start_(start),
            stop_(stop)
        {
            transform_ = std::make_unique<MGROUP_TRANSLATE>(0, group_.data(), group_.size(), translate);
        }
        
        std::vector<UINT>                   group_;
        std::unique_ptr<MGROUP_TRANSFORM>   transform_;
        double start_;
        double stop_;
    };

    /*	The State* classes below provide the state update algorithms for the
        animation class.  We pass these in as a template argument to avoid
        a lot of virtual function calls in the 'Step' loop.
    */

    typedef std::function<void()> TargetAchievedFunc;

    struct StateUpdate
    {
        double	            speed_{ 1.0 };
        double				state_{};
        double              targetState_{};
    };

    /**
        Updates the state directly.
    */
    struct StateUpdateDirect
    {
        static bool UpdateState(StateUpdate& state, double dt)
        {
            if (state.state_ != state.targetState_)
            {
                state.state_ = state.targetState_;
                return true;
            }

            return false;
        }
    };

    /**
        Updates the state towards a target at a given speed.
    */
    struct StateUpdateTarget
    {
        static bool UpdateState(StateUpdate& state, double dt)
        {
            if (state.state_ == state.targetState_) return false;

            double da = dt * state.speed_;
            auto stateDiff = state.state_ - state.targetState_;

            if (da > abs(stateDiff))
            {
                state.state_ = state.targetState_;
            }
            else
            {
                if (stateDiff > 0)
                {
                    state.state_ = max(state.targetState_, state.state_ - da);
                }
                else
                {
                    state.state_ = min(state.targetState_, state.state_ + da);
                }
            }

            if (isnan(state.state_))
            {
                state.state_ = 0.0;
            }
            return true;
        }
    };

    /**
        Similar to StateUpdateTarget, but will treat the 0-1 range as
        a loop (wrap).  Useful for things like clock or altimeter dials.

        This works by moving 'state' towards the 'target'.  An animation
        state is defined as being between 0 and 1.  If the difference between
        state and target is > 0.5 then rather then move towards the target
        we move the state in the opposite direction.  When we hit 1 we wrap
        to 0 and when we hit zero we wrap to 1.

        Tgt   St    Df      Da -> amount to move
        0.2  0.3  -0.1      add max( -Da,   Df) to state
        0.3  0.2   0.1      add min(  Da,   Df) to state
        0.8  0.2   0.6      add max( -Da, 1-Df) from state.  If state < 0: add 1
        0.2  0.8  -0.6      add min(  Da, 1+Df) from state.  If state > 1: sub 1
    */
    struct StateUpdateWrap
    {
        static bool UpdateState(StateUpdate& state, double dt)
        {
            if (state.targetState_ == state.state_)
            {
                return false;
            }

            double da = dt * state.speed_;

            // Find the direction to move.  The sign of stateDiff
            // indicates the direction to move.
            auto Df = state.targetState_ - state.state_;

            if (Df > 0.0)
            {
                if (Df <= 0.5)
                    state.state_ += min(da, Df);
                else
                {
                    state.state_ -= max(-da, 1 - Df);
                    if (state.state_ < 0.0) state.state_ += 1;
                }
            }
            else
            {
                if (Df > -0.5)
                    state.state_ += max(-da, Df);
                else
                {
                    state.state_ += min(da, 1 + Df);
                    if (state.state_ > 1) state.state_ -= 1;
                }
            }

            return true;
        }
    };

    class IAnimation
    {
    public:
        virtual void Step(double dt) = 0;
        virtual double GetState() const = 0;
        virtual void SetState(double) = 0;
    };

    /**
    Class to manage vessel animations.  Supports adding rotation and translation groups as well as child animations.
    Animation
    */
    template <typename T = StateUpdateDirect>
    class AnimationBase : public IAnimation, public IAnimationState
    {
    public:
        AnimationBase()
        {}

        AnimationBase(IAnimationState* state, double speed, TargetAchievedFunc func = nullptr) :
            stateProvider_(state),
            funcTarget_(func)
        {
            updateState_.speed_ = speed;
        }


        /**
        Step
        Updates the state towards the target based on time delta.
        @param target The target to move towards.
        @param dt Time delta.
        */
        void Step(double target, double dt)
        {
            updateState_.targetState_ = target;
            if (T::UpdateState(updateState_, dt))
            {
                if (funcTarget_ && (updateState_.targetState_ == updateState_.state_))
                {
                    funcTarget_();
                }
            }
        }

        void Step(double dt) override
        {
            updateState_.targetState_ = stateProvider_->GetState();
            if (T::UpdateState(updateState_, dt))
            {
                if (funcTarget_ && (updateState_.targetState_ == updateState_.state_))
                {
                    funcTarget_();
                }
            }
        }

        /**
        Sets the state of the animation.
        @param state The new state of the animation.  The state is a number 0..1 which indicates
        where the animation is.  Normally target state should be set to allow the animation to move
        to a specific state.  This calls is useful during load configuration in order to set the
        animation state immediatly to a starting point.
        */
        void SetState(double state)
        {
            updateState_.state_ = state;
            updateState_.targetState_ = state;
        }

        /**
        SetTargetFunction
        @param func The function to call when the animation target is achieved.
        */
        void SetTargetFunction(TargetAchievedFunc func) { funcTarget_ = func; }

        /**
        GetState
        Returns the current state of the animation.
        */
        double GetState() const override { return updateState_.state_; }

        const double* GetStatePtr() const { return &updateState_.state_; }

    private:

        StateUpdate			updateState_;
        TargetAchievedFunc  funcTarget_;
        IAnimationState* stateProvider_;
    };

    /* Animation
    Animation with standard target update.
    */
    using Animation = AnimationBase<StateUpdateTarget>;

    // Animation with 'wrap' target update.
    using AnimationWrap = AnimationBase<StateUpdateWrap>;
}