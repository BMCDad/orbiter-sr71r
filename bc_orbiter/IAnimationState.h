//	IAnimationState - bco Orbiter Library
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

namespace bc_orbiter
{
	/**
	Implemented by an object to indicate that it provides state to control
	an animation.
	*/
	class IAnimationState
	{
	public:
		/**
		Implementation should provide a value between 0.0 and 1.0 which determines
		the current state of the animation.
		*/
		virtual double GetState() const = 0;

		virtual ~IAnimationState(){}
	};
}