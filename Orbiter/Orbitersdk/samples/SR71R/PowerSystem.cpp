//	PowerSystem - SR-71r Orbiter Addon
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
#include "PowerSystem.h"
#include "SR71r_mesh.h"

#include <assert.h>

PowerSystem::PowerSystem(bco::BaseVessel* vessel) :
PoweredComponent(vessel, 0.0, 0.0),
powerExternal_(0.0),
fuelCell_(nullptr),
externAvailLight_(			bm::vc::ExtAvailableLight_verts,		bm::vc::ExtAvailableLight_id),
externConnectedLight_(		bm::vc::ExtConnectedLight_verts,		bm::vc::ExtConnectedLight_id),
fuelCellAvailLight_(		bm::vc::FuelCellAvailableLight_verts,	bm::vc::FuelCellAvailableLight_id),
fuelCellConnectedLight_(	bm::vc::FuelCellConnectedLight_verts,	bm::vc::FuelCellConnectedLight_id),
isBatteryDraw_(false),
prevTime_(0.0),
prevAvailPower_(-1.0)
{
}

void PowerSystem::Step(double simt, double simdt, double mjd)
{
	mainCircuit_.Step(simt, simdt, mjd);

    gaugePowerVolt_.SetState(VoltNeedlePosition());
	gaugePowerAmp_.SetState(AmpNeedlePosition());

	if (fabs(simt - prevTime_) > 0.2)
	{
		Update();
		prevTime_ = simt;
	}
}

void PowerSystem::OnSetClassCaps()
{
    auto vessel = GetBaseVessel();
    
    swPower_.Setup(vessel);
    swConnectExternal_.Setup(vessel);
    swConnectFuelCell_.Setup(vessel);
    gaugePowerVolt_.Setup(vessel);
	gaugePowerAmp_.Setup(vessel);
	
	areaId_ = GetBaseVessel()->RegisterVCRedrawEvent(this);
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

	swPower_.SetState((main == 0) ? 0.0 : 1.0);
	swConnectExternal_.SetState((external == 0) ? 0.0 : 1.0);
	swConnectFuelCell_.SetState((fuelcell == 0) ? 0.0 : 1.0);

	volt = max(0.0, min(30.0, volt));		// Bracket to 0-30
	mainCircuit_.PowerVolts((double)volt);

	batteryLevel_ = max(0.0, min(1.0, batLvl));

	return true;
}

void PowerSystem::OnSaveConfiguration(FILEHANDLE scn) const
{
	char cbuf[256];
	auto val = (swPower_.GetState() == 0.0) ? 0 : 1;
	auto ext = (swConnectExternal_.GetState() == 0.0) ? 0 : 1;
	auto fc = (swConnectFuelCell_.GetState() == 0.0) ? 0 : 1;
	auto vlt = mainCircuit_.GetVoltLevel();
	auto blv = 1.0;

	sprintf_s(cbuf, "%i %i %i %0.2f %0.2f", val, ext, fc, vlt, blv);
	oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}

void PowerSystem::PostCreation()
{
    Update();
}

bool PowerSystem::OnLoadVC(int id)
{
	// Redraw areas:
	oapiVCRegisterArea(areaId_, PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE);

	return true;
}

bool PowerSystem::OnVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	auto devMesh = GetBaseVessel()->GetVirtualCockpitMesh0();
	assert(devMesh != nullptr);

	const double offset = 0.0244;
	double trans = 0.0;

	trans = IsExternalSourceAvailable() ? offset : 0.0;
	externAvailLight_.SetTranslate(_V(trans, 0.0, 0.0));
	externAvailLight_.Draw(devMesh);

	trans = IsExternalSourceConnected() ? offset : 0.0;
	externConnectedLight_.SetTranslate(_V(trans, 0.0, 0.0));
	externConnectedLight_.Draw(devMesh);

	trans = IsFuelCellAvailable() ? offset : 0.0;
	fuelCellAvailLight_.SetTranslate(_V(trans, 0.0, 0.0));
	fuelCellAvailLight_.Draw(devMesh);

	trans = IsFuelCellConnected() ? offset : 0.0;
	fuelCellConnectedLight_.SetTranslate(_V(trans, 0.0, 0.0));
	fuelCellConnectedLight_.Draw(devMesh);

	return true;
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
        swPower_.SetOff();
    }

	// If main switch is off, then powersource is nullptr--no power.
	// Otherwise see if either external or fuel cell is on.  External
	// power will take precedence if on.
	if (swPower_.IsOn())
	{
		auto availExternal = 0.0;
		auto availFuelCell = 0.0;
		auto availBattery = batteryLevel_ * FULL_POWER;

		if (swConnectExternal_.IsOn())
		{
			availExternal = powerExternal_;
		}
		
		if (swConnectFuelCell_.IsOn())
		{
			availFuelCell = (nullptr != fuelCell_) ? fuelCell_->AvailablePower() : 0.0;
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

	GetBaseVessel()->UpdateUIArea(areaId_);
}

bool PowerSystem::IsFuelCellAvailable()
{
	return (nullptr != fuelCell_) ? fuelCell_->IsFuelCellPowerAvailable() : false; 
}

bool PowerSystem::IsFuelCellConnected()
{
	return IsFuelCellAvailable() && swConnectFuelCell_.IsOn(); 
}
