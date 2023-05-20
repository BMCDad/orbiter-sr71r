//	RCSSystem - SR-71r Orbiter Addon
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

#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\VCRotorSwitch.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class VESSEL3;

/** RCSMode
*/

class RCSSystem : public bco::PoweredComponent
{
public:
	RCSSystem(bco::BaseVessel* vessel, double amps);

	void OnSetClassCaps() override;

	bool OnLoadVC(int id) override;
	bool OnVCMouseEvent(int id, int event) override;
	bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf) override;

	bool OnLoadPanel2D(int id, PANELHANDLE hPanel) override;
	bool OnPanelMouseEvent(int id, int event) override;
	bool OnPanelRedrawEvent(int id, int event, SURFHANDLE surf) override;

	virtual void ChangePowerLevel(double newLevel) override;

	virtual double CurrentDraw() override;
	
	// Callback:
	void OnRCSMode(int mode);

private:
    void OnChanged(int mode);

	struct VcData
	{
		int id;
		int mode;
		const UINT group;
		const VECTOR3& loc;
		const NTVERTEX* verts;
	};

	struct PnlData
	{
		int id;
		int mode;
		const UINT group;
		const RECT rc;
		const NTVERTEX* verts;
	};

	std::vector<VcData> vcData_
	{
		{ GetBaseVessel()->GetIdForComponent(this), RCS_LIN, bm::vc::vcRCSLin_id,	bm::vc::vcRCSLin_location,	bm::vc::vcRCSLin_verts },
		{ GetBaseVessel()->GetIdForComponent(this), RCS_ROT, bm::vc::vcRCSRot_id,	bm::vc::vcRCSRot_location,	bm::vc::vcRCSRot_verts }
	};

	std::vector<PnlData> pnlData_
	{
		{ GetBaseVessel()->GetIdForComponent(this), RCS_LIN, bm::pnl::pnlRCSLin_id,	bm::pnl::pnlRCSLin_RC,		bm::pnl::pnlRCSLin_verts },
		{ GetBaseVessel()->GetIdForComponent(this), RCS_ROT, bm::pnl::pnlRCSRot_id,	bm::pnl::pnlRCSRot_RC,		bm::pnl::pnlRCSRot_verts }
	};
};