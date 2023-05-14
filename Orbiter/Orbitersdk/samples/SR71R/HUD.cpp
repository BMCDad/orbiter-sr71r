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
	for (auto& a : data_)
	{
		oapiVCRegisterArea(a.Id, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(a.Id, a.vcLocation, .01);
	}

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

	auto m = std::find_if(data_.begin(), data_.end(), [&](const HUDData& o) { return o.Id == id; });
	if (m == data_.end()) false;
	OnChanged(m->mode);
	return true;
}

bool HUD::OnVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	auto devMesh = GetBaseVessel()->GetVirtualCockpitMesh0();
	assert(devMesh != nullptr);

	const double offset = 0.0352;
	double trans = 0.0;

	auto m = std::find_if(data_.begin(), data_.end(), [&](const HUDData& o) { return o.Id == id; });
	if (m == data_.end()) false;
	trans = oapiGetHUDMode() == m->mode ? offset : 0.0;

	GROUPEDITSPEC change{};
	NTVERTEX* delta = new NTVERTEX[4];

	bco::TransformUV2d(m->vcVerts, delta, 4, _V(trans, 0.0, 0.0), 0.0);

	change.flags = GRPEDIT_VTXTEX;
	change.nVtx = 4;
	change.vIdx = NULL; //Just use the mesh order
	change.Vtx = delta;
	auto res = oapiEditMeshGroup(devMesh, m->vcGroupId, &change);
	delete[] delta;

	return true;
}

bool HUD::OnLoadPanel2D(int id, PANELHANDLE hPanel)
{
	for (auto& a : data_)
	{
		oapiRegisterPanelArea(a.Id, a.pnlRect, PANEL_REDRAW_USER);
		GetBaseVessel()->RegisterPanelArea(hPanel, a.Id, a.pnlRect, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
	}

	return true;
}

bool HUD::OnPanelMouseEvent(int id, int event)
{
	auto m = std::find_if(data_.begin(), data_.end(), [&](const HUDData& o) { return o.Id == id; });
	if (m == data_.end()) false;
	OnChanged(m->mode);
	return true;
}

bool HUD::OnPanelRedrawEvent(int id, int event)
{
	double trans = 0.0;
	auto m = std::find_if(data_.begin(), data_.end(), [&](const HUDData& o) { return o.Id == id; });
	if (m == data_.end()) false;

	auto grp = oapiMeshGroup(GetBaseVessel()->GetpanelMeshHandle0(), m->pnlGroupId);
	auto vrt = m->pnlVerts;

	trans = (oapiGetHUDMode() == m->mode) ? 0.0352 : 0.0;
	grp->Vtx[0].tu = vrt[0].tu + trans;
	grp->Vtx[1].tu = vrt[1].tu + trans;
	grp->Vtx[2].tu = vrt[2].tu + trans;
	grp->Vtx[3].tu = vrt[3].tu + trans;

	return true;
}

void HUD::OnHudMode(int mode)
{
	// HUD mode is changing, if it is NOT changing to NONE, and we don't have power, turn it off.
	if ((HUD_NONE != mode) && (!HasPower()))
	{
		oapiSetHUDMode(HUD_NONE);
	}

	// Trigger all buttons:
	for (auto& a : data_)
	{
		GetBaseVessel()->TriggerRedrawArea(0, 0, a.Id);
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
