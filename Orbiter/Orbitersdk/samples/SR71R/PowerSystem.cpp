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
	batteryLevel_(0.0),
	powerExternal_(0.0),
	isBatteryDraw_(false),
	prevTime_(0.0),
	prevAvailPower_(-1.0),
	slotMainPower_([&](bool v) { Update(); }),
	slotConnectExternal_([&](bool v) { Update(); }),
	slotConnectFuelCell_([&](bool v) { Update(); }),
	slotFuelCellAvailablePower_([&](double v) {Update(); })
{
}

void PowerSystem::Step(double simt, double simdt, double mjd)
{
	mainCircuit_.Step(simt, simdt, mjd);

	if (fabs(simt - prevTime_) > 0.2)
	{
		Update();
		prevTime_ = simt;

		signalVoltLevel_.fire(mainCircuit_.GetVoltLevel());
		signalAmpLoad_.fire(mainCircuit_.GetTotalAmps());
	}
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
	mainCircuit_.PowerVolts((double)volt);

	batteryLevel_ = max(0.0, min(1.0, batLvl));

	return true;
}

void PowerSystem::OnSaveConfiguration(FILEHANDLE scn) const
{
	char cbuf[256];
	auto val = 1; // TODO (swPower_.GetState() == 0.0) ? 0 : 1;
	auto ext = 0; // TODO (swConnectExternal_.GetState() == 0.0) ? 0 : 1;
	auto fc = 1; // TODO (swConnectFuelCell_.GetState() == 0.0) ? 0 : 1;
	auto vlt = mainCircuit_.GetVoltLevel();
	auto blv = 1.0;

	sprintf_s(cbuf, "%i %i %i %0.2f %0.2f", val, ext, fc, vlt, blv);
	oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}

void PowerSystem::PostCreation()
{
    Update();
}

void PowerSystem::AddMainCircuitDevice(bco::PoweredComponent* device)
{
	mainCircuit_.AddDevice(device);
}

void PowerSystem::Update()
{
	auto availPower = 0.0;

	powerExternal_ = (GetBaseVessel()->IsStoppedOrDocked()) ? FULL_POWER : 0.0;

    // Check if our circuit is overloaded, turn off power if it is.
    if (mainCircuit_.GetTotalAmps() > AMP_OVERLOAD)
    {
// TODO        swPower_.SetOff();
    }

	auto availExternal = 0.0;
	auto availFuelCell = 0.0;
	// If main switch is off, then powersource is nullptr--no power.
	// Otherwise see if either external or fuel cell is on.  External
	// power will take precedence if on.
	if ( slotMainPower_.value() )
	{
		auto availBattery = batteryLevel_ * FULL_POWER;

		if ( slotConnectExternal_.value() )
		{
			availExternal = powerExternal_;
		}
		
		if ( slotConnectFuelCell_.value() )
		{
			availFuelCell = slotFuelCellAvailablePower_.value();
		}


		availPower = fmax(availExternal, availFuelCell);

		if (availPower > USEABLE_POWER)
		{
			isBatteryDraw_ = false;
			availPower = FULL_POWER;
		}
		else
		{
			isBatteryDraw_ = true;
			availPower = availBattery;
		}
	}
	else
	{
		// Main switch is off.  Only external avail power light should be on.
		isBatteryDraw_ = false;
	}

	if (availPower != prevAvailPower_)
	{
		mainCircuit_.PowerVolts(availPower);
		prevAvailPower_ = availPower;
	}

	signalExternalAvailable_.fire(powerExternal_);

	// Switch is on, and external power is actually there.
	signalExternalConnected_.fire((powerExternal_ > 0.0) && slotConnectExternal_.value());
	signalFuelCellConnected_.fire((slotFuelCellAvailablePower_.value() > 0.0) && slotConnectFuelCell_.value());
}