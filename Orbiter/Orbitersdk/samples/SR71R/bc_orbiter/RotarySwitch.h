//	RotarySwitch - bco Orbiter Library
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

#include <functional>
#include <vector>
#include <memory>
#include <assert.h>
#include <algorithm>

namespace bc_orbiter
{

	/**
	A switch with multiple stop points.  Each stop point is associated with a step position which
	starts at 0 and serves as an index.  Each point also has a position that should vary between
	0.0 and 1.0.  This can be used to indicate the position an animation should take.  Finally each
	stop has a function.  When the switch moves into a step position the function fires.
	*/
	class RotarySwitch : public EventTarget, public IAnimationState
	{
	public:
		RotarySwitch(VECTOR3 target = _V(0.0, 0.0, 0.0), double radius = 0.0) :
            EventTarget(target, radius),
			currentIndex_(0),
			currentPosition_(0.0)
		{
            SetLeftMouseDownFunc([this] { Decrement(); });
            SetRightMouseDownFunc([this] {Increment(); });
        }

		virtual ~RotarySwitch()	{ for (auto &p : stopFuncs)	delete p; }


		/**
		Stop positions need to be added in sequence.  Position 0.0 first etc.
		*/
		void AddStopFunc(double position, SwitchStopFunc func);

		/**
		Move the switch to the next stop if there is one.
		*/
		void Increment();

		/**
		Move the switch to the previous step.
		*/
		void Decrement();

		/**
		Set the switch to a specific step.
		*/
		void SetStep(int step);

		/**
		Get the current step.
		*/
		int GetStep() const { return currentIndex_; }

		/**
		Returns the current position.
		*/
		double GetPosition() const { return currentPosition_; }

		/**
		Gets the current index;
		*/
		int GetCurrentIndex() const { return currentIndex_; }

		virtual double GetState() const override { return currentPosition_; }
	private:
		int		currentIndex_;
		double	currentPosition_;

		std::vector<SwitchStop*>		stopFuncs;
	};

	inline void RotarySwitch::AddStopFunc(double position, SwitchStopFunc func)
	{
		assert(position >= 0.0);
		assert(position <= 1.0);

		for (auto &p : stopFuncs)
		{
			assert(p->stop_ != position);
		}

		stopFuncs.push_back(new SwitchStop(position, func));
		std::sort(stopFuncs.begin(), stopFuncs.end(), less_than_stops());
	}

	inline void RotarySwitch::Increment()
	{
		auto size = stopFuncs.size();

		if ((size == 0) || (currentIndex_ == (size - 1)))
		{
			return;
		}

		currentIndex_++;

		currentPosition_ = stopFuncs[currentIndex_]->stop_;
		stopFuncs[currentIndex_]->func_();
	}

	inline void RotarySwitch::Decrement()
	{
		auto size = stopFuncs.size();

		if ((size == 0) || (currentIndex_ == 0))
		{
			return;
		}

		currentIndex_--;

		currentPosition_ = stopFuncs[currentIndex_]->stop_;
		stopFuncs[currentIndex_]->func_();
	}

	inline void RotarySwitch::SetStep(int step)
	{
		if ((step < 0) || (step >= (int)stopFuncs.size()))
		{
			currentIndex_ = 0;
			currentPosition_ = 0.0;
		}
		else
		{
			currentIndex_ = step;
			currentPosition_ = stopFuncs[currentIndex_]->stop_;
			stopFuncs[currentIndex_]->func_();
		}
	}
}