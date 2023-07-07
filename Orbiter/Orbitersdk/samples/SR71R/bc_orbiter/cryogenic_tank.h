//	CryogenicTank - SR-71r Orbiter Addon
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

#include "generic_tank.h"

namespace bc_orbiter {

	/**
	Base class for a cryogenic tank.
	*/
	class cryogenic_tank : 
		public generic_tank
	{
	public:
		cryogenic_tank(double capacity, double fillRate, double lossPerHour) :
			generic_tank(capacity, fillRate) {
			lossRate_ = lossPerHour / 3600; // 60*60
		}

		// post_step
		void handle_post_step(BaseVessel& vessel, double simt, double simdt, double mjd) override {
			draw(Level() * (lossRate_ * simdt));	// evaporative loss
			generic_tank::handle_post_step(vessel, simt, simdt, mjd);
		}

		// power_consumer
		double amp_load() override {
			return generic_tank::amp_load() + (IsPowered() ? AMPS_COOLING : 0.0);
		}

	private:
		const double AMPS_COOLING = 4.0;
		double                  lossRate_;
	};
}