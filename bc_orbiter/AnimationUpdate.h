/*
AnimatedUpdate - Orbiter Addon
Copyright(C) 2025  Blake Christensen

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <math.h>

namespace bc_orbiter
{
    struct AnimationState
    {
        double  Speed{ 1.0 };
        double  CurrentState{ 0.0 };
        double  TargetState{ 0.0 };
    };

    /**
        Updates the state directly.  Time is ignored and the current state is set to the target.
    */
    struct StateUpdateDirect
    {
        static bool UpdateState(AnimationState& state, double dt)
        {
            if (state.CurrentState != state.TargetState)
            {
                state.CurrentState = state.TargetState;
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
        static bool UpdateState(AnimationState& state, double dt)
        {
            if (state.CurrentState == state.TargetState) return false;

            double da = dt * state.Speed;
            auto stateDiff = state.CurrentState - state.TargetState;

            if (da > abs(stateDiff))
            {
                state.CurrentState = state.TargetState;
            }
            else
            {
                if (stateDiff > 0)
                {
                    state.CurrentState = fmax(state.TargetState, state.CurrentState - da);
                }
                else
                {
                    state.CurrentState = fmin(state.TargetState, state.CurrentState + da);
                }
            }

            if (isnan(state.CurrentState))
            {
                state.CurrentState = 0.0;
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
    struct StateUpdateWrap
    {
        static bool UpdateState(AnimationState& state, double dt)
        {
            if (state.TargetState == state.CurrentState)
            {
                return false;
            }

            double da = dt * state.Speed;

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
            auto Df = state.TargetState - state.CurrentState;

            if (signbit(Df)) {
                // Negative move
                da = (Df < -0.5) ?          // For negative moves, reverse moves are less then -.5
                    fmin(da, (Df + 1)) :     // Negative move, pick the smaller of da and -Df + 1.
                    fmax(-da, Df);           // Since Df is neg, pick the LARGER of -da, Df.
            }
            else {
                // Positive move
                da = (Df > 0.5) ?
                    fmax(-da, (Df - 1)) :    // Positive move needs a neg
                    fmin(Df, da);
            }
            state.CurrentState += da;
            if (state.CurrentState > 1.0) state.CurrentState -= 1.0;
            if (state.CurrentState < 0.0) state.CurrentState += 1.0;

            return true;
        }
    };
}