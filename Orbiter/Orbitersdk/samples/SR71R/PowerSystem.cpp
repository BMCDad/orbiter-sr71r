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

PowerSystem::PowerSystem(bco::BaseVessel& vessel) :
	batteryLevel_(1.0),			// Always available for now.
	slotFuelCellAvailablePower_([&](double v) { })
{
	vessel.AddControl(&switchEnabled);
	vessel.AddControl(&switchConnectExternal_);
	vessel.AddControl(&switchConnectFuelCell_);
	vessel.AddControl(&lightFuelCellConnected_);
	vessel.AddControl(&lightExternalAvail_);
	vessel.AddControl(&lightExternalConnected_);
	vessel.AddControl(&gaugePowerAmps_);
	vessel.AddControl(&gaugePowerVolts_);
	vessel.AddControl(&statusBattery_);
}

bool PowerSystem::handle_load_state(const std::string& line)
{
	std::stringstream ss(line);
	ss >> switchEnabled;
	ss >> switchConnectExternal_;
	ss >> switchConnectFuelCell_;

	return true;
}

std::string PowerSystem::handle_save_state()
{
	std::stringstream ss;

	ss << switchEnabled << " " << switchConnectExternal_ << " " << switchConnectFuelCell_;
	return ss.str();
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
	auto externalConnected = vessel.IsStoppedOrDocked();
	lightExternalAvail_.set_state(externalConnected);

	// handle connected power
	auto availExternal = 
		externalConnected &&				// External power is available
		switchConnectExternal_.is_on() 		// External power is connected to the bus
		? FULL_POWER : 0.0;

	lightExternalConnected_.set_state(availExternal > USEABLE_POWER);

	// handle fuelcell power
	auto availFuelCell = switchConnectFuelCell_.is_on() ? slotFuelCellAvailablePower_.value() : 0.0;
	lightFuelCellConnected_.set_state(availFuelCell > USEABLE_POWER);

	// handle battery power
	auto availBattery = batteryLevel_ * FULL_POWER;
	auto availPower = 0.0;
	isDrawingBattery_ = false;

	if (switchEnabled.is_on())
	{
		availPower = fmax(availExternal, availFuelCell);
		if (availPower < USEABLE_POWER) {
			isDrawingBattery_ = true;
			availPower = availBattery;
		}
	}

	if (availPower != prevVolts_) {
		prevVolts_ = availPower;

		for each (auto & c in consumers_) {
			c->on_change(prevVolts_);
		}
	}

	gaugePowerVolts_.set_state(availPower);

	statusBattery_.set_state(
		(switchEnabled.is_on() && isDrawingBattery_)
		?	bco::status_display::status::warn
		:	bco::status_display::status::off
	);
}