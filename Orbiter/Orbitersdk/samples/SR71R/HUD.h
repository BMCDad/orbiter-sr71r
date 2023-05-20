//	HUD - SR-71r Orbiter Addon
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

#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\TextureVisual.h"
#include "bc_orbiter\PushButtonSwitch.h"
#include "bc_orbiter\VCRotorSwitch.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

/**
	Manage the heads up display (HUD).

	The HUD requires power from the main circuit to operate (see Power System to enable
	main power).  The HUD draw a constant amp level when powered on (one of the HUD modes
	is selected and main power is available).

	Configuration:
	The HUD mode is managed by Orbiter.
*/
class HUD : public bco::PoweredComponent
{
public:
	HUD(bco::BaseVessel* vessel, double amps);

	bool OnLoadVC(int id) override;
	bool OnVCMouseEvent(int id, int event) override;
	bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf) override;

	bool OnLoadPanel2D(int id, PANELHANDLE hPanel) override;
	bool OnPanelMouseEvent(int id, int event) override;
	bool OnPanelRedrawEvent(int id, int event, SURFHANDLE surf) override;


	/**
	Override to manage power based on vessel HUD state.
	*/
	virtual double CurrentDraw() override;

	void ChangePowerLevel(double newLevel) override;


	bool DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

	void OnHudMode(int mode);

private:
	void OnChanged(int mode);

	struct PnlData
	{
		int id;
		int mode;
		const UINT group;
		const RECT rc;
		const NTVERTEX* verts;
	};

	struct VcData
	{
		int id;
		int mode;
		const UINT group;
		const VECTOR3& loc;
		const NTVERTEX* verts;
	};

	std::vector<PnlData> pnlData_
	{
		{ GetBaseVessel()->GetIdForComponent(this), HUD_DOCKING, bm::pnl::pnlHUDDock_id,	bm::pnl::pnlHUDDock_RC,		bm::pnl::pnlHUDDock_verts },
		{ GetBaseVessel()->GetIdForComponent(this), HUD_SURFACE, bm::pnl::pnlHUDSurf_id,	bm::pnl::pnlHUDSurf_RC,		bm::pnl::pnlHUDSurf_verts },
		{ GetBaseVessel()->GetIdForComponent(this), HUD_ORBIT,	 bm::pnl::pnlHUDOrbit_id,	bm::pnl::pnlHUDOrbit_RC,	bm::pnl::pnlHUDOrbit_verts }
	};

	std::vector<VcData> vcData_
	{
		{ GetBaseVessel()->GetIdForComponent(this), HUD_DOCKING, bm::vc::vcHUDDock_id,	bm::vc::vcHUDDock_location,	 bm::vc::vcHUDDock_verts },
		{ GetBaseVessel()->GetIdForComponent(this), HUD_SURFACE, bm::vc::vcHUDSURF_id,	bm::vc::vcHUDSURF_location,	 bm::vc::vcHUDSURF_verts },
		{ GetBaseVessel()->GetIdForComponent(this), HUD_ORBIT,	 bm::vc::vcHUDOrbit_id,	bm::vc::vcHUDOrbit_location, bm::vc::vcHUDOrbit_verts }
	};
};
