//	NavModes - SR-71r Orbiter Addon
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

#pragma

#include "bc_orbiter\bco.h"
#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\PushButtonSwitch.h"
#include "bc_orbiter\TextureVisual.h"
#include "bc_orbiter\PanelButton.h"
#include "bc_orbiter\PanelTextureVisual.h"

#include "SR71r_mesh.h"

#include <vector>
#include <map>

namespace bco = bc_orbiter;

/**
Models the nav mode selector function.
*/
class NavModes : public bco::PoweredComponent
{
public:
	NavModes(bco::BaseVessel* vessel, double amps);

	// Component overrides:
	bool OnLoadPanel2D(int id, PANELHANDLE hPanel) override;
	bool OnLoadVC(int id) override;
	bool OnPanelRedrawEvent(int id, int event, SURFHANDLE surf) override;
	bool OnPanelMouseEvent(int id, int event) override;
	void OnSetClassCaps() override;
	bool OnVCMouseEvent(int id, int event) override;
	bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf) override;

	// These overrides come directly from SR71Vessel callbacks.
	void OnNavMode(int mode, bool active);
	bool DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

protected:
	void Update();

private:
	void ToggleMode(int mode);

	int						vcUIArea_{ 0 };
	int						panelUIArea_{ 0 };

	int                     navMode1_{ 0 };	// HUD nav left area
	int                     navMode2_{ 0 };	// HUD nav right area

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

	std::vector<PnlData> pnlData_
	{
		{ GetBaseVessel()->GetIdForComponent(this), NAVMODE_ANTINORMAL,	bm::pnl::pnlNavAntiNorm_id,	bm::pnl::pnlNavAntiNorm_RC,		bm::pnl::pnlNavAntiNorm_verts },
		{ GetBaseVessel()->GetIdForComponent(this), NAVMODE_HLEVEL,		bm::pnl::pnlNavHorzLvl_id,	bm::pnl::pnlNavHorzLvl_RC,		bm::pnl::pnlNavHorzLvl_verts },
		{ GetBaseVessel()->GetIdForComponent(this), NAVMODE_KILLROT,	bm::pnl::pnlNavKillrot_id,	bm::pnl::pnlNavKillrot_RC,		bm::pnl::pnlNavKillrot_verts },
		{ GetBaseVessel()->GetIdForComponent(this), NAVMODE_NORMAL,		bm::pnl::pnlNavNorm_id,		bm::pnl::pnlNavNorm_RC,			bm::pnl::pnlNavNorm_verts },
		{ GetBaseVessel()->GetIdForComponent(this), NAVMODE_PROGRADE,	bm::pnl::pnlNavPrograde_id,	bm::pnl::pnlNavPrograde_RC,		bm::pnl::pnlNavPrograde_verts },
		{ GetBaseVessel()->GetIdForComponent(this), NAVMODE_RETROGRADE,	bm::pnl::pnlNavRetro_id,	bm::pnl::pnlNavRetro_RC,		bm::pnl::pnlNavRetro_verts }
	};

	std::vector<VcData> vcData_
	{
		{ GetBaseVessel()->GetIdForComponent(this), NAVMODE_ANTINORMAL,	bm::vc::vcNavAntiNorm_id,	bm::vc::vcNavAntiNorm_location,	bm::vc::vcNavAntiNorm_verts },
		{ GetBaseVessel()->GetIdForComponent(this), NAVMODE_HLEVEL,		bm::vc::vcNavHorzLvl_id,	bm::vc::vcNavHorzLvl_location,	bm::vc::vcNavHorzLvl_verts },
		{ GetBaseVessel()->GetIdForComponent(this), NAVMODE_KILLROT,	bm::vc::vcNavKillRot_id,	bm::vc::vcNavKillRot_location,	bm::vc::vcNavKillRot_verts },
		{ GetBaseVessel()->GetIdForComponent(this), NAVMODE_NORMAL,		bm::vc::vcNavNorm_id,		bm::vc::vcNavNorm_location,		bm::vc::vcNavNorm_verts	},
		{ GetBaseVessel()->GetIdForComponent(this), NAVMODE_PROGRADE,	bm::vc::vcNavProGrade_id,	bm::vc::vcNavProGrade_location,	bm::vc::vcNavProGrade_verts },
		{ GetBaseVessel()->GetIdForComponent(this), NAVMODE_RETROGRADE,	bm::vc::vcNavRetro_id,		bm::vc::vcNavRetro_location,	bm::vc::vcNavRetro_verts }
	};
};