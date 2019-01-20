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
}
