//	animation_target - bco Orbiter Library
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
    struct animation_group
    {
        animation_group(
            std::initializer_list<UINT> const& grp,
            const VECTOR3& locA, const VECTOR3& locB,
            double angle,
            double start, double stop) :
            group_(grp),
            start_(start),
            stop_(stop),
            location_(locA)
        {
            VECTOR3 axis = locB - locA;
            normalise(axis);
            transform_ = std::make_unique<MGROUP_ROTATE>(0, group_.data(), group_.size(), locA, axis, (float)angle);
        }

        animation_group(
            std::initializer_list<UINT> const& grp,
            const VECTOR3& translate,
            double start, double stop) :
            group_(grp),
            start_(start),
            stop_(stop),
            location_(_V(0.0, 0.0, 0.0))
        {
            transform_ = std::make_unique<MGROUP_TRANSLATE>(0, group_.data(), group_.size(), translate);
        }
        
        std::vector<UINT>                   group_;
        VECTOR3                             location_{ _V(0.0,0.0,0.0) };
        std::unique_ptr<MGROUP_TRANSFORM>   transform_;
        double start_;
        double stop_;
    };

    /*	The State* classes below provide the state update algorithms for the
        animation class.  We pass these in as a template argument to avoid
        a lot of virtual function calls in the 'Step' loop.
    */

    using func_target_achieved = std::function<void()>;

    struct state_update
    {
        double	            speed_{ 1.0 };
        double				state_{ 0.0 };
        double              target_state_{ 0.0 };
    };

    /**
        Updates the state directly.
    */
    struct state_update_direct
    {
        static bool update_state(state_update& state, double dt)
        {
            if (state.state_ != state.target_state_)
            {
                state.state_ = state.target_state_;
                return true;
            }

            return false;
        }
    };

    /**
        Updates the state towards a target at a given speed.
    */
    struct state_update_target
    {
        static bool update_state(state_update& state, double dt)
        {
            if (state.state_ == state.target_state_) return false;

            double da = dt * state.speed_;
            auto stateDiff = state.state_ - state.target_state_;

            if (da > abs(stateDiff))
            {
                state.state_ = state.target_state_;
            }
            else
            {
                if (stateDiff > 0)
                {
                    state.state_ = max(state.target_state_, state.state_ - da);
                }
                else
                {
                    state.state_ = min(state.target_state_, state.state_ + da);
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
        Similar to state_update_target, but will treat the 0-1 range as
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
    struct state_update_wrap
    {
        static bool update_state(state_update& state, double dt)
        {
            if (state.target_state_ == state.state_)
            {
                return false;
            }

            double da = dt * state.speed_;

            // Find the direction to move.  The sign of stateDiff
            // indicates the direction to move.
            /*
            * 
                da      =  .1                   time step
                state   =  .97                  current
                target  =  .03                  target.  The new state should be in the range: .97-1.0   and 0.0-0.3
                Df      = -.94 = .03-.97        Df < da, so Df should always win.
                da      =  .1                   so state become 1.07, which is wrong
            */
            auto Df = state.target_state_ - state.state_;

            if (signbit(Df)) {
                // Negative move
                da = (Df < -0.5) ?          // For negative moves, reverse moves are less then -.5
                    min(da, (Df + 1)) :     // Negative move, pick the smaller of da and -Df + 1.
                    max(-da, Df);           // Since Df is neg, pick the LARGER of -da, Df.
            }
            else {
                // Positive move
                da = (Df > 0.5) ? 
                    max(-da, (Df - 1)) :    // Positive move needs a neg
                    min(Df, da);
            }
            state.state_ += da;
            if (state.state_ > 1.0) state.state_ -= 1.0;
            if (state.state_ < 0.0) state.state_ += 1.0;

            // tgt = .9,  state = 0  : Df = .9  assume da = .01
            //if (Df > 0.0)   // Just checking sign here, Df = 0 checked above
            //{
            //    if (Df <= 0.5)
            //        state.state_ += min(da, Df);
            //    else
            //    {
            //        // Get here:
            //        state.state_ -= max(-da, 1 - Df);       // This would set the state - (max( -.01, 1 - .9)
            //        if (state.state_ < 0.0) state.state_ += 1;
            //    }
            //}
            //else
            //{
            //    if (Df > -0.5)
            //        state.state_ += max(-da, Df);
            //    else
            //    {
            //        state.state_ += min(da, 1 + Df);
            //        if (state.state_ > 1) state.state_ -= 1;
            //    }
            //}

            return true;
        }
    };

    struct animation {
        virtual void Step(double dt) = 0;
        virtual double GetState() const = 0;
        virtual void SetState(double) = 0;
    };

    /**
    Class to manage vessel animations.  Supports adding rotation and translation groups as well as child animations.
    animation_target
    */
    template <typename T = state_update_direct>
    class animation_base : public animation, public IAnimationState
    {
    public:
        animation_base()
        {}

        animation_base(IAnimationState* state, double speed, func_target_achieved func = nullptr) :
            stateProvider_(state),
            funcTarget_(func)
        {
            updateState_.speed_ = speed;
        }

        animation_base(double speed)
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
            updateState_.target_state_ = target;
            if (T::update_state(updateState_, dt))
            {
                if (funcTarget_ && (updateState_.target_state_ == updateState_.state_))
                {
                    funcTarget_();
                }
            }
        }

        void Step(double dt) override
        {
            updateState_.target_state_ = stateProvider_->GetState();
            if (T::update_state(updateState_, dt))
            {
                if (funcTarget_ && (updateState_.target_state_ == updateState_.state_))
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
            updateState_.target_state_ = state;
        }

        /**
        SetTargetFunction
        @param func The function to call when the animation target is achieved.
        */
        void SetTargetFunction(func_target_achieved func) { funcTarget_ = func; }

        /**
        GetState
        Returns the current state of the animation.
        */
        double GetState() const override { return updateState_.state_; }

        const double* GetStatePtr() const { return &updateState_.state_; }

        void VesselId(UINT id) { vesselId_ = id; }
        UINT VesselId() const { return vesselId_; }

        friend std::istream& operator>>(std::istream& input, animation_base& obj) {
            if (input) {
                double state = 0.0;
                input >> state;
                if (state < 0.0) state = 0.0;
                if (state > 1.0) state = 1.0;
                obj.SetState(state);
            }

            return input;
        }

        friend std::ostream& operator<<(std::ostream& output, animation_base& obj) {
            output.precision(4);
            output << obj.GetState();
            return output;
        }

    private:

        state_update			updateState_;
        func_target_achieved  funcTarget_;
        IAnimationState*    stateProvider_{ nullptr };
        UINT                vesselId_{ 0 };
    };

    /* animation_target
    animation_target with standard target update.
    */
    using animation_target = animation_base<state_update_target>;

    // animation_target with 'wrap' target update.
    using animation_wrap = animation_base<state_update_wrap>;
}