/*
AnimatedValue - Orbiter Addon
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

#include <limits>
#include "AnimationUpdate.h"

namespace bc_orbiter
{
    const UINT ANIM_ID_MAX = (std::numeric_limits<UINT>::max)();

    /**
     * \brief A class to manage animated values for Orbiter vessels.
     * This class provides a way to animate values over time, such as
     * for clock hands or other animated components.
     */
    template<typename T = StateUpdateDirect>
    class AnimatedValue
    {
    public:
        AnimatedValue(double speed)
        {
            state_.Speed = speed;
            state_.CurrentState = 0.0;
            state_.TargetState = 0.0;
        }

        AnimatedValue() : state_{ 1.0, 0.0, 0.0 } {}

        ~AnimatedValue() = default;

        double GetCurrent() const { return state_.CurrentState; }

        bool Update(double simdt, double target) 
        {
            state_.TargetState = target;
            return T::UpdateState(state_, simdt); 
        }

        /**
        * \brief Loads the current and target state of the animated value.
        * \param currentState - The current state of the animation.
        * \param targetState - The target state of the animation.
        */
        void LoadState(double currentState, double targetState)
        {
            state_.CurrentState = currentState;
            state_.TargetState = targetState;
        }

    private:
        AnimationState state_;
    };
}