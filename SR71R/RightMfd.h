//	RightMFD - SR-71r Orbiter Addon
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

#pragma once

#include "Orbitersdk.h"

#include "../bc_orbiter/MFDBase.h"
#include "../bc_orbiter/Tools.h"
#include "../bc_orbiter/Control.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class RightMFD :
	  public bco::MFDBase
	, public bco::set_class_caps
	, public bco::load_vc
	, public bco::load_panel
{
public:
	RightMFD(bco::power_provider& pwr, bco::vessel* vessel);

	virtual void handle_set_class_caps(bco::vessel& vessel) override;

	// load_vc
	bool handle_load_vc(bco::vessel& vessel, int vcid) override;
	bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf);

	bool handle_load_panel(bco::vessel& vessel, int id, PANELHANDLE hPanel) override;
	bool OnPanelRedrawEvent(int id, int event, SURFHANDLE surf);

private:
	bco::FontInfo	vcFont_;

	const int PnlColLeftX = 1968;
	const int PnlColsDiff = 540;
	const int PnlRowsTop = 1728;
	const int PnlRowsDiff = 56;

	struct MFDData
	{
		int id;
		int key;
		int row;
		int col;
		const VECTOR3& vcLoc;
		const RECT& pnlRC;
	};

	std::vector<MFDData> data_
	{
		{ vessel_.GetIdForComponent(this), 0,  0, 0, bm::vc::MFCRightL1_loc, bm::pnl::pnlRightMFD1_RC },
		{ vessel_.GetIdForComponent(this), 1,  1, 0, bm::vc::MFCRightL2_loc, bm::pnl::pnlRightMFD2_RC },
		{ vessel_.GetIdForComponent(this), 2,  2, 0, bm::vc::MFCRightL3_loc, bm::pnl::pnlRightMFD3_RC },
		{ vessel_.GetIdForComponent(this), 3,  3, 0, bm::vc::MFCRightL4_loc, bm::pnl::pnlRightMFD4_RC },
		{ vessel_.GetIdForComponent(this), 4,  4, 0, bm::vc::MFCRightL5_loc, bm::pnl::pnlRightMFD5_RC },
		{ vessel_.GetIdForComponent(this), 5,  5, 0, bm::vc::MFCRightL6_loc, bm::pnl::pnlRightMFD6_RC },
		{ vessel_.GetIdForComponent(this), 6,  0, 1, bm::vc::MFCRightR1_loc, bm::pnl::pnlRightMFD7_RC },
		{ vessel_.GetIdForComponent(this), 7,  1, 1, bm::vc::MFCRightR2_loc, bm::pnl::pnlRightMFD8_RC },
		{ vessel_.GetIdForComponent(this), 8,  2, 1, bm::vc::MFCRightR3_loc, bm::pnl::pnlRightMFD9_RC },
		{ vessel_.GetIdForComponent(this), 9,  3, 1, bm::vc::MFCRightR4_loc, bm::pnl::pnlRightMFD10_RC },
		{ vessel_.GetIdForComponent(this), 10, 4, 1, bm::vc::MFCRightR5_loc, bm::pnl::pnlRightMFD11_RC },
		{ vessel_.GetIdForComponent(this), 11, 5, 1, bm::vc::MFCRightR6_loc, bm::pnl::pnlRightMFD12_RC }
	};
};