//	FuelCell - SR-71r Orbiter Addon
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

#include "Orbitersdk.h"
#include "FuelCell.h"
#include "SR71r_mesh.h"


FuelCell::FuelCell(bco::consumable& hydrogen, bco::consumable& oxygen) :
oxygenSystem_(oxygen),
hydrogenSystem_(hydrogen),
isFuelCellAvailable_(false),
slotAmpLoad_([&](double v) { })
{
}

void FuelCell::handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd)
{
	if (!IsPowered())
	{
		SetIsFuelCellPowerAvailable(false);
	}
	else
	{
		/*	The burn rate is figured at lbs per hour at 100 amps.
			*/
		auto ampFac = (slotAmpLoad_.value() / 100) * simdt;
		auto oBurn = OXYGEN_BURN_RATE_PER_SEC * ampFac;
		auto hBurn = HYDROGEN_BURN_RATE_PER_SEC * ampFac;

		auto drawOxy = oxygenSystem_.draw(oBurn);
		auto drawHyd = hydrogenSystem_.draw(hBurn);

		SetIsFuelCellPowerAvailable((drawOxy == oBurn) && (drawHyd == hBurn));
	}

	sigAvailPower_.fire(isFuelCellAvailable_ ? MAX_VOLTS : 0.0);
}

double FuelCell::amp_load()
{
	return IsPowered() ? AMP_DRAW : 0.0;
}

void FuelCell::SetIsFuelCellPowerAvailable(bool newValue)
{
	if (newValue != isFuelCellAvailable_)
	{
		isFuelCellAvailable_ = newValue;
		sigIsAvail_.fire(isFuelCellAvailable_);
	}
}