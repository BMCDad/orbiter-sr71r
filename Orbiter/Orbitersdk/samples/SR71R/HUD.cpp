//	HUD - SR-71r Orbiter Addon
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

#include "HUD.h"
#include "SR71r_mesh.h"

HUD::HUD(bco::BaseVessel* vessel, double amps)
	: PoweredComponent(vessel, amps, 20.0),
	slotDockMode_([&](bool b) {
		OnChanged(HUD_DOCKING); 
		slotDockMode_.set();	// Reset slot.
	}),
	slotOrbitMode_([&](bool b) {
		OnChanged(HUD_ORBIT);
		slotOrbitMode_.set();	// Reset slot.
	}),
	slotSurfaceMode_([&](bool b) {
		OnChanged(HUD_SURFACE);
		slotSurfaceMode_.set();	// Reset slot.
	})
{
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
		oapiSetHUDMode(HUD_NONE);
	}
}

bool HUD::OnLoadVC(int id)
{
	// Register HUD
	static VCHUDSPEC huds =
	{
		1,						// Mesh number (VC)
		bm::vc::HUD_id,			// mesh group
		{ 0.0, 0.8, 15.25 },	// hud center (location)
		0.12					// hud size
	};

	oapiVCRegisterHUD(&huds);	// HUD parameters
	return true;
}

void HUD::OnHudMode(int mode)
{
	// HUD mode is changing, if it is NOT changing to NONE, and we don't have power, turn it off.
	if ((HUD_NONE != mode) && (!HasPower()))
	{
		oapiSetHUDMode(HUD_NONE);
	}

	sigDockMode_.fire(mode == HUD_DOCKING);
	sigOrbitMode_.fire(mode == HUD_ORBIT);
	sigSurfaceMode_.fire(mode == HUD_SURFACE);
}

void HUD::OnChanged(int mode)
{
	auto currentMode = oapiGetHUDMode();

	if (GetBaseVessel()->IsCreated())
	{
		auto newMode = ((mode == currentMode) || !HasPower()) ? HUD_NONE : mode;
		oapiSetHUDMode(newMode);;
	}
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
