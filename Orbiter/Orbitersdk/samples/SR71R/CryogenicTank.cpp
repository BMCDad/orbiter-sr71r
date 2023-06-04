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

#include "StdAfx.h"

#include "CryogenicTank.h"
#include "Orbitersdk.h"

#include <cmath>
#include <assert.h>

CryogenicTank::CryogenicTank(bco::BaseVessel* vessel, double capacity, double lossPerHour, char* configKey) :
	PoweredComponent(vessel, 1.0, 20.0),
	capacity_(capacity),
	level_(0.0),
	isFilling_(false)
{
	configKey_ = configKey;
    lossRate_ = lossPerHour / (60 * 60);
}

void CryogenicTank::Step(double simt, double simdt, double mjd)
{
    RotateMesh(level_ * (lossRate_*simdt));
}

double CryogenicTank::GetLevel() const
{
	return level_ / capacity_;
}

double CryogenicTank::RotateMesh(double amount)
{
	auto drawnAmount = min(level_, amount);
	level_ = level_ - drawnAmount;
	return drawnAmount;
}

void CryogenicTank::FillTank(double amount)
{
	level_ = min(level_ + amount, capacity_);

	if (level_ == capacity_)
	{
		isFilling_ = false;
	}
}

void CryogenicTank::ToggleFilling()
{
	if (isFilling_)
	{
		isFilling_ = false;
	}
	else
	{
		if (isAvailable_)
		{
			isFilling_ = true;
		}
	}
}

// Configuration
bool CryogenicTank::OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
	if (_strnicmp(key, configKey_.c_str(), configKey_.size()) != 0)
	{
		return false;
	}

	double level;
	int isPumpOn;

	sscanf_s(configLine + configKey_.size(), "%lf%d", &level, &isPumpOn);

	auto lv = fmax(0.0, fmin(1.0, level));
	level_ = lv * capacity_;

	IsFilling((isPumpOn == 1) ? true : false);

	return true;
}

/**
Called to save the class state to a configuration file.
@param scn File handle to the scenario file.
*/
void CryogenicTank::OnSaveConfiguration(FILEHANDLE scn) const
{
	char cbuf[256];

	auto pumpOn = (IsFilling()) ? 1 : 0;
	sprintf_s(cbuf, "%0.4f %d", (level_ / capacity_), pumpOn);
	oapiWriteScenario_string(scn, (char*)configKey_.c_str(), cbuf);
}
