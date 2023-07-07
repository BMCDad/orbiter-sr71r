//	PowerSystem - SR-71r Orbiter Addon
//	Copyright(C) 2023  Blake Christensen
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
#include "PowerSystem.h"
#include "SR71r_mesh.h"

#include <assert.h>

PowerSystem::PowerSystem() :
	batteryLevel_(1.0),			// Always available for now.
	slotIsEnabled_([&](bool v) { }),
	slotConnectExternal_([&](bool v) { }),
	slotConnectFuelCell_([&](bool v) { }),
	slotFuelCellAvailablePower_([&](double v) { }),
	slotAmpDraw_([&](double v) { ampDraw_ += v; })
{
}

bool PowerSystem::handle_load_state(const std::string& line)
{
	// Only thing to persist right now is batt level, and that is not currently modelled.
	batteryLevel_ = 1.0;
	return true;
}

std::string PowerSystem::handle_save_state()
{
	return "1.0";
}

//void PowerSystem::AddMainCircuitDevice(bco::PoweredComponent* device)
//{
//	mainCircuit_.AddDevice(device);
//}

void PowerSystem::Update(bco::BaseVessel& vessel)
{
	/* Power system update:
	*	Determine if we have a power source available and how much it provides.
	*	Check the current amp total and determine if we have an overload.
	*	Report the voltage available signal so components know what they have to work with.
	*/
	signalExternalAvailable_.fire(vessel.IsStoppedOrDocked());
	
	// handle connected power
	auto availExternal = 
		slotConnectExternal_.value() && signalExternalAvailable_.current()		// External available, and the connect switch is on
		? FULL_POWER 
		: 0.0;

	// handle fuelcell power
	auto availFuelCell = slotConnectFuelCell_.value() ? slotFuelCellAvailablePower_.value() : 0.0;
	signalFuelCellConnected_.fire((slotFuelCellAvailablePower_.value() > USEABLE_POWER) && slotConnectFuelCell_.value());

	// handle battery power
	auto availBattery = batteryLevel_ * FULL_POWER;

	auto availPower = 0.0;
	if (slotIsEnabled_.value())
	{
		availPower = fmax(availExternal, availFuelCell);
		if (availPower < USEABLE_POWER) {
			signalIsDrawingBattery_.fire(true);
			availPower = availBattery;
		}
		else {
			signalIsDrawingBattery_.fire(false);
		}
	}
	else {
		signalIsDrawingBattery_.fire(false);
	}

	signalVoltLevel_.fire(availPower);

	auto ampTotal = 0.0;

	for (auto& a : power_users_) {
		ampTotal += a->amp_load();
	}

	if (ampTotal > AMP_OVERLOAD) {
		// Signal overload
	}

	signalAmpLoad_.fire(ampTotal);
}