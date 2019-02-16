//	PoweredComponent - bco Orbiter Library
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

#include "Component.h"

namespace bc_orbiter
{
    /**
    Base class for components that will take part in the vessel power
    management simulation.
    */
	class PoweredComponent : public Component
	{
	public:
		PoweredComponent(BaseVessel* baseVessel, double amps, double minVolts) :
			Component(baseVessel),
			ampsDraw_(amps),
			minVolts_(minVolts),
			voltLevel_(0.0)
		{}

        /**
        Override to change what it means for a component to have power.  By
        default a component has power when the available volt level is greater
        then the minimum volt required for the component.
        */
		virtual bool HasPower() const {	return voltLevel_ > minVolts_; }

        /**
        Called by vessel to inform the component that the power level has 
        changed.
        */
		virtual void ChangePowerLevel(double newLevel) { voltLevel_ = newLevel; }

        /**
        Indicates the amps drawn by the component when it is functioning.
        */
		virtual double CurrentDraw() { return HasPower() ? ampsDraw_ : 0.0; }

	private:
		double		ampsDraw_;
		double		minVolts_;
		double		voltLevel_;
	};
}