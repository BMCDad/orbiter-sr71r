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

APU::APU(bco::BaseVessel* vessel, double amps) :
bco::PoweredComponent(vessel, amps, APU_MIN_VOLT),
propulsionControl_(nullptr)
{
}

void APU::Step(double simt, double simdt, double mjd)
{
	if ((nullptr == propulsionControl_) || !HasPower() || !swPower_.IsOn())
	{
        gaugeHydrPress_.SetState(0.0);
	}
	else
	{
		auto fuelDraw = APU_BURN_RATE * simdt;
		auto actualDraw = propulsionControl_->DrawRCSFuel(fuelDraw);

        gaugeHydrPress_.SetState((fuelDraw == actualDraw) ? 1.0 : 0.0);
	}

	SetSound();
}

void APU::SetSound()
{
	double status = gaugeHydrPress_.GetState();
	// TODO: NOTE: Startup and shutdown sound aren't 'true', since the status is just 0 and 1.
	// However, the code supports shutdown and start. When the status shows the actual status, it will work.

	if (HasPower() && swPower_.IsOn() && propulsionControl_) {
		// If it's starting (See note above)
		if (status < 1.0 && status > 0.0) { 
			if(!GetBaseVessel()->IsSoundRunning(APU_START_ID)) GetBaseVessel()->PlaySound(APU_START_ID, true, false);
		}
		// If it's running
		else if (status == 1.0) {
			if (!GetBaseVessel()->IsSoundRunning(APU_RUNNING_ID)) {
				StopAllSounds();
				GetBaseVessel()->PlaySound(APU_RUNNING_ID, true, false);
			}
		}
		if(isSoundStopped) isSoundStopped = false;
	}
	else {
		// If it's shutting down (See note above)
		if (status < 1.0 && status > 0.0) {
			if (!GetBaseVessel()->IsSoundRunning(APU_SHUTDOWN_ID)) {
				StopAllSounds();
				GetBaseVessel()->PlaySound(APU_SHUTDOWN_ID, true, false);
			}
		}
		else {
			if (!isSoundStopped) { StopAllSounds(); isSoundStopped = true; }
		}
	}
}

void APU::StopAllSounds()
{
	for (int i = APU_START_ID; i <= APU_SHUTDOWN_ID; i++) GetBaseVessel()->StopSound(i);
}

double APU::CurrentDraw()
{
	return (HasPower() && swPower_.IsOn()) ? PoweredComponent::CurrentDraw() : 0.0;
}

void APU::SetClassCaps()
{
    swPower_.Setup(GetBaseVessel());
    gaugeHydrPress_.Setup(GetBaseVessel());
}

bool APU::LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
	if (_strnicmp(key, ConfigKey, 3) != 0)
	{
		return false;
	}

	int state;

	sscanf_s(configLine + 3, "%i", &state);

	swPower_.SetState((state == 0) ? 0.0 : 1.0);

	return true;
}

void APU::SaveConfiguration(FILEHANDLE scn) const
{
	char cbuf[256];
	auto val = (swPower_.GetState() == 0.0) ? 0 : 1;

	sprintf_s(cbuf, "%i", val);
	oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}

bco::OnOffSwitch&   APU::PowerSwitch()                                  { return swPower_; }
double              APU::GetHydraulicLevel() const                      { return gaugeHydrPress_.GetState(); }
void                APU::SetPropulsionControl(PropulsionController* pc) { propulsionControl_ = pc; }
