//	APU - SR-71r Orbiter Addon
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

#include "StdAfx.h"

#include "APU.h"
#include "Orbitersdk.h"
#include "SR71r_mesh.h"

APU::APU(bco::consumable& main_fuel) :
main_fuel_(main_fuel),
slotIsEnabled_([&](bool v) {})
{
}

void APU::handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd)
{
	if (!IsPowered())
	{
		signalHydPressure_.fire(0.0);
	}
	else
	{
		auto fuelDraw = APU_BURN_RATE * simdt;
		auto actualDraw = main_fuel_.draw(fuelDraw);

		signalHydPressure_.fire((fuelDraw == actualDraw) ? 1.0 : 0.0);
	}
}