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
PoweredComponent(vessel, amps, 20.0)
{
}

void NavModes::OnSetClassCaps()
{
}

bool NavModes::OnLoadVC(int id)
{
	for each (auto & v in mapVc_)
	{
		oapiVCRegisterArea(v.first, PANEL_REDRAW_USER, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(v.first, v.second.loc, .01);
	}

	return true;
}

bool NavModes::OnVCMouseEvent(int id, int event)
{
	if (event != PANEL_MOUSE_LBDOWN) return false;

	auto mode = mapMode_.find(id);
	if (mode == mapMode_.end()) return false;

	GetBaseVessel()->ToggleNavmode(mode->second);
	return true;
}

bool NavModes::OnVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	auto vc = mapVc_.find(id);
	if (vc == mapVc_.end()) return false;

	auto devMesh = GetBaseVessel()->GetVirtualCockpitMesh0();
	assert(devMesh != nullptr);

	NTVERTEX* delta = new NTVERTEX[4];

	bco::TransformUV2d(
		vc->second.verts,
		delta, 4,
		_V(GetBaseVessel()->GetNavmodeState(vc->second.mode) ? 0.0352 : 0.0,
			0.0,
			0.0),
		0.0);

	GROUPEDITSPEC change{};
	change.flags = GRPEDIT_VTXTEX;
	change.nVtx = 4;
	change.vIdx = NULL; //Just use the mesh order
	change.Vtx = delta;
	auto res = oapiEditMeshGroup(devMesh, vc->second.group, &change);
	delete[] delta;

	return true;
}

void NavModes::OnNavMode(int mode, bool active)
{
	for each (auto& v in mapMode_)
	{
		GetBaseVessel()->TriggerRedrawArea(0, 0, v.first);
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

bool NavModes::OnLoadPanel2D(int id, PANELHANDLE hPanel)
{
	for each (auto & v in mapPnl_)
	{
		oapiRegisterPanelArea(v.first, v.second.rc, PANEL_REDRAW_USER);
		GetBaseVessel()->RegisterPanelArea(hPanel, v.first, v.second.rc, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
	}

	return true;
}

bool NavModes::OnPanelRedrawEvent(int id, int event, SURFHANDLE surf)
{
	auto p = mapPnl_.find(id);
	if (p == mapPnl_.end()) return false;

	auto grp = oapiMeshGroup(GetBaseVessel()->GetpanelMeshHandle0(), p->second.group);
	auto vrt = p->second.verts;

	float trans = (float)(GetBaseVessel()->GetNavmodeState(p->second.mode) ? 0.0352 : 0.0);
	grp->Vtx[0].tu = vrt[0].tu + trans;
	grp->Vtx[1].tu = vrt[1].tu + trans;
	grp->Vtx[2].tu = vrt[2].tu + trans;
	grp->Vtx[3].tu = vrt[3].tu + trans;

	return true;
}

bool NavModes::OnPanelMouseEvent(int id, int event)
{
	return OnVCMouseEvent(id, event);
}