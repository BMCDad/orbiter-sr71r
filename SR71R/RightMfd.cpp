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

RightMFD::RightMFD(bco::power_provider& pwr, bco::vessel* vessel) :
	bco::MFDBase(pwr, vessel, MFD_RIGHT, 4.0)
{
}

void RightMFD::handle_set_class_caps(bco::vessel& vessel)
{
	for (auto& a : data_)
	{
		AssignKey(a.id, a.key);
	}

	AssignPwrKey(GetBaseVessel()->GetIdForComponent(this));
	AssignSelect(GetBaseVessel()->GetIdForComponent(this));
	AssignMenu(GetBaseVessel()->GetIdForComponent(this));
}

bool RightMFD::handle_load_vc(bco::vessel& vessel, int vcid)
{
	auto vcMeshHandle = vessel.GetVCMeshHandle0();
	assert(vcMeshHandle != nullptr);

	SURFHANDLE surfHandle = oapiGetTextureHandle(vcMeshHandle, bm::vc::TXIDX_SR71R_100_VC2_dds);

	// handle_set_class_caps our font:
	vcFont_.surfSource = surfHandle;
	vcFont_.charWidth = 12;
	vcFont_.charHeight = 20;
	vcFont_.sourceX = 4;
	vcFont_.sourceY = 2021;
	vcFont_.blankX = 1600;
	vcFont_.blankY = 2021;

	static VCMFDSPEC mfds_right = { 1, bm::vc::VCMfdRight_id };
	oapiVCRegisterMFD(MFD_RIGHT, &mfds_right);

	int rc_left			= MFDRLTEXL;						// Left side of left buttons;
	int rc_col2_offset	= MFDLRTEXL - MFDLLTEXL;			// Diff between right col and left
	int rc_top			= MFD_B1TEX_TOP;					// Top of row 1 buttons
	int rc_row_offset	= MFD_B2TEX_TOP - MFD_B1TEX_TOP;	// Difference in button y axis
	int btn_height		= MFD_B1TEX_BOT - MFD_B1TEX_TOP;	// Button height
	int btn_width		= MFDLLTEXR - MFDLLTEXL;			// Button width;

	for (auto& a : data_)
	{
		int rc_l = (a.col * rc_col2_offset) + rc_left;
		int rc_t = (a.row * rc_row_offset) + rc_top;
		int rc_r = rc_l + btn_width;
		int rc_b = rc_t + btn_height;
		oapiVCRegisterArea(
			a.id,
			_R(rc_l, rc_t, rc_r, rc_b),
			PANEL_REDRAW_USER,
			PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
			PANEL_MAP_BACKGROUND,
			surfHandle);

		oapiVCSetAreaClickmode_Spherical(a.id, a.vcLoc, .01);
	}


	// PWR
	oapiVCRegisterArea(GetPwrKey(), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_ONREPLAY);
	oapiVCSetAreaClickmode_Spherical(GetPwrKey(), bm::vc::MFCRightPWR_loc, MFDBRAD);

	// SEL
	oapiVCRegisterArea(GetSelectKey(), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_ONREPLAY);
	oapiVCSetAreaClickmode_Spherical(GetSelectKey(), bm::vc::MFCRightSEL_loc, MFDBRAD);

	// MNU
	oapiVCRegisterArea(GetMenuKey(), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_ONREPLAY);
	oapiVCSetAreaClickmode_Spherical(GetMenuKey(), bm::vc::MFCRightMNU_loc, MFDBRAD);
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

bool RightMFD::handle_load_panel(bco::vessel& vessel, int id, PANELHANDLE hPanel)
{
	auto panelMesh = vessel.GetpanelMeshHandle0();

	// Orbiter bug with panel MFDs, it expects a specific vertex sequence, so we need to re-arrange.
	static NTVERTEX VTX_MFD[1][4] = {
		{
			bm::pnl::pnlMFDRight_vrt[2],
			bm::pnl::pnlMFDRight_vrt[3],
			bm::pnl::pnlMFDRight_vrt[0],
			bm::pnl::pnlMFDRight_vrt[1]
		}
	};
	static WORD IDX_MFD[6] = {
		0,1,2,
		3,2,1
	};

	MESHGROUP grp_lmfd = { VTX_MFD[0], IDX_MFD, 4, 6, 0, 0, 0, 0, 0 };
	auto lmfd_grp = oapiAddMeshGroup(panelMesh, &grp_lmfd);   // left MFD

	vessel.RegisterPanelMFDGeometry(hPanel, MFD_RIGHT, 0, lmfd_grp);

	SURFHANDLE surfHandle = oapiGetTextureHandle(panelMesh, bm::pnl::TXIDX_SR71R_100_2DPanel_dds);

	// handle_set_class_caps our font:
	vcFont_.surfSource = surfHandle;
	vcFont_.charWidth = 12;
	vcFont_.charHeight = 20;
	vcFont_.sourceX = 4;
	vcFont_.sourceY = 2;
	vcFont_.blankX = 1600;
	vcFont_.blankY = 2;

	for (auto& a : data_)
	{
		vessel.RegisterPanelArea(
			hPanel,
			a.id,
			a.pnlRC,
			PANEL_REDRAW_USER,
			PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
			surfHandle);
	}

	vessel.RegisterPanelArea(
		hPanel,
		GetPwrKey(),
		bm::pnl::pnlRightMFDPwr_RC,
		PANEL_REDRAW_NEVER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_ONREPLAY);

	// SEL
	vessel.RegisterPanelArea(
		hPanel,
		GetSelectKey(),
		bm::pnl::pnlRightMFDSel_RC,
		PANEL_REDRAW_NEVER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_ONREPLAY);

	// MNU
	vessel.RegisterPanelArea(
		hPanel,
		GetSelectKey(),
		bm::pnl::pnlRightMFDMenu_RC,
		PANEL_REDRAW_NEVER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_ONREPLAY);

	return true;
}

bool RightMFD::OnPanelRedrawEvent(int id, int event, SURFHANDLE surf)
{
	auto m = std::find_if(data_.begin(), data_.end(), [&](const MFDData& o) { return o.id == id; });
	if (m == data_.end()) return false;

	const char* label = GetButtonLabel(id);

	auto xPos = PnlColLeftX + (m->col * PnlColsDiff);
	auto yPos = PnlRowsTop + (m->row * PnlRowsDiff);
	bco::DrawBlankText(xPos - 20, yPos, surf, vcFont_);

	if (NULL != label)
	{
		bco::DrawSurfaceText(xPos, yPos, label, bco::DrawTextFormat::Center, surf, vcFont_);
	}
	return true;
}