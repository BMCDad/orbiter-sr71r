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

HUD::HUD(bco::PowerProvider& pwr, bco::Vessel& Vessel) :
    power_(pwr)
{ 
    power_.AttachConsumer(this),
    Vessel.AddControl(&btnLightDocking_);
    Vessel.AddControl(&btnLightSurface_);
    Vessel.AddControl(&btnLightOrbit_);
    
    Vessel.AddControl(&btnDocking_);
    Vessel.AddControl(&btnSurface_);
    Vessel.AddControl(&btnOrbit_);

    btnDocking_.Attach( [&]() { OnChanged(HUD_DOCKING); });
    btnOrbit_.Attach(   [&]() { OnChanged(HUD_ORBIT); });
    btnSurface_.Attach( [&]() { OnChanged(HUD_SURFACE); });
}

bool HUD::HandleLoacVC(bco::Vessel& Vessel, int vcid)
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
    btnLightDocking_.set_state( mode == HUD_DOCKING);
    btnLightSurface_.set_state( mode == HUD_SURFACE);
    btnLightOrbit_.set_state(   mode == HUD_ORBIT);

    // HUD mode is changing, if it is NOT changing to NONE, and we don't have power, turn it off.
	if (!IsPowered())
	{
		oapiSetHUDMode(HUD_NONE);
	}
}

void HUD::OnChanged(int mode)
{
	auto currentMode = oapiGetHUDMode();
	auto newMode = ((mode == currentMode) || !IsPowered()) ? HUD_NONE : mode;
	oapiSetHUDMode(newMode);;
}

void HUD::HandleDrawHUD(bco::Vessel& Vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return;

    auto am = Vessel.GetAttitudeMode();

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
}
