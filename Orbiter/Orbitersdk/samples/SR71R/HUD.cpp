//	HUD - SR-71r Orbiter Addon
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

#include "HUD.h"
#include "SR71r_mesh.h"

HUD::HUD(bco::BaseVessel* vessel, double amps)
	: PoweredComponent(vessel, amps, 20.0)
{
	swSelectMode_.AddStopFunc(0.0, [this] {SwitchPositionChanged(HUD_DOCKING); });
	swSelectMode_.AddStopFunc(0.33, [this] {SwitchPositionChanged(HUD_SURFACE); });
	swSelectMode_.AddStopFunc(0.66, [this] {SwitchPositionChanged(HUD_ORBIT); });
	swSelectMode_.AddStopFunc(1.0, [this] {SwitchPositionChanged(HUD_NONE); });
	swSelectMode_.SetStep(3);
}

double HUD::CurrentDraw()
{
	return (HasPower() && (HUD_NONE != oapiGetHUDMode()) ? PoweredComponent::CurrentDraw() : 0.0);
}

void HUD::ChangePowerLevel(double newLevel)
{
	PoweredComponent::ChangePowerLevel(newLevel);
	if (!HasPower())
	{
		isInternalTrigger_ = true;
		oapiSetHUDMode(HUD_NONE);
		isInternalTrigger_ = false;
	}
	else
	{
		swSelectMode_.SetStep(swSelectMode_.GetStep());
	}
}

bool HUD::OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
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

void HUD::OnSaveConfiguration(FILEHANDLE scn) const
{
	char cbuf[256];
	auto mode = swSelectMode_.GetStep();

	sprintf_s(cbuf, "%i", mode);
	oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}


bool HUD::OnLoadVC(int id)
{
    oapiVCRegisterArea(areaId_, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Register HUD
	static VCHUDSPEC huds =
	{
		1,							// Mesh number (VC)
		bm::vc::HUD_id,		// mesh group
		{ 0.0, 0.8, 15.25 },			// hud center (location)
		0.12						// hud size
	};

	oapiVCRegisterHUD(&huds); // HUD parameters

    return true;
}

void HUD::OnSetClassCaps()
{
	auto vessel = GetBaseVessel();
	swSelectMode_.Setup(vessel);
	areaId_ = GetBaseVessel()->RegisterVCRedrawEvent(this);
}


void HUD::OnHudMode(int mode)
{
	if (!isInternalTrigger_)
	{
		// Probably change due to key.  If we don't have power,
		// we will have to reverse the setting.  If we do, we
		// need to update the switch.
		if ((HUD_NONE != mode) && (!HasPower()))
		{
			oapiSetHUDMode(HUD_NONE);
		}

		switch (mode)
		{
		case HUD_NONE:
			swSelectMode_.SetStep(3);
			break;

		case HUD_ORBIT:
			swSelectMode_.SetStep(2);
			break;

		case HUD_SURFACE:
			swSelectMode_.SetStep(1);
			break;

		case HUD_DOCKING:
			swSelectMode_.SetStep(0);
			break;
		}
	}
}

void HUD::SwitchPositionChanged(int mode)
{
	if (mode == oapiGetHUDMode()) return;

	isInternalTrigger_ = true;

	if (GetBaseVessel()->IsCreated())
	{
		if (mode != HUD_NONE)
		{
			if (HasPower())
			{
				oapiSetHUDMode(mode);
			}
		}
		else
		{
			oapiSetHUDMode(HUD_NONE);
		}
	}

	isInternalTrigger_ = false;
}


bool HUD::DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return false;
    auto am = GetBaseVessel()->GetAttitudeMode();

    if (am != RCS_NONE)
    {
        int xLeft = 200;
        int yTop = hps->H - 30;

        skp->Rectangle(
            xLeft,
            yTop,
            xLeft + 45,
            yTop + 25);

        skp->Text(xLeft + 3, yTop + 1, "RCS", 3);

        // show RCS mode
        switch (am)
        {
        case RCS_ROT:
            skp->Text(xLeft + 50, yTop, "ROT", 3);
            break;
        case RCS_LIN:
            skp->Text(xLeft + 50, yTop, "LIN", 3);
            break;
        }
    }


    return true;
}
