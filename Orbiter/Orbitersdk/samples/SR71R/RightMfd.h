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

#include "bc_orbiter\MFDBase.h"
#include "bc_orbiter\Tools.h"
#include "bc_orbiter\PushButtonSwitch.h"
#include "bc_orbiter\TextureVisual.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class RightMFD : public bco::MFDBase
{
public:
	RightMFD(bco::BaseVessel* vessel, double amps);

	virtual void OnSetClassCaps() override;

	bool OnVCMouseEvent(int id, int event) override;
	bool OnLoadVC(int id) override;
	bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf) override;

	bool OnLoadPanel2D(int id, PANELHANDLE hPanel) override;
	bool OnPanelMouseEvent(int id, int event) override;
	bool OnPanelRedrawEvent(int id, int event, SURFHANDLE surf) override;

private:
	bco::FontInfo	vcFont_;

	const int PnlColLeftX = 1840;
	const int PnlColsDiff = 540;
	const int PnlRowsTop = 1740;
	const int PnlRowsDiff = 56;

	struct MFDData
	{
		int Id;
		int key;
		int row;
		int col;
		const VECTOR3& vcLoc;
	};

	std::vector<MFDData> data_
	{
		{ GetBaseVessel()->GetIdForComponent(this), 0,  0, 0, bm::vc::MFCRightL1_location },
		{ GetBaseVessel()->GetIdForComponent(this), 1,  1, 0, bm::vc::MFCRightL2_location },
		{ GetBaseVessel()->GetIdForComponent(this), 2,  2, 0, bm::vc::MFCRightL3_location },
		{ GetBaseVessel()->GetIdForComponent(this), 3,  3, 0, bm::vc::MFCRightL4_location },
		{ GetBaseVessel()->GetIdForComponent(this), 4,  4, 0, bm::vc::MFCRightL5_location },
		{ GetBaseVessel()->GetIdForComponent(this), 5,  5, 0, bm::vc::MFCRightL6_location },
		{ GetBaseVessel()->GetIdForComponent(this), 6,  0, 1, bm::vc::MFCRightR1_location },
		{ GetBaseVessel()->GetIdForComponent(this), 7,  1, 1, bm::vc::MFCRightR2_location },
		{ GetBaseVessel()->GetIdForComponent(this), 8,  2, 1, bm::vc::MFCRightR3_location },
		{ GetBaseVessel()->GetIdForComponent(this), 9,  3, 1, bm::vc::MFCRightR4_location },
		{ GetBaseVessel()->GetIdForComponent(this), 10, 4, 1, bm::vc::MFCRightR5_location },
		{ GetBaseVessel()->GetIdForComponent(this), 11, 5, 1, bm::vc::MFCRightR6_location }
	};

};