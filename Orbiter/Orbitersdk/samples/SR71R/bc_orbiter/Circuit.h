//	Circuit - bco Orbiter Library
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

#include "PoweredComponent.h"

#include <vector>

namespace bc_orbiter
{
	/**
		Manages a powered circuit for the vessel.  It ties together powered device and both provides
		them power and calculates the total amperage of the devices powered.
	*/
	class Circuit
	{
	public:
		Circuit() :
			ampsDraw_(0.0),
			powerVolts_(0.0),
			prevTimeStep_(0.0)
		{ }

		virtual void Step(double simt, double simdt, double mjd);
		void AddDevice(PoweredComponent* device);

		// Inputs:
		void PowerVolts(double volts) {
			powerVolts_ = volts;
			OnPowerChange();
		}


		// Outputs:
		double GetTotalAmps() const { return ampsDraw_; }
		double GetVoltLevel() const { return powerVolts_; }

	private:
		void OnPowerChange() { for (auto &p : devices_) p->ChangePowerLevel(powerVolts_); }

		std::vector<PoweredComponent*>	devices_;

		double				ampsDraw_;
		double				powerVolts_;

		double				prevTimeStep_;
	};


	inline void Circuit::Step(double simt, double simdt, double mjd)
	{
		if (fabs(simt - prevTimeStep_) < 0.10)
		{
			return;
		}
		prevTimeStep_ = simt;

		double sumAmps = 0.0;
		for (auto &p : devices_)
		{
			sumAmps += p->CurrentDraw();
		}

		ampsDraw_ = sumAmps;
	}



	inline void Circuit::AddDevice(PoweredComponent* device)
	{
		// See if we already have this device and return if we do.
		for (auto p : devices_)
		{
			if (p == device)
			{
				return;
			}
		}

		// Add to our circuit devices.
		devices_.push_back(device);
	}
}