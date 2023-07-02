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
cargoBay_(nullptr)
//msgCanopy_(				bm::vc::MsgLightCanopy_verts,		bm::vc::MsgLightCanopy_id),
//msgCargoBay_(			bm::vc::MsgLightBay_verts,			bm::vc::MsgLightBay_id),
////msgKeasWarning_(		bm::vc::MsgLightKeasWarn_verts,	bm::vc::MsgLightKeasWarn_id),
//msgBattery_(			bm::vc::MsgLightBattery_verts,		bm::vc::MsgLightBattery_id),
//msgThrustLimit_(		bm::vc::MsgLightThrustLimit_verts,	bm::vc::MsgLightThrustLimit_id),
//msgApu_(				bm::vc::MsgLightAPU_verts,			bm::vc::MsgLightAPU_id),
//msgDock_(				bm::vc::MsgLightDock_verts,		bm::vc::MsgLightDock_id),
//msgFuel_(				bm::vc::MsgLightFuelWarn_verts,	bm::vc::MsgLightFuelWarn_id),
//msgAirBrake_(			bm::vc::MsgLightSpeedBrake_verts,	bm::vc::MsgLightSpeedBrake_id),
//msgHoverDoors_(         bm::vc::MsgLightHover_verts,       bm::vc::MsgLightHover_id),
//msgRetroDoors_(         bm::vc::MsgLightRetro_verts,       bm::vc::MsgLightRetro_id)
{
}

void StatusBoard::OnSetClassCaps()
{
//	vcRedrawArea_ = GetBaseVessel()->RegisterVCRedrawEvent(this);
}

bool StatusBoard::OnVCRedrawEvent(int id, int event, SURFHANDLE surf)
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

bool StatusBoard::OnLoadVC(int id)
{
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
	//auto ofs = GetWarnMsgOffset(canopy_->GetCanopyState());
	//msgCanopy_.SetTranslate(_V(ofs, 0.0, 0.0));
	//msgCanopy_.RotateMesh(devMesh);

	////ofs = GetWarnMsgOffset(cargoBay_->GetCargoBayState());	TODO <- signal
	//msgCargoBay_.SetTranslate(_V(ofs, 0.0, 0.0));
	//msgCargoBay_.RotateMesh(devMesh);

 //   ofs = GetWarnMsgOffset(retro_->GetRetroDoorsState());
 //   msgRetroDoors_.SetTranslate(_V(ofs, 0.0, 0.0));
 //   msgRetroDoors_.RotateMesh(devMesh);

 //   ofs = GetWarnMsgOffset(hover_->GetHoverDoorsState());
 //   msgHoverDoors_.SetTranslate(_V(ofs, 0.0, 0.0));
 //   msgHoverDoors_.RotateMesh(devMesh);
}

void StatusBoard::DrawKEASMsg(DEVMESHHANDLE devMesh)
{
	//auto ofs = (avionics_->IsOverSpeed() ? ErrOffset : OffOffset);
	//msgKeasWarning_.SetTranslate(_V(ofs, 0.0, 0.0));
	//msgKeasWarning_.RotateMesh(devMesh);
}

void StatusBoard::DrawThrustLimit(DEVMESHHANDLE devMesh)
{
// TODO	auto ofs = GetWarnMsgOffset(propulsion_->ThrustLimitSwitch().IsOn() ? 0.0 : 0.5);
	//auto ofs = GetWarnMsgOffset(0.0);  // TODO ^
	//msgThrustLimit_.SetTranslate(_V(ofs, 0.0, 0.0));
	//msgThrustLimit_.RotateMesh(devMesh);
}

void StatusBoard::DrawBattery(DEVMESHHANDLE devMesh)
{
	// TODO : like much else here, this is a signal from the power system.
	//auto ofs = GetWarnMsgOffset(power_->IsBatteryPower() ? 0.5 : 0.0);
	//msgBattery_.SetTranslate(_V(ofs, 0.0, 0.0));
	//msgBattery_.RotateMesh(devMesh);
}

void StatusBoard::DrawAPU(DEVMESHHANDLE devMesh)
{
	//auto ofs = GetWarnMsgOffset(apu_->GetHydraulicLevel());

	//msgApu_.SetTranslate(_V(ofs, 0.0, 0.0));
	//msgApu_.RotateMesh(devMesh);
}

void StatusBoard::DrawDock(DEVMESHHANDLE devMesh)
{
	//auto id = GetBaseVessel()->DockingStatus(0);

	//auto ofs = id == 1.0 ? OnOffset : OffOffset;

	//msgDock_.SetTranslate(_V(ofs, 0.0, 0.0));
	//msgDock_.RotateMesh(devMesh);
}

void StatusBoard::DrawFuel(DEVMESHHANDLE devMesh)
{
	auto fuelLevel = 1.0; // TODO propulsion_->GetMainFuelLevel();
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

	//msgFuel_.SetTranslate(_V(ofsf, 0.0, 0.0));
	//msgFuel_.RotateMesh(devMesh);
}

void StatusBoard::DrawAirBrakeMsg(DEVMESHHANDLE devMesh)
{
	auto ofs = OffOffset;
	if (nullptr != airBrake_)
	{
		ofs = (airBrake_->GetAirBrakeState() > 0.0) ? WrnOffset : OffOffset;
	}

	//msgAirBrake_.SetTranslate(_V(ofs, 0.0, 0.0));
	//msgAirBrake_.RotateMesh(devMesh);
}
