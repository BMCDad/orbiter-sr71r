//	StatusBoard - SR-71r Orbiter Addon
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

#include "StatusBoard.h"
#include "SR71r_mesh.h"

#include <assert.h>

StatusBoard::StatusBoard(bco::BaseVessel* vessel, double amps) :
bco::PoweredComponent(vessel, amps, 20.0),
cargoBay_(nullptr),
msgCanopy_(				bt_mesh::SR71rVC::MsgLightCanopy_verts,		bt_mesh::SR71rVC::MsgLightCanopy_id),
msgCargoBay_(			bt_mesh::SR71rVC::MsgLightBay_verts,			bt_mesh::SR71rVC::MsgLightBay_id),
msgKeasWarning_(		bt_mesh::SR71rVC::MsgLightKeasWarn_verts,	bt_mesh::SR71rVC::MsgLightKeasWarn_id),
msgBattery_(			bt_mesh::SR71rVC::MsgLightBattery_verts,		bt_mesh::SR71rVC::MsgLightBattery_id),
msgThrustLimit_(		bt_mesh::SR71rVC::MsgLightThrustLimit_verts,	bt_mesh::SR71rVC::MsgLightThrustLimit_id),
msgApu_(				bt_mesh::SR71rVC::MsgLightAPU_verts,			bt_mesh::SR71rVC::MsgLightAPU_id),
msgDock_(				bt_mesh::SR71rVC::MsgLightDock_verts,		bt_mesh::SR71rVC::MsgLightDock_id),
msgFuel_(				bt_mesh::SR71rVC::MsgLightFuelWarn_verts,	bt_mesh::SR71rVC::MsgLightFuelWarn_id),
msgAirBrake_(			bt_mesh::SR71rVC::MsgLightSpeedBrake_verts,	bt_mesh::SR71rVC::MsgLightSpeedBrake_id),
msgHoverDoors_(         bt_mesh::SR71rVC::MsgLightHover_verts,       bt_mesh::SR71rVC::MsgLightHover_id),
msgRetroDoors_(         bt_mesh::SR71rVC::MsgLightRetro_verts,       bt_mesh::SR71rVC::MsgLightRetro_id)
{
}

void StatusBoard::SetClassCaps()
{
}

bool StatusBoard::VCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	auto devMesh = GetBaseVessel()->GetVirtualCockpitMesh0();
	assert(devMesh != nullptr);

	DrawDoorsMsg(devMesh);
	DrawKEASMsg(devMesh);
	DrawBattery(devMesh);
	DrawThrustLimit(devMesh);
	DrawAPU(devMesh);
	DrawDock(devMesh);
	DrawFuel(devMesh);
	DrawAirBrakeMsg(devMesh);
	return true;
}

void StatusBoard::Step(double simt, double simdt, double mjd)
{
	if (fabs(prevTime - simt) < 0.2)
	{
		return;
	}

	prevTime = simt;
	GetBaseVessel()->UpdateUIArea(vcRedrawArea_);
}

bool StatusBoard::LoadVC(int id)
{
	vcRedrawArea_ = GetBaseVessel()->RegisterVCRedrawEvent(this);
	oapiVCRegisterArea(vcRedrawArea_, PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE);
	return true;
}

/**
	Returns the offset for the following:
	0.0				- off
	0.0 < a < 1.0	- warn
	1.0				- on
*/
double StatusBoard::GetWarnMsgOffset(double state)
{
	auto ofs = OffOffset;

	if (HasPower())
	{
		if ((state > 0.0) && (state < 1.0))
		{
			ofs = WrnOffset;
		}
		else
		{
			ofs = (state == 0.0) ? OffOffset : OnOffset;
		}
	}

	return ofs;
}

void StatusBoard::DrawDoorsMsg(DEVMESHHANDLE devMesh)
{
	auto ofs = GetWarnMsgOffset(canopy_->GetCanopyState());
	msgCanopy_.SetTranslate(_V(ofs, 0.0, 0.0));
	msgCanopy_.Draw(devMesh);

	ofs = GetWarnMsgOffset(cargoBay_->GetCargoBayState());
	msgCargoBay_.SetTranslate(_V(ofs, 0.0, 0.0));
	msgCargoBay_.Draw(devMesh);

    ofs = GetWarnMsgOffset(retro_->GetRetroDoorsState());
    msgRetroDoors_.SetTranslate(_V(ofs, 0.0, 0.0));
    msgRetroDoors_.Draw(devMesh);

    ofs = GetWarnMsgOffset(hover_->GetHoverDoorsState());
    msgHoverDoors_.SetTranslate(_V(ofs, 0.0, 0.0));
    msgHoverDoors_.Draw(devMesh);
}

void StatusBoard::DrawKEASMsg(DEVMESHHANDLE devMesh)
{
	auto ofs = (avionics_->IsOverSpeed() ? ErrOffset : OffOffset);
	msgKeasWarning_.SetTranslate(_V(ofs, 0.0, 0.0));
	msgKeasWarning_.Draw(devMesh);
}

void StatusBoard::DrawThrustLimit(DEVMESHHANDLE devMesh)
{
	auto ofs = GetWarnMsgOffset(propulsion_->ThrustLimitSwitch().IsOn() ? 0.0 : 0.5);
	msgThrustLimit_.SetTranslate(_V(ofs, 0.0, 0.0));
	msgThrustLimit_.Draw(devMesh);
}

void StatusBoard::DrawBattery(DEVMESHHANDLE devMesh)
{
	auto ofs = GetWarnMsgOffset(power_->IsBatteryPower() ? 0.5 : 0.0);
	msgBattery_.SetTranslate(_V(ofs, 0.0, 0.0));
	msgBattery_.Draw(devMesh);
}

void StatusBoard::DrawAPU(DEVMESHHANDLE devMesh)
{
	auto ofs = GetWarnMsgOffset(apu_->GetHydraulicLevel());

	msgApu_.SetTranslate(_V(ofs, 0.0, 0.0));
	msgApu_.Draw(devMesh);
}

void StatusBoard::DrawDock(DEVMESHHANDLE devMesh)
{
	auto id = GetBaseVessel()->DockingStatus(0);

	auto ofs = id == 1.0 ? OnOffset : OffOffset;

	msgDock_.SetTranslate(_V(ofs, 0.0, 0.0));
	msgDock_.Draw(devMesh);
}

void StatusBoard::DrawFuel(DEVMESHHANDLE devMesh)
{
	auto fuelLevel = propulsion_->GetMainFuelLevel();
	auto ofsf = OffOffset;

    if (HasPower())
    {
        if (fuelLevel < 0.1)
        {
            ofsf = WrnOffset;
        }

        if (fuelLevel < 0.03)
        {
            ofsf = ErrOffset;
        }
    }

	msgFuel_.SetTranslate(_V(ofsf, 0.0, 0.0));
	msgFuel_.Draw(devMesh);
}

void StatusBoard::DrawAirBrakeMsg(DEVMESHHANDLE devMesh)
{
	auto ofs = OffOffset;
	if (nullptr != airBrake_)
	{
		ofs = (airBrake_->GetAirBrakeState() > 0.0) ? WrnOffset : OffOffset;
	}

	msgAirBrake_.SetTranslate(_V(ofs, 0.0, 0.0));
	msgAirBrake_.Draw(devMesh);
}
