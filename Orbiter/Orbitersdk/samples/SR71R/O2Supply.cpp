//	O2Supply - SR-71r Orbiter Addon
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

#include "O2Supply.h"
#include "Orbitersdk.h"
#include "SR71r_mesh.h"
#include "ShipMets.h"

#include <cmath>
#include <assert.h>

O2Supply::O2Supply(bco::BaseVessel* vessel, double capacity) :
CryogenicTank(vessel, capacity, O2_NER, "OXYGEN"),
prevTime_(0.0),
lightSupplyAvailable_(	bt_mesh::SR71rVC::LOXSupplyOnLight_verts,	bt_mesh::SR71rVC::LOXSupplyOnLight_id),
lightValveOpen_(		bt_mesh::SR71rVC::LOXValveOpenSwitch_verts,	bt_mesh::SR71rVC::LOXValveOpenSwitch_id)
{
	switchValveOpen_.SetPressedFunc([this] {ToggleFilling(); });
    vessel->RegisterVCEventTarget(&switchValveOpen_);
}

void O2Supply::SetClassCaps()
{
    gaugeOxygenLevel_.Setup(GetBaseVessel());
}

void O2Supply::Step(double simt, double simdt, double mjd)
{
    gaugeOxygenLevel_.SetState(GetState());

	if (fabs(simt - prevTime_) > 0.2)
	{
        CryogenicTank::Step(simt, (simt - prevTime_), mjd);

		Update();
		prevTime_ = simt;
	}

	if (IsFilling())
	{
		FillTank(OXYGEN_FILL_RATE * simdt);
		GetBaseVessel()->PlaySound(LOX_SUPPLY_ID, true, false);
	}
	else if (GetBaseVessel()->IsSoundRunning(LOX_SUPPLY_ID)) GetBaseVessel()->StopSound(LOX_SUPPLY_ID);

	if (GetLevel() == 0.0) {
		if (!isDepSoundRun_) {
			GetBaseVessel()->PlaySound(LOX_DEP_ID, false, true);
			isDepSoundRun_ = true;
		}
	}
	else if (GetLevel() <= 0.05) {
		if (!isLowSoundRun_) {
			GetBaseVessel()->PlaySound(LOX_LOW_ID, false, true);
			isLowSoundRun_ = true;
		}
	}
	else if (GetLevel() == 1.0) {
		if (!isFullSoundRun_) {
			GetBaseVessel()->PlaySound(LOX_FULL_ID, false, true);
			isFullSoundRun_ = true;
		}
	}
	else isDepSoundRun_ = isLowSoundRun_ = isFullSoundRun_ = false;
}


bool O2Supply::VCRedrawEvent(int id, int event, SURFHANDLE surf)
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

bool O2Supply::LoadVC(int id)
{
	// Redraw areas:
	areaId_ = GetBaseVessel()->RegisterVCRedrawEvent(this);
	oapiVCRegisterArea(areaId_, PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE);

	return true;
}

void O2Supply::Update()
{
    IsAvailable(GetBaseVessel()->IsStoppedOrDocked() && HasPower());

	if (!IsAvailable())
	{
		IsFilling(false);
	}

	auto vessel = GetBaseVessel();
	vessel->UpdateUIArea(areaId_);
}