//	OnOffSwitch - bco Orbiter Library
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

#include "bco.h"
#include "IAnimationState.h"
#include "EventTarget.h"
#include "BaseVessel.h"

#include <functional>

namespace bc_orbiter
{
	/**	OnOffSwitch
		A switch with two states, on and off.

        You can define on and off functions to call, or simply
        use the OnOffSwitch instance itself as a true of false
        state holder.
	*/
	class OnOffSwitch : public EventTarget, public IAnimationState
	{
	public:
		OnOffSwitch(VECTOR3 target = _V(0.0, 0.0, 0.0), double radius = 0.0, BaseVessel* vessel = nullptr) :
            EventTarget(target, radius),
            funcOn_([] {}),
            funcOff_([] {}),
			state_(0.0)
		{
			this->vessel = vessel;
            SetLeftMouseDownFunc([this] { Toggle(); });
        }

		void SetOn() {
			state_ = 1.0;
			if (vessel) vessel->SetSound(SWITCH_ON_ID, false, false);
			if (nullptr != funcOn_) funcOn_();
		}
		void SetOff() {
			state_ = 0.0;
			if (vessel) vessel->SetSound(SWITCH_OFF_ID, false, false);
			if (nullptr != funcOff_) funcOff_();
		}
		void Toggle()       { (state_ == 0.0) ? SetOn() : SetOff(); }

		bool IsOn() const                       { return (state_ != 0.0); }

		void OnFunction(SwitchStopFunc func)    { funcOn_ = func; }
		void OffFunction(SwitchStopFunc func)   { funcOff_ = func; }

		void SetState(double state)             { (state == 0.0) ? SetOff() : SetOn(); }

		virtual double GetState() const override { return state_; }
	private:
		double			state_;

		BaseVessel* vessel;

        SwitchStopFunc funcOn_;
        SwitchStopFunc funcOff_;
	};
}