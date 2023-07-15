//	NavModes - SR-71r Orbiter Addon
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
#include "bc_orbiter\BaseVessel.h"

#include "NavModes.h"
#include "SR71r_mesh.h"

#include <assert.h>

NavModes::NavModes(bco::BaseVessel& baseVessel) :
    baseVessel_(baseVessel)
{
    baseVessel_.AddControl(&btnKillRot_);
    baseVessel_.AddControl(&btnHorzLevel_);
    baseVessel_.AddControl(&btnAntiNorm_);
    baseVessel_.AddControl(&btnNormal_);
    baseVessel_.AddControl(&btnPrograde_);
    baseVessel_.AddControl(&btnRetrograde_);

    baseVessel_.AddControl(&lightKillRot_);
    baseVessel_.AddControl(&lightHorzLevel_);
    baseVessel_.AddControl(&lightAntiNorm_);
    baseVessel_.AddControl(&lightNormal_);
    baseVessel_.AddControl(&lightPrograde_);
    baseVessel_.AddControl(&lightRetrograde_);

    btnKillRot_     .attach([&]() { ToggleMode(NAVMODE_KILLROT); });
    btnHorzLevel_   .attach([&]() { ToggleMode(NAVMODE_HLEVEL); });
    btnAntiNorm_    .attach([&]() { ToggleMode(NAVMODE_ANTINORMAL); });
    btnNormal_      .attach([&]() { ToggleMode(NAVMODE_NORMAL); });
    btnPrograde_    .attach([&]() { ToggleMode(NAVMODE_PROGRADE); });
    btnRetrograde_  .attach([&]() { ToggleMode(NAVMODE_RETROGRADE); });
}

void NavModes::OnNavMode(int mode, bool active)
{
    switch (mode) {
    case NAVMODE_ANTINORMAL:
        lightAntiNorm_.set_state(active);
        break;
    case NAVMODE_HLEVEL:
        lightHorzLevel_.set_state(active);
        break;
    case NAVMODE_KILLROT:
        lightKillRot_.set_state(active);
        break;
    case NAVMODE_NORMAL:
        lightNormal_.set_state(active);
        break;
    case NAVMODE_PROGRADE:
        lightPrograde_.set_state(active);
        break;
    case NAVMODE_RETROGRADE:
        lightRetrograde_.set_state(active);
        break;
    }
}

void NavModes::ToggleMode(int mode)
{
	if (slotIsEnabled_.value())
	{
        baseVessel_.ToggleNavmode(mode);
	}

	Update();
}

void NavModes::Update()
{
	if (!IsEnabledSlot().value())
	{
		// Shut down all modes:
		baseVessel_.DeactivateNavmode(NAVMODE_KILLROT);
		baseVessel_.DeactivateNavmode(NAVMODE_HLEVEL);
		baseVessel_.DeactivateNavmode(NAVMODE_PROGRADE);
		baseVessel_.DeactivateNavmode(NAVMODE_RETROGRADE);
		baseVessel_.DeactivateNavmode(NAVMODE_NORMAL);
		baseVessel_.DeactivateNavmode(NAVMODE_ANTINORMAL);
	}
}

void NavModes::handle_draw_hud(bco::BaseVessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return;
    int xLeft = 2;
    int yTop = hps->H - 30;

    if ((navMode1_ != 0) || (navMode2_ != 0))
    {
        skp->Rectangle(
            xLeft,
            yTop,
            xLeft + 45,
            yTop + 25);

        skp->Text(xLeft + 3, yTop + 1, "NAV", 3);

        char* lbl1 = nullptr;

        switch (navMode1_)
        {
        case NAVMODE_ANTINORMAL:
            lbl1 = "ANRM";
            break;

        case NAVMODE_NORMAL:
            lbl1 = "NRM ";
            break;

        case NAVMODE_PROGRADE:
            lbl1 = "PROG";
            break;

        case NAVMODE_RETROGRADE:
            lbl1 = "RTRO";
            break;

        case NAVMODE_KILLROT:
            lbl1 = "KLRT";
            break;

        }

        if (lbl1) skp->Text(xLeft + 50, yTop + 1, lbl1, 4);

        skp->Line(xLeft + 100, yTop, xLeft + 100, yTop + 25);

        if (navMode2_ == NAVMODE_HLEVEL) skp->Text(xLeft + 105, yTop + 1, "HLVL", 4);
    }
}