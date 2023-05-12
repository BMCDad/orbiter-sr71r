//	NavModes - SR-71r Orbiter Addon
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

#include "bc_orbiter\BaseVessel.h"

#include "NavModes.h"
#include "SR71r_mesh.h"

#include <assert.h>

NavModes::NavModes(bco::BaseVessel* vessel, double amps) :
PoweredComponent(vessel, amps, 20.0)
{
    vessel->RegisterVCEventTarget(&btnKillRotation_);
    vessel->RegisterVCEventTarget(&btnLevelHorizon_);
    vessel->RegisterVCEventTarget(&btnPrograde_);
    vessel->RegisterVCEventTarget(&btnRetrograde_);
    vessel->RegisterVCEventTarget(&btnNormal_);
    vessel->RegisterVCEventTarget(&btnAntiNormal_);

	vessel->RegisterPanelEventTarget(&pnlBtnKillRotation_);
	vessel->RegisterPanelEventTarget(&pnlBtnHorzLevel_);
	vessel->RegisterPanelEventTarget(&pnlBtnPrograde_);
	vessel->RegisterPanelEventTarget(&pnlBtnRetrograde_);
	vessel->RegisterPanelEventTarget(&pnlBtnNormal_);
	vessel->RegisterPanelEventTarget(&pnlBtnAntiNormal_);
}

void NavModes::OnSetClassCaps()
{
}

bool NavModes::OnLoadVC(int id)
{
	// Redraw event
	oapiVCRegisterArea(GetRedrawId(), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE);
	return true;
}

bool NavModes::OnVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	auto devMesh = GetBaseVessel()->GetVirtualCockpitMesh0();
	assert(devMesh != nullptr);

	auto vessel = GetBaseVessel();
	const double offset = 0.0352;
	double trans = 0.0;

    navMode1_ = 0;
    navMode2_ = 0;

	trans = vessel->GetNavmodeState(NAVMODE_KILLROT) ? offset : 0.0;
    if (trans != 0.0) navMode1_ = NAVMODE_KILLROT;
	visKillRot_.SetTranslate(_V(trans, 0.0, 0.0));
	visKillRot_.Draw(devMesh);

	trans = vessel->GetNavmodeState(NAVMODE_HLEVEL) ? offset : 0.0;
    if (trans != 0.0) navMode2_ = NAVMODE_HLEVEL;
	visHorzLevel_.SetTranslate(_V(trans, 0.0, 0.0));
	visHorzLevel_.Draw(devMesh);

	trans = vessel->GetNavmodeState(NAVMODE_PROGRADE) ? offset : 0.0;
    if (trans != 0.0) navMode1_ = NAVMODE_PROGRADE;
    visProGrade_.SetTranslate(_V(trans, 0.0, 0.0));
	visProGrade_.Draw(devMesh);

	trans = vessel->GetNavmodeState(NAVMODE_RETROGRADE) ? offset : 0.0;
    if (trans != 0.0) navMode1_ = NAVMODE_RETROGRADE;
	visRetroGrade_.SetTranslate(_V(trans, 0.0, 0.0));
	visRetroGrade_.Draw(devMesh);

	trans = vessel->GetNavmodeState(NAVMODE_NORMAL) ? offset : 0.0;
    if (trans != 0.0) navMode1_ = NAVMODE_NORMAL;
	visNormal_.SetTranslate(_V(trans, 0.0, 0.0));
	visNormal_.Draw(devMesh);

	trans = vessel->GetNavmodeState(NAVMODE_ANTINORMAL) ? offset : 0.0;
    if (trans != 0.0) navMode1_ = NAVMODE_ANTINORMAL;
	visAntiNormal_.SetTranslate(_V(trans, 0.0, 0.0));
	visAntiNormal_.Draw(devMesh);

	return true;
}

void NavModes::OnNavMode(int mode, bool active)
{
	Update();
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

	GetBaseVessel()->TriggerRedrawArea(0, 0, GetRedrawId());
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

bool NavModes::OnLoadPanel2D(int id, PANELHANDLE hPanel)
{
	GetBaseVessel()->RegisterPanelArea(hPanel, GetRedrawId(), _R(0,0,0,0), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE);
	return true;
}

bool NavModes::OnPanelRedrawEvent(int id, int event)
{
	auto devMesh = GetBaseVessel()->GetpanelMeshHandle0();
	assert(devMesh != nullptr);

	auto vessel = GetBaseVessel();
	const double offset = 0.0352;
	double trans = 0.0;

	navMode1_ = 0;
	navMode2_ = 0;

	trans = vessel->GetNavmodeState(NAVMODE_KILLROT) ? offset : 0.0;
	if (trans != 0.0) navMode1_ = NAVMODE_KILLROT;
	pnlKillRot_.SetTranslate(_V(trans, 0.0, 0.0));
	pnlKillRot_.Draw(devMesh);

	trans = vessel->GetNavmodeState(NAVMODE_HLEVEL) ? offset : 0.0;
	if (trans != 0.0) navMode1_ = NAVMODE_HLEVEL;
	pnlHorzLevel_.SetTranslate(_V(trans, 0.0, 0.0));
	pnlHorzLevel_.Draw(devMesh);

	trans = vessel->GetNavmodeState(NAVMODE_PROGRADE) ? offset : 0.0;
	if (trans != 0.0) navMode1_ = NAVMODE_PROGRADE;
	pnlProGrade_.SetTranslate(_V(trans, 0.0, 0.0));
	pnlProGrade_.Draw(devMesh);

	trans = vessel->GetNavmodeState(NAVMODE_RETROGRADE) ? offset : 0.0;
	if (trans != 0.0) navMode1_ = NAVMODE_RETROGRADE;
	pnlRetroGrade_.SetTranslate(_V(trans, 0.0, 0.0));
	pnlRetroGrade_.Draw(devMesh);

	trans = vessel->GetNavmodeState(NAVMODE_NORMAL) ? offset : 0.0;
	if (trans != 0.0) navMode1_ = NAVMODE_NORMAL;
	pnlNormal_.SetTranslate(_V(trans, 0.0, 0.0));
	pnlNormal_.Draw(devMesh);

	trans = vessel->GetNavmodeState(NAVMODE_ANTINORMAL) ? offset : 0.0;
	if (trans != 0.0) navMode1_ = NAVMODE_ANTINORMAL;
	pnlAntiNormal_.SetTranslate(_V(trans, 0.0, 0.0));
	pnlAntiNormal_.Draw(devMesh);

	return true;
}