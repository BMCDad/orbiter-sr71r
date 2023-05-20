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
#include "bc_orbiter\bco.h"
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
	bco::RunForEach<VcData>(vcData_, [this](const VcData& d)
		{
			for (auto& a : vcData_)
			{
				oapiVCRegisterArea(a.id, PANEL_REDRAW_USER, PANEL_MOUSE_DOWN);
				oapiVCSetAreaClickmode_Spherical(a.id, a.loc, .01);
			}
		});

	return true;
}

bool RCSSystem::OnVCMouseEvent(int id, int event)
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

bool RCSSystem::OnVCRedrawEvent(int id, int event, SURFHANDLE surf)
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
					_V(GetBaseVessel()->GetAttitudeMode() ==d.mode ? 0.0352 : 0.0,
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

bool RCSSystem::OnLoadPanel2D(int id, PANELHANDLE hPanel)
{
	bco::RunForEach<PnlData>(pnlData_, [&](const PnlData& d)
		{
			oapiRegisterPanelArea(d.id, d.rc, PANEL_REDRAW_USER);
			GetBaseVessel()->RegisterPanelArea(hPanel, d.id, d.rc, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
		});

	return true;
}

bool RCSSystem::OnPanelMouseEvent(int id, int event)
{
	return bco::RunFor<PnlData>(pnlData_,
		[&](const PnlData& d) {return d.id == id; },
		[this](const PnlData& d) { OnChanged(d.mode); });
}

bool RCSSystem::OnPanelRedrawEvent(int id, int event, SURFHANDLE surf)
{
	return bco::RunFor<PnlData>(pnlData_,
		[&](const PnlData& d) {return d.id == id; },
		[this](const PnlData& d)
		{
			double trans = 0.0;
			auto grp = oapiMeshGroup(GetBaseVessel()->GetpanelMeshHandle0(), d.group);
			auto vrt = d.verts;

			trans = (GetBaseVessel()->GetAttitudeMode() == d.mode) ? 0.0352 : 0.0;
			grp->Vtx[0].tu = vrt[0].tu + trans;
			grp->Vtx[1].tu = vrt[1].tu + trans;
			grp->Vtx[2].tu = vrt[2].tu + trans;
			grp->Vtx[3].tu = vrt[3].tu + trans;
		});
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

void RCSSystem::OnChanged(int mode)
{
	auto currentMode = GetBaseVessel()->GetAttitudeMode();

	if (GetBaseVessel()->IsCreated())
	{
		auto newMode = ((mode == currentMode) || !HasPower()) ? RCS_NONE : mode;
		GetBaseVessel()->SetAttitudeMode(newMode);
	}
}
