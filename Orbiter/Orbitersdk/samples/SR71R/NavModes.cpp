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
	bco::RunForEach<VcData>(vcData_, [this](const VcData& d)
		{
			oapiVCRegisterArea(d.id, PANEL_REDRAW_USER, PANEL_MOUSE_DOWN);
			oapiVCSetAreaClickmode_Spherical(d.id, d.loc, .01);
		});

	return true;
}

bool NavModes::OnVCMouseEvent(int id, int event)
{
	if (event != PANEL_MOUSE_LBDOWN) return false;

	bco::RunFor<VcData>(vcData_,
		[&](const VcData& d) {return d.id == id; },
		[this](const VcData& d)
		{
			GetBaseVessel()->ToggleNavmode(d.mode);
		});

	return true;
}

bool NavModes::OnVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	bco::RunFor<VcData>(vcData_,
		[&](const VcData& d) {return d.id == id; },
		[this](const VcData& d)
		{
			auto devMesh = GetBaseVessel()->GetVirtualCockpitMesh0();
			assert(devMesh != nullptr);

			NTVERTEX* delta = new NTVERTEX[4];

			bco::TransformUV2d(
				d.verts,
				delta, 4,
				_V(GetBaseVessel()->GetNavmodeState(d.mode) ? 0.0352 : 0.0,
					0.0,
					0.0),
				0.0);

			GROUPEDITSPEC change{};
			change.flags = GRPEDIT_VTXTEX;
			change.nVtx = 4;
			change.vIdx = NULL; //Just use the mesh order
			change.Vtx = delta;
			auto res = oapiEditMeshGroup(devMesh, d.group, &change);
			delete[] delta;
		});

	return true;
}

void NavModes::OnNavMode(int mode, bool active)
{
	switch (oapiCockpitMode())
	{
	case COCKPIT_PANELS:
		bco::RunFor<PnlData>(
			pnlData_,
			[&](const PnlData& d) {return d.mode == mode; },
			[this](const PnlData& d) {GetBaseVessel()->TriggerRedrawArea(0, 0, d.id); });
		break;

	case COCKPIT_VIRTUAL:
		bco::RunFor<VcData>(
			vcData_,
			[&](const VcData& d) {return d.mode == mode; },
			[this](const VcData& d) {GetBaseVessel()->TriggerRedrawArea(0, 0, d.id); });
		break;

	case COCKPIT_GENERIC:
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

bool NavModes::OnLoadPanel2D(int id, PANELHANDLE hPanel)
{
	bco::RunForEach<PnlData>(pnlData_, [&](const PnlData& d)
		{
			oapiRegisterPanelArea(d.id, d.rc, PANEL_REDRAW_USER);
			GetBaseVessel()->RegisterPanelArea(hPanel, d.id, d.rc, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		});

	return true;
}

bool NavModes::OnPanelRedrawEvent(int id, int event, SURFHANDLE surf)
{
	return bco::RunFor<PnlData>(pnlData_,
		[&](const PnlData& d) {return d.id == id; },
		[this](const PnlData& d)
		{
			double trans = 0.0;
			auto grp = oapiMeshGroup(GetBaseVessel()->GetpanelMeshHandle0(), d.group);
			auto vrt = d.verts;

			trans = GetBaseVessel()->GetNavmodeState(d.mode) ? 0.0352 : 0.0;
			grp->Vtx[0].tu = vrt[0].tu + trans;
			grp->Vtx[1].tu = vrt[1].tu + trans;
			grp->Vtx[2].tu = vrt[2].tu + trans;
			grp->Vtx[3].tu = vrt[3].tu + trans;
		});
}

bool NavModes::OnPanelMouseEvent(int id, int event)
{
	return bco::RunFor<PnlData>(pnlData_,
		[&](const PnlData& d) {return d.id == id; }, 
		[this](const PnlData& d) { GetBaseVessel()->ToggleNavmode(d.mode);});
}