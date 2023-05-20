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
	: PoweredComponent(vessel, amps, 20.0)
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
	bco::RunForEach<VcData>(vcData_, [this](const VcData& d)
		{
			oapiVCRegisterArea(d.id, PANEL_REDRAW_USER, PANEL_MOUSE_DOWN);
			oapiVCSetAreaClickmode_Spherical(d.id, d.loc, .01);
		});

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

bool HUD::OnVCMouseEvent(int id, int event)
{
	if (event != PANEL_MOUSE_LBDOWN) return false;

	bco::RunFor<VcData>(vcData_,
		[&](const VcData& d) {return d.id == id; },
		[this](const VcData& d)
		{
			OnChanged(d.mode);
		});

	return true;
}

bool HUD::OnVCRedrawEvent(int id, int event, SURFHANDLE surf)
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
				_V(oapiGetHUDMode() == d.mode ? 0.0352 : 0.0,
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

bool HUD::OnLoadPanel2D(int id, PANELHANDLE hPanel)
{
	bco::RunForEach<PnlData>(pnlData_, [&](const PnlData& d)
		{
			oapiRegisterPanelArea(d.id, d.rc, PANEL_REDRAW_USER);
			GetBaseVessel()->RegisterPanelArea(hPanel, d.id, d.rc, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		});

	return true;
}

bool HUD::OnPanelMouseEvent(int id, int event)
{
	return bco::RunFor<PnlData>(pnlData_,
		[&](const PnlData& d) {return d.id == id; },
		[this](const PnlData& d) { OnChanged(d.mode); });
}

bool HUD::OnPanelRedrawEvent(int id, int event, SURFHANDLE surf)
{
	return bco::RunFor<PnlData>(pnlData_,
		[&](const PnlData& d) {return d.id == id; },
		[this](const PnlData& d)
		{
			double trans = 0.0;
			auto grp = oapiMeshGroup(GetBaseVessel()->GetpanelMeshHandle0(), d.group);
			auto vrt = d.verts;

			trans = (oapiGetHUDMode() == d.mode) ? 0.0352 : 0.0;
			grp->Vtx[0].tu = vrt[0].tu + trans;
			grp->Vtx[1].tu = vrt[1].tu + trans;
			grp->Vtx[2].tu = vrt[2].tu + trans;
			grp->Vtx[3].tu = vrt[3].tu + trans;
		});

	return true;
}

void HUD::OnHudMode(int mode)
{
	// HUD mode is changing, if it is NOT changing to NONE, and we don't have power, turn it off.
	if ((HUD_NONE != mode) && (!HasPower()))
	{
		oapiSetHUDMode(HUD_NONE);
	}

	switch (oapiCockpitMode())
	{
	case COCKPIT_PANELS:
		bco::RunForEach<PnlData>(
			pnlData_,
			[&](const PnlData& d) {GetBaseVessel()->TriggerRedrawArea(0, 0, d.id); });
		break;

	case COCKPIT_VIRTUAL:
		bco::RunForEach<VcData>(
			vcData_,
			[&](const VcData& d) {GetBaseVessel()->TriggerRedrawArea(0, 0, d.id); });
		break;
	case COCKPIT_GENERIC:
		break;
	}
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
