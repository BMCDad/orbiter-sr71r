//	bco Orbiter Library
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

#include <functional>
#include <vector>

/**	bco.h
	Include file for bco types.  This file is for bco types that do not require a dependency on Orbiter.
*/

namespace bc_orbiter
{
	/**
	Function def for switches that define an action function.
	*/
	typedef std::function<void()> SwitchStopFunc;

	/**
	Function def for notifying a redraw is needed.
	*/
	typedef std::function<void()> NotifyChangeFunc;

	/**	SwitchStops
	Defines the stop positions of a switch.
	*/
	struct SwitchStop
	{
		SwitchStop(double stop, SwitchStopFunc func) : stop_(stop), func_(func){}

		/**
			A value between 0.0 and 1.0.
		*/
		double				stop_;

		/**
			The function to call for this stop.
		*/
		SwitchStopFunc	func_;
	};

	struct less_than_stops
	{
		inline bool operator() (const SwitchStop* stopa, const SwitchStop* stopb)
		{
			return (stopa->stop_ < stopb->stop_);
		}
	};

	template <typename T>
	using RunPred = std::function<bool(const T& d)>;

	template <typename T>
	using RunFunc = std::function<void(const T& d)>;

	/**
	RunForEach
	Takes a vector of component data and runs the supplied function for each member.
	@param func A function that takes type T and performs an action with it.  No return value.
	*/
	template <typename T>
	void RunForEach(const std::vector<T>& data, RunFunc<T> func)
	{
		for (auto& a : data)
		{
			func(a);
		}
	}

	/**
	RunFor
	Takes a vector of component data, applies the supplied predicate, and if found runs the
	supplid function using the component data found.
	@param pred A predicate function that takes a type T, does a boolean operation and returns a boolean.
	@param func A function that takes type T and performs an action with it.  No return value.
	*/
	template <typename T>
	bool RunFor(const std::vector<T>& data, RunPred<T> pred, RunFunc<T> func)
	{
		auto m = std::find_if(data.begin(), data.end(), [&](const T& o) { return pred(o); });
		if (m == data.end()) return false;
		func(*m);
		return true;
	}
}
