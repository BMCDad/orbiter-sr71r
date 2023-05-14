//	RCSSystem - SR-71r Orbiter Addon
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
#include "bc_orbiter\Tools.h"

#include "RCSSystem.h"
#include "SR71r_mesh.h"


RCSSystem::RCSSystem(bco::BaseVessel* vessel, double amps) :
PoweredComponent(vessel, amps, 20.0)
{
}


void RCSSystem::OnSetClassCaps()
{
}

double RCSSystem::CurrentDraw()
{
    return (GetBaseVessel()->GetAttitudeMode() == RCS_NONE) ? 0.0 : PoweredComponent::CurrentDraw();
}

bool RCSSystem::OnLoadVC(int id)
{
	for (auto& a : data_)
	{
		oapiVCRegisterArea(a.Id, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(a.Id, a.vcLocation, .01);
	}

	return true;
}

bool RCSSystem::OnVCMouseEvent(int id, int event)
{
	if (event != PANEL_MOUSE_LBDOWN) return false;

	auto m = std::find_if(data_.begin(), data_.end(), [&](const RCSData& o) { return o.Id == id; });
	if (m == data_.end()) false;
	OnChanged(m->mode);
	return true;
}

bool RCSSystem::OnVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	auto devMesh = GetBaseVessel()->GetVirtualCockpitMesh0();
	assert(devMesh != nullptr);

	auto vessel = GetBaseVessel();
	const double offset = 0.0352;
	double trans = 0.0;

	auto m = std::find_if(data_.begin(), data_.end(), [&](const RCSData& o) { return o.Id == id; });
	if (m == data_.end()) false;
	trans = GetBaseVessel()->GetAttitudeMode() == m->mode ? offset : 0.0;

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

bool RCSSystem::OnLoadPanel2D(int id, PANELHANDLE hPanel)
{
	for (auto& a : data_)
	{
		oapiRegisterPanelArea(a.Id, a.pnlRect, PANEL_REDRAW_USER);
		GetBaseVessel()->RegisterPanelArea(hPanel, a.Id, a.pnlRect, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
	}

	return true;
}

bool RCSSystem::OnPanelMouseEvent(int id, int event)
{
	auto m = std::find_if(data_.begin(), data_.end(), [&](const RCSData& o) { return o.Id == id; });
	if (m == data_.end()) false;
	OnChanged(m->mode);
	return true;
}

bool RCSSystem::OnPanelRedrawEvent(int id, int event)
{
	double trans = 0.0;
	auto m = std::find_if(data_.begin(), data_.end(), [&](const RCSData& o) { return o.Id == id; });
	if (m == data_.end()) false;

	auto grp = oapiMeshGroup(GetBaseVessel()->GetpanelMeshHandle0(), m->pnlGroupId);
	auto vrt = m->pnlVerts;

	trans = (GetBaseVessel()->GetAttitudeMode() == m->mode) ? 0.0352 : 0.0;
	grp->Vtx[0].tu = vrt[0].tu + trans;
	grp->Vtx[1].tu = vrt[1].tu + trans;
	grp->Vtx[2].tu = vrt[2].tu + trans;
	grp->Vtx[3].tu = vrt[3].tu + trans;

	return true;
}

void RCSSystem::ChangePowerLevel(double newLevel)
{
	PoweredComponent::ChangePowerLevel(newLevel);
	if (!HasPower())
	{
		if (GetBaseVessel()->IsCreated())
		{
			GetBaseVessel()->SetAttitudeMode(RCS_NONE);
		}
	}
}


// Callback:
void RCSSystem::OnRCSMode(int mode)
{
	if ((RCS_NONE != mode) && (!HasPower()))
	{
		GetBaseVessel()->SetAttitudeMode(RCS_NONE);
	}

	// Trigger all buttons:
	for (auto& a : data_)
	{
		GetBaseVessel()->TriggerRedrawArea(0, 0, a.Id);
	}
}

void RCSSystem::OnChanged(int mode)
{
	auto currentMode = GetBaseVessel()->GetAttitudeMode();

	if (GetBaseVessel()->IsCreated())
	{
		auto newMode = ((mode == currentMode) || !HasPower()) ? RCS_NONE : mode;
		GetBaseVessel()->SetAttitudeMode(newMode);
	}
}
