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

PowerSystem::PowerSystem(bco::BaseVessel* vessel) :
	PoweredComponent(vessel, 0.0, 0.0),
	batteryLevel_(1.0),			// Always available for now.
	slotIsEnabled_([&](bool v) { Update(); }),
	slotConnectExternal_([&](bool v) { Update(); }),
	slotConnectFuelCell_([&](bool v) { Update(); }),
	slotFuelCellAvailablePower_([&](double v) {Update(); }),
	slotAmpDraw_([&](double v) { ampDraw_ += v; })
{
}

bool PowerSystem::OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
	if (_strnicmp(key, ConfigKey, 5) != 0)
	{
		return false;
	}

	int main = 0;
	int external = 0;
	int fuelcell = 0;
	double volt = 0.0;
	double batLvl = 1.0;

	sscanf_s(configLine + 5, "%i%i%i%lf%lf", &main, &external, &fuelcell, &volt, &batLvl);

	// TODO
//	swPower_.SetState((main == 0) ? 0.0 : 1.0);
//	swConnectExternal_.SetState((external == 0) ? 0.0 : 1.0);
//	swConnectFuelCell_.SetState((fuelcell == 0) ? 0.0 : 1.0);

	volt = max(0.0, min(30.0, volt));		// Bracket to 0-30
	//mainCircuit_.PowerVolts((double)volt);

	batteryLevel_ = max(0.0, min(1.0, batLvl));

	return true;
}

void PowerSystem::OnSaveConfiguration(FILEHANDLE scn) const
{
	char cbuf[256];
	int val = 1; // TODO (swPower_.GetState() == 0.0) ? 0 : 1;
	int ext = 0; // TODO (swConnectExternal_.GetState() == 0.0) ? 0 : 1;
	int fc = 1; // TODO (swConnectFuelCell_.GetState() == 0.0) ? 0 : 1;
	double vlt = 0.0; // TODO mainCircuit_.GetVoltLevel();
	double blv = 1.0;

	sprintf_s(cbuf, "%i %i %i %0.2f %0.2f", val, ext, fc, vlt, blv);
	oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}

//void PowerSystem::AddMainCircuitDevice(bco::PoweredComponent* device)
//{
//	mainCircuit_.AddDevice(device);
//}

void PowerSystem::Update()
{
	/* Power system update:
	*	Determine if we have a power source available and how much it provides.
	*	Check the current amp total and determine if we have an overload.
	*	Report the voltage available signal so components know what they have to work with.
	*/
	signalExternalAvailable_.fire((GetBaseVessel()->IsStoppedOrDocked()) ? FULL_POWER : 0.0);
	
	auto availExternal = slotConnectExternal_.value() ? signalExternalAvailable_.current() : 0.0;
	auto availFuelCell = slotConnectFuelCell_.value() ? FuelCellAvailablePowerSlot().value() : 0.0;
	auto availBattery = batteryLevel_ * FULL_POWER;

	auto availPower = 0.0;
	signalIsDrawingBattery_.fire(false);
	if (slotIsEnabled_.value())
	{
		availPower = fmax(availExternal, availFuelCell);
		if (availPower < USEABLE_POWER) {
			signalIsDrawingBattery_.fire(true);
			availPower = availBattery;
		}
	}

	signalVoltLevel_.fire(availPower);

	// Switch is on, and external power is actually there.
	signalFuelCellConnected_.fire((slotFuelCellAvailablePower_.value() > USEABLE_POWER) && slotConnectFuelCell_.value());
}