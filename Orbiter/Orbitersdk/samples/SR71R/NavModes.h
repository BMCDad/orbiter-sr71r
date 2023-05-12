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
	bool OnPanelRedrawEvent(int id, int event) override;
	void OnSetClassCaps() override;
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

	bco::PushButtonSwitch	btnKillRotation_	{ bt_mesh::SR71rVC::NAVMODE_KILL_ROT_location,       0.01, [this] { ToggleMode(NAVMODE_KILLROT); } };
	bco::PushButtonSwitch	btnLevelHorizon_	{ bt_mesh::SR71rVC::NAVMODE_HORZ_LEVEL_location,     0.01, [this] { ToggleMode(NAVMODE_HLEVEL); } };
	bco::PushButtonSwitch	btnPrograde_		{ bt_mesh::SR71rVC::NAVMODE_PRO_GRADE_location,      0.01, [this] { ToggleMode(NAVMODE_PROGRADE); } };
	bco::PushButtonSwitch	btnRetrograde_		{ bt_mesh::SR71rVC::NAVMODE_RETRO_GRADE_location,    0.01, [this] { ToggleMode(NAVMODE_RETROGRADE); } };
	bco::PushButtonSwitch	btnNormal_			{ bt_mesh::SR71rVC::NAVMODE_NORMAL_PLUS_location,    0.01, [this] { ToggleMode(NAVMODE_NORMAL); } };
	bco::PushButtonSwitch	btnAntiNormal_		{ bt_mesh::SR71rVC::NAVMODE_NORMAL_MINUS_location,   0.01, [this] { ToggleMode(NAVMODE_ANTINORMAL); } };

	bco::TextureVisual		visKillRot_		{ bt_mesh::SR71rVC::NAVMODE_KILL_ROT_verts,		bt_mesh::SR71rVC::NAVMODE_KILL_ROT_id };
	bco::TextureVisual		visHorzLevel_	{ bt_mesh::SR71rVC::NAVMODE_HORZ_LEVEL_verts,	bt_mesh::SR71rVC::NAVMODE_HORZ_LEVEL_id };
	bco::TextureVisual		visProGrade_	{ bt_mesh::SR71rVC::NAVMODE_PRO_GRADE_verts,	bt_mesh::SR71rVC::NAVMODE_PRO_GRADE_id };
	bco::TextureVisual		visRetroGrade_	{ bt_mesh::SR71rVC::NAVMODE_RETRO_GRADE_verts,	bt_mesh::SR71rVC::NAVMODE_RETRO_GRADE_id };
	bco::TextureVisual		visNormal_		{ bt_mesh::SR71rVC::NAVMODE_NORMAL_PLUS_verts,	bt_mesh::SR71rVC::NAVMODE_NORMAL_PLUS_id };
	bco::TextureVisual		visAntiNormal_	{ bt_mesh::SR71rVC::NAVMODE_NORMAL_MINUS_verts,	bt_mesh::SR71rVC::NAVMODE_NORMAL_MINUS_id };

	bco::PanelPushButtonSwitch pnlBtnKillRotation_	{ bt_mesh::SR71r2D::APKillRot_RC,		[this] { ToggleMode(NAVMODE_KILLROT); } };
	bco::PanelPushButtonSwitch pnlBtnHorzLevel_		{ bt_mesh::SR71r2D::APHorzLevel_RC,		[this] { ToggleMode(NAVMODE_HLEVEL); } };
	bco::PanelPushButtonSwitch pnlBtnPrograde_		{ bt_mesh::SR71r2D::APPrograde_RC,		[this] { ToggleMode(NAVMODE_PROGRADE); } };
	bco::PanelPushButtonSwitch pnlBtnRetrograde_	{ bt_mesh::SR71r2D::APRetro_RC,			[this] { ToggleMode(NAVMODE_RETROGRADE); } };
	bco::PanelPushButtonSwitch pnlBtnNormal_		{ bt_mesh::SR71r2D::APNormal_RC,		[this] { ToggleMode(NAVMODE_NORMAL); } };
	bco::PanelPushButtonSwitch pnlBtnAntiNormal_	{ bt_mesh::SR71r2D::APAntiNormal_RC,	[this] { ToggleMode(NAVMODE_ANTINORMAL); } };

	bco::PanelTextureVisual	pnlKillRot_		{ bt_mesh::SR71r2D::APKillRot_verts,	bt_mesh::SR71r2D::APKillRot_id };
	bco::PanelTextureVisual	pnlHorzLevel_	{ bt_mesh::SR71r2D::APHorzLevel_verts,	bt_mesh::SR71r2D::APHorzLevel_id };
	bco::PanelTextureVisual	pnlProGrade_	{ bt_mesh::SR71r2D::APPrograde_verts,	bt_mesh::SR71r2D::APPrograde_id };
	bco::PanelTextureVisual	pnlRetroGrade_	{ bt_mesh::SR71r2D::APRetro_verts,		bt_mesh::SR71r2D::APRetro_id };
	bco::PanelTextureVisual	pnlNormal_		{ bt_mesh::SR71r2D::APNormal_verts,		bt_mesh::SR71r2D::APNormal_id };
	bco::PanelTextureVisual	pnlAntiNormal_	{ bt_mesh::SR71r2D::APAntiNormal_verts,	bt_mesh::SR71r2D::APAntiNormal_id };
};