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


FuelCell::FuelCell(bco::BaseVessel* vessel, double amps) :
PoweredComponent(vessel, amps, 26.0),
powerSystem_(nullptr),
oxygenSystem_(nullptr),
hydrogenSystem_(nullptr),
isFuelCellAvailable_(false)
{
}

void FuelCell::Step(double simt, double simdt, double mjd)
{
	if ((nullptr == powerSystem_) || (nullptr == oxygenSystem_) || (nullptr == hydrogenSystem_) || !(HasPower() && swPower_.IsOn()))
	{
		SetIsFuelCellPowerAvailable(false);
	}
	else
	{
		/*	The burn rate is figured at 100 amps per second. We need to adjust the
			burnrate for the current amp draw and multiply by the time delta.
			*/
		auto ampsFactor = powerSystem_->GetAmpDraw() / 100;
		auto oBurn = (OXYGEN_BURN_RATE * ampsFactor) * simdt;
		auto hBurn = (HYDROGEN_BURN_RATE * ampsFactor) * simdt;

		auto drawOxy = oxygenSystem_->Draw(oBurn);
		auto drawHyd = hydrogenSystem_->Draw(hBurn);

		SetIsFuelCellPowerAvailable((drawOxy == oBurn) && (drawHyd == hBurn));
	}
}

double FuelCell::CurrentDraw()
{
	return (HasPower() && swPower_.IsOn()) ? PoweredComponent::CurrentDraw() : 0.0;
}

void FuelCell::OnSetClassCaps()
{
    swPower_.Setup(GetBaseVessel());
}

bool FuelCell::OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
	if (_strnicmp(key, ConfigKey, 8) != 0)
	{
		return false;
	}

	int state;

	sscanf_s(configLine + 8, "%i", &state);

    swPower_.SetState((state == 0) ? 0.0 : 1.0);

	return true;
}

void FuelCell::OnSaveConfiguration(FILEHANDLE scn) const
{
	char cbuf[256];
	auto val = (swPower_.GetState() == 0.0) ? 0 : 1;

	sprintf_s(cbuf, "%i", val);
	oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}

void FuelCell::SetIsFuelCellPowerAvailable(bool newValue)
{
	if (newValue != isFuelCellAvailable_)
	{
		isFuelCellAvailable_ = newValue;
	}
}