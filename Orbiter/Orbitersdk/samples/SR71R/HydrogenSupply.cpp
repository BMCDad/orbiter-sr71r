//	HydrogenSupply - SR-71r Orbiter Addon
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

#include "HydrogenSupply.h"
#include "SR71r_mesh.h"
#include "ShipMets.h"

#include <math.h>
#include <assert.h>

HydrogenSupply::HydrogenSupply(bco::BaseVessel* vessel, double capacity) :
	CryogenicTank(vessel, capacity, HYDRO_NER, "HYDROGEN"),
	lightSupplyAvailable_(	bt_mesh::SR71rVC::LH2SupplyOnLight_verts,	bt_mesh::SR71rVC::LH2SupplyOnLight_id),
	lightValveOpen_(		bt_mesh::SR71rVC::LH2ValveOpenSwitch_verts,	bt_mesh::SR71rVC::LH2ValveOpenSwitch_id)
{
	switchValveOpen_.SetPressedFunc([this] {ToggleFilling(); });
    vessel->RegisterVCEventTarget(&switchValveOpen_);
}

void HydrogenSupply::SetClassCaps()
{
    gaugeHydro_.Setup(GetBaseVessel());
}

void HydrogenSupply::Step(double simt, double simdt, double mjd)
{
    gaugeHydro_.SetState(GetLevel());

    if (fabs(simt - prevTime_) > 0.1)
	{
        CryogenicTank::Step(simt, (simt - prevTime_), mjd);
		Update();
		prevTime_ = simt;
	}

	if (IsFilling())
	{
		FillTank(HYDROGEN_FILL_RATE * simdt);
	}
}

bool HydrogenSupply::VCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	auto devMesh = GetBaseVessel()->GetVirtualCockpitMesh0();
	assert(devMesh != nullptr);

	const double availOffset = 0.0244;
	const double buttonOffset = 0.0352;
	double trans = 0.0;

	trans = IsAvailable() ? availOffset : 0.0;
	lightSupplyAvailable_.SetTranslate(_V(trans, 0.0, 0.0));
	lightSupplyAvailable_.Draw(devMesh);

	trans = IsFilling() ? buttonOffset : 0.0;
	lightValveOpen_.SetTranslate(_V(trans, 0.0, 0.0));
	lightValveOpen_.Draw(devMesh);

	return true;
}

bool HydrogenSupply::LoadVC(int id)
{
	// Redraw areas:
	areaId_ = GetBaseVessel()->RegisterVCRedrawEvent(this);
	oapiVCRegisterArea(areaId_, PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE);

	return true;
}

void HydrogenSupply::Update()
{
    IsAvailable(GetBaseVessel()->IsStoppedOrDocked() && HasPower());

	if (!IsAvailable())
	{
		IsFilling(false);
	}

	auto vessel = GetBaseVessel();
	vessel->UpdateUIArea(areaId_);
}