//	RCSSystem - SR-71r Orbiter Addon
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

#include "RCSSystem.h"
#include "Orbitersdk.h"
#include "SR71r_mesh.h"


RCSSystem::RCSSystem(bco::BaseVessel* vessel, double amps) :
PoweredComponent(vessel, amps, 20.0)
{
	swSelectMode_.AddStopFunc(0.0, [this] {SwitchPositionChanged(RCS_LIN); });
	swSelectMode_.AddStopFunc(0.5, [this] {SwitchPositionChanged(RCS_ROT); });
	swSelectMode_.AddStopFunc(1.0, [this] {SwitchPositionChanged(RCS_NONE); });
	swSelectMode_.SetStep(2);
}


void RCSSystem::OnSetClassCaps()
{
	auto vessel = GetBaseVessel();
	swSelectMode_.Setup(vessel);
    
	vcUIArea_ = GetBaseVessel()->RegisterVCRedrawEvent(this);
}

bool RCSSystem::OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
	if (_strnicmp(key, ConfigKey, 9) != 0)
	{
		return false;
	}

	int mode;

	sscanf_s(configLine + 8, "%i", &mode);

	swSelectMode_.SetStep(mode);
	return true;
}

void RCSSystem::OnSaveConfiguration(FILEHANDLE scn) const
{
	char cbuf[256];
	auto mode = swSelectMode_.GetStep();

	sprintf_s(cbuf, "%i", mode);
	oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}


double RCSSystem::CurrentDraw()
{
    return (GetBaseVessel()->GetAttitudeMode() == RCS_NONE) ? 0.0 : PoweredComponent::CurrentDraw();
}

bool RCSSystem::OnLoadVC(int id)
{
    // Redraw event
    oapiVCRegisterArea(vcUIArea_, PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE);
    return true;
}

void RCSSystem::ChangePowerLevel(double newLevel)
{
	PoweredComponent::ChangePowerLevel(newLevel);
	if (!HasPower())
	{
		isInternalTrigger_ = true;

		if (GetBaseVessel()->IsCreated())
		{
			GetBaseVessel()->SetAttitudeMode(RCS_NONE);
		}

		isInternalTrigger_ = false;
	}
	else
	{
		swSelectMode_.SetStep(swSelectMode_.GetStep());
	}
}


// Callback:
void RCSSystem::OnRCSMode(int mode)
{
	if (!isInternalTrigger_)
	{
		// Probably change due to key.  If we don't have power,
		// we will have to reverse the setting.  If we do, we
		// need to update the switch.
		if ((RCS_NONE != mode) && (!HasPower()))
		{
			GetBaseVessel()->SetAttitudeMode(RCS_NONE);
		}

		switch (mode)
		{
		case RCS_NONE:
			swSelectMode_.SetStep(2);
			break;

		case RCS_ROT:
			swSelectMode_.SetStep(1);
			break;

		case RCS_LIN:
			swSelectMode_.SetStep(0);
			break;
		}
	}
}

void RCSSystem::SwitchPositionChanged(int mode)
{
	if (mode == GetBaseVessel()->GetAttitudeMode()) return;

	isInternalTrigger_ = true;

	if (GetBaseVessel()->IsCreated())
	{
		if (mode != RCS_NONE)
		{
			if (HasPower())
			{
				GetBaseVessel()->SetAttitudeMode(mode);
			}
		}
		else
		{
			GetBaseVessel()->SetAttitudeMode(RCS_NONE);
		}
	}

	isInternalTrigger_ = false;
}
