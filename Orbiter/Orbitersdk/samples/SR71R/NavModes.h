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

	virtual void SetClassCaps() override;

	virtual bool LoadVC(int id) override;
	virtual bool VCRedrawEvent(int id, int event, SURFHANDLE surf) override;

	/**
		Respond to vessel clbkNavMode call by updating the nav mode status ports.
	*/
	void OnNavMode(int mode, bool active);

    bool DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);
protected:
	void Update();

private:
	void ToggleMode(int mode);

	int						uiArea_;

    int                     navMode1_{ 0 };
    int                     navMode2_{ 0 };

    bco::PushButtonSwitch	btnKillRotation_    { bt_mesh::SR71rVC::NAVMODE_KILL_ROT_location,       0.01,  GetBaseVessel() };
    bco::PushButtonSwitch	btnLevelHorizon_    { bt_mesh::SR71rVC::NAVMODE_HORZ_LEVEL_location,     0.01,  GetBaseVessel() };
    bco::PushButtonSwitch	btnPrograde_        { bt_mesh::SR71rVC::NAVMODE_PRO_GRADE_location,      0.01,  GetBaseVessel() };
    bco::PushButtonSwitch	btnRetrograde_      { bt_mesh::SR71rVC::NAVMODE_RETRO_GRADE_location,    0.01,  GetBaseVessel() };
    bco::PushButtonSwitch	btnNormal_          { bt_mesh::SR71rVC::NAVMODE_NORMAL_PLUS_location,    0.01,  GetBaseVessel() };
    bco::PushButtonSwitch	btnAntiNormal_      { bt_mesh::SR71rVC::NAVMODE_NORMAL_MINUS_location,   0.01,  GetBaseVessel() };

	bco::TextureVisual		visKillRot_;
	bco::TextureVisual		visHorzLevel_;
	bco::TextureVisual		visProGrade_;
	bco::TextureVisual		visRetroGrade_;
	bco::TextureVisual		visNormal_;
	bco::TextureVisual		visAntiNormal_;
};