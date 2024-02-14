//	LeftMFD - SR-71r Orbiter Addon
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

#pragma once

#include "Orbitersdk.h"

#include "bc_orbiter/MFDBase.h"
#include "bc_orbiter/Tools.h"
#include "bc_orbiter/Control.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class LeftMFD : 
	  public bco::MFDBase
	, public bco::HandlesSetClassCaps
	, public bco::HandlesVCLoading
	, public bco::HandlesPanelLoading
{
public:
	LeftMFD(bco::PowerProvider& pwr, bco::vessel* vessel);

	virtual void HandleSetClassCaps(bco::vessel& vessel) override;

	// load_vc
	bool HandleLoacVC(bco::vessel& vessel, int vcid) override;
	bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf);

	bool HandleLoadPanel(bco::vessel& vessel, int id, PANELHANDLE hPanel) override;
	bool OnPanelRedrawEvent(int id, int event, SURFHANDLE surf);

private:
	bco::FontInfo	vcFont_;

	const int PnlColLeftX = 54;
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
		{ vessel_.GetIdForComponent(this), 0,  0, 0, bm::vc::MFCLeftL1_loc, bm::pnl::pnlLeftMFD1_RC },
		{ vessel_.GetIdForComponent(this), 1,  1, 0, bm::vc::MFCLeftL2_loc, bm::pnl::pnlLeftMFD2_RC },
		{ vessel_.GetIdForComponent(this), 2,  2, 0, bm::vc::MFCLeftL3_loc, bm::pnl::pnlLeftMFD3_RC },
		{ vessel_.GetIdForComponent(this), 3,  3, 0, bm::vc::MFCLeftL4_loc, bm::pnl::pnlLeftMFD4_RC },
		{ vessel_.GetIdForComponent(this), 4,  4, 0, bm::vc::MFCLeftL5_loc, bm::pnl::pnlLeftMFD5_RC },
		{ vessel_.GetIdForComponent(this), 5,  5, 0, bm::vc::MFCLeftL6_loc, bm::pnl::pnlLeftMFD6_RC },
		{ vessel_.GetIdForComponent(this), 6,  0, 1, bm::vc::MFCLeftR1_loc, bm::pnl::pnlLeftMFD7_RC },
		{ vessel_.GetIdForComponent(this), 7,  1, 1, bm::vc::MFCLeftR2_loc, bm::pnl::pnlLeftMFD8_RC },
		{ vessel_.GetIdForComponent(this), 8,  2, 1, bm::vc::MFCLeftR3_loc, bm::pnl::pnlLeftMFD9_RC },
		{ vessel_.GetIdForComponent(this), 9,  3, 1, bm::vc::MFCLeftR4_loc, bm::pnl::pnlLeftMFD10_RC },
		{ vessel_.GetIdForComponent(this), 10, 4, 1, bm::vc::MFCLeftR5_loc, bm::pnl::pnlLeftMFD11_RC },
		{ vessel_.GetIdForComponent(this), 11, 5, 1, bm::vc::MFCLeftR6_loc, bm::pnl::pnlLeftMFD12_RC }
	};
};