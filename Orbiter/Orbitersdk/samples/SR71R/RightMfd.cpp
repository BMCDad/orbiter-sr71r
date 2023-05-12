//	RightMFD - SR-71r Orbiter Addon
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

#include "RightMfd.h"
#include "SR71r_mesh.h"
#include "MfdMets.h"

#include <assert.h>

RightMFD::RightMFD(bco::BaseVessel* vessel, double amps) :
bco::MFDBase(vessel, MFD_RIGHT, amps)
{
}

void RightMFD::OnSetClassCaps()
{
}


bool RightMFD::OnVCMouseEvent(int id, int event)
{
	return OnMouseEvent(id, event);
}

bool RightMFD::OnLoadVC(int id)
{
	auto vcMeshHandle = GetBaseVessel()->GetVCMeshHandle0();
	assert(vcMeshHandle != nullptr);

	SURFHANDLE surfHandle = oapiGetTextureHandle(vcMeshHandle, bt_mesh::SR71rVC::TXIDX_SR71R_100_VC2_dds);

	// Init our font:
	vcFont_.surfSource = surfHandle;
	vcFont_.charWidth = 12;
	vcFont_.charHeight = 20;
	vcFont_.sourceX = 4;
	vcFont_.sourceY = 2021;
	vcFont_.blankX = 1600;
	vcFont_.blankY = 2021;

	static VCMFDSPEC mfds_right = { 1, bt_mesh::SR71rVC::VCMfdRight_id };
	oapiVCRegisterMFD(MFD_RIGHT, &mfds_right);   // right MFD

	// Button 1
	auto eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	oapiVCRegisterArea(
		eventId,
		_R(MFDRLTEXL, MFD_B1TEX_TOP, MFDRLTEXR, MFD_B1TEX_BOT),
		PANEL_REDRAW_USER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
		PANEL_MAP_BACKGROUND,
		surfHandle);
	AssignKey(eventId, 0);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightL1_location, MFDBRAD);

	// Button 2
	eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	oapiVCRegisterArea(
		eventId,
		_R(MFDRLTEXL, MFD_B2TEX_TOP, MFDRLTEXR, MFD_B2TEX_BOT),
		PANEL_REDRAW_USER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
		PANEL_MAP_BACKGROUND,
		surfHandle);
	AssignKey(eventId, 1);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightL2_location, MFDBRAD);

	// Button 3
	eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	oapiVCRegisterArea(
		eventId,
		_R(MFDRLTEXL, MFD_B3TEX_TOP, MFDRLTEXR, MFD_B3TEX_BOT),
		PANEL_REDRAW_USER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
		PANEL_MAP_BACKGROUND,
		surfHandle);
	AssignKey(eventId, 2);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightL3_location, MFDBRAD);

	// Button 4
	eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	oapiVCRegisterArea(
		eventId,
		_R(MFDRLTEXL, MFD_B4TEX_TOP, MFDRLTEXR, MFD_B4TEX_BOT),
		PANEL_REDRAW_USER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
		PANEL_MAP_BACKGROUND,
		surfHandle);
	AssignKey(eventId, 3);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightL4_location, MFDBRAD);

	// Button 5
	eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	oapiVCRegisterArea(
		eventId,
		_R(MFDRLTEXL, MFD_B5TEX_TOP, MFDRLTEXR, MFD_B5TEX_BOT),
		PANEL_REDRAW_USER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
		PANEL_MAP_BACKGROUND,
		surfHandle);
	AssignKey(eventId, 4);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightL5_location, MFDBRAD);

	// Button 6
	eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	oapiVCRegisterArea(
		eventId,
		_R(MFDRLTEXL, MFD_B6TEX_TOP, MFDRLTEXR, MFD_B6TEX_BOT),
		PANEL_REDRAW_USER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
		PANEL_MAP_BACKGROUND,
		surfHandle);
	AssignKey(eventId, 5);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightL6_location, MFDBRAD);

	// Button R1
	eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	oapiVCRegisterArea(
		eventId,
		_R(MFDRRTEXL, MFD_B1TEX_TOP, MFDRRTEXR, MFD_B1TEX_BOT),
		PANEL_REDRAW_USER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
		PANEL_MAP_BACKGROUND,
		surfHandle);
	AssignKey(eventId, 6);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightR1_location, MFDBRAD);

	// Button R2
	eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	oapiVCRegisterArea(
		eventId,
		_R(MFDRRTEXL, MFD_B2TEX_TOP, MFDRRTEXR, MFD_B2TEX_BOT),
		PANEL_REDRAW_USER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
		PANEL_MAP_BACKGROUND,
		surfHandle);
	AssignKey(eventId, 7);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightR2_location, MFDBRAD);

	// Button R3
	eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	oapiVCRegisterArea(
		eventId,
		_R(MFDRRTEXL, MFD_B3TEX_TOP, MFDRRTEXR, MFD_B3TEX_BOT),
		PANEL_REDRAW_USER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
		PANEL_MAP_BACKGROUND,
		surfHandle);
	AssignKey(eventId, 8);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightR3_location, MFDBRAD);

	// Button R4
	eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	oapiVCRegisterArea(
		eventId,
		_R(MFDRRTEXL, MFD_B4TEX_TOP, MFDRRTEXR, MFD_B4TEX_BOT),
		PANEL_REDRAW_USER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
		PANEL_MAP_BACKGROUND,
		surfHandle);
	AssignKey(eventId, 9);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightR4_location, MFDBRAD);

	// Button R5
	eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	oapiVCRegisterArea(
		eventId,
		_R(MFDRRTEXL, MFD_B5TEX_TOP, MFDRRTEXR, MFD_B5TEX_BOT),
		PANEL_REDRAW_USER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
		PANEL_MAP_BACKGROUND,
		surfHandle);
	AssignKey(eventId, 10);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightR5_location, MFDBRAD);

	// Button 6
	eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	oapiVCRegisterArea(
		eventId,
		_R(MFDRRTEXL, MFD_B6TEX_TOP, MFDRRTEXR, MFD_B6TEX_BOT),
		PANEL_REDRAW_USER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
		PANEL_MAP_BACKGROUND,
		surfHandle);
	AssignKey(eventId, 11);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightR6_location, MFDBRAD);

	// PWR
	eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	AssignPwrKey(eventId);
	oapiVCRegisterArea(eventId, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_ONREPLAY);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightPWR_location, MFDBRAD);

	// SEL
	eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	AssignSelect(eventId);
	oapiVCRegisterArea(eventId, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_ONREPLAY);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightSEL_location, MFDBRAD);

	// MNU
	eventId = GetBaseVessel()->RegisterVCEvent(this, bco::VCIdMode::All);
	AssignMenu(eventId);
	oapiVCRegisterArea(eventId, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_ONREPLAY);
	oapiVCSetAreaClickmode_Spherical(eventId, bt_mesh::SR71rVC::MFCRightMNU_location, MFDBRAD);
	return true;
}

bool RightMFD::OnVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	bco::DrawBlankText(10, 13, surf, vcFont_);

	const char *label = GetButtonLabel(id);
	if (NULL != label)
	{
		bco::DrawSurfaceText(30, 13, label, bco::DrawTextFormat::Center, surf, vcFont_);
	}

	return true;
}