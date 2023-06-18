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

NavModes::NavModes(bco::BaseVessel* vessel, double amps) :
PoweredComponent(vessel, amps, 20.0),
slotIsKillRot_([&](bool v)	{ ToggleMode(NAVMODE_KILLROT);		slotIsKillRot_.set(); }),
slotIsHorzLvl_([&](bool v)	{ ToggleMode(NAVMODE_HLEVEL);		slotIsHorzLvl_.set(); }),
slotIsPrograd_([&](bool v)	{ ToggleMode(NAVMODE_PROGRADE);		slotIsPrograd_.set(); }),
slotIsRetroGr_([&](bool v)	{ ToggleMode(NAVMODE_RETROGRADE);	slotIsRetroGr_.set(); }),
slotIsNorm_([&](bool v)		{ ToggleMode(NAVMODE_NORMAL);		slotIsNorm_.set(); }),
slotIsAntNorm_([&](bool v)	{ ToggleMode(NAVMODE_ANTINORMAL);	slotIsAntNorm_.set(); })
{
}

void NavModes::OnNavMode(int mode, bool active)
{
	switch (mode) {
	case NAVMODE_KILLROT:
		sigIsKillRot_.fire(active);
		break;
	case NAVMODE_HLEVEL:
		sigIsHorzLvl_.fire(active);
		break;
	case NAVMODE_PROGRADE:
		sigIsPrograd_.fire(active);
		break;
	case NAVMODE_RETROGRADE:
		sigIsRetroGr_.fire(active);
		break;
	case NAVMODE_NORMAL:
		sigIsNorm_.fire(active);
		break;
	case NAVMODE_ANTINORMAL:
		sigIsAntNorm_.fire(active);
		break;
	}
}

void NavModes::ToggleMode(int mode)
{
	if (HasPower())
	{
		GetBaseVessel()->ToggleNavmode(mode);
	}

	Update();
}

void NavModes::Update()
{
	auto vessel = GetBaseVessel();

	if (!HasPower())
	{
		// Shut down all modes:
		vessel->DeactivateNavmode(NAVMODE_KILLROT);
		vessel->DeactivateNavmode(NAVMODE_HLEVEL);
		vessel->DeactivateNavmode(NAVMODE_PROGRADE);
		vessel->DeactivateNavmode(NAVMODE_RETROGRADE);
		vessel->DeactivateNavmode(NAVMODE_NORMAL);
		vessel->DeactivateNavmode(NAVMODE_ANTINORMAL);
	}
}

bool NavModes::DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return false;
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

    return true;
}