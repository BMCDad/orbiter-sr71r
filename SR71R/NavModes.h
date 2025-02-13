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

#include "../bc_orbiter/vessel.h"
#include "../bc_orbiter/simple_event.h"
#include "../bc_orbiter/on_off_display.h"
#include "../bc_orbiter/panel_display.h"

#include "Avionics.h"
#include "SR71r_mesh.h"

#include <vector>
#include <map>

namespace bco = bc_orbiter;

/**
Models the nav mode selector function.
*/
class NavModes :
	  public bco::vessel_component
	, public bco::draw_hud
	, public bco::post_step
{
public:
	NavModes(bco::vessel& baseVessel, Avionics& avionics);

	// These overrides come directly from SR71Vessel callbacks.
	void OnNavMode(int mode, bool active);
	void handle_draw_hud(bco::vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

	void handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd) override {
		if (oapiCockpitMode() != COCKPIT_PANELS) return;
		UpdatePanel(); 
	}

protected:
	void Update();

	void UpdatePanel() { 
		pnlHudFrame_.set_position(((navMode1_ + navMode2_) == 0) ? 1 : 0); 
		pnlHudMode_.set_position(navMode1_);
		pnlHudMode2_.set_position(navMode2_);
	}
private:
	bco::vessel&	baseVessel_;
	Avionics&		avionics_;

	void ToggleMode(int mode);

	int		vcUIArea_{ 0 };
	int		panelUIArea_{ 0 };
	int		navMode1_{ 0 };	// HUD nav left area
	int		navMode2_{ 0 };	// HUD nav right area

	// ***  NAV MODES  *** //
	using evt = bco::simple_event<int>;
	using dsp = bco::on_off_display;
	const double rad = 0.01;		// hit radius
	const double ofs = 0.0352;		// tex offset

	evt	btnKillRot_     { bm::vc::vcNavKillRot_loc,     rad, 0, bm::pnl::pnlNavKillrot_RC,     0,  NAVMODE_KILLROT     };
	evt	btnHorzLevel_   { bm::vc::vcNavHorzLvl_loc,     rad, 0, bm::pnl::pnlNavHorzLvl_RC,     0,  NAVMODE_HLEVEL      };
	evt	btnPrograde_    { bm::vc::vcNavProGrade_loc,    rad, 0, bm::pnl::pnlNavPrograde_RC,    0,  NAVMODE_PROGRADE    };
	evt	btnRetrograde_  { bm::vc::vcNavRetro_loc,       rad, 0, bm::pnl::pnlNavRetro_RC,       0,  NAVMODE_RETROGRADE  };
	evt	btnNormal_      { bm::vc::vcNavNorm_loc,        rad, 0, bm::pnl::pnlNavNorm_RC,        0,  NAVMODE_NORMAL      };
	evt	btnAntiNorm_    { bm::vc::vcNavAntiNorm_loc,    rad, 0, bm::pnl::pnlNavAntiNorm_RC,    0,  NAVMODE_ANTINORMAL  };

	dsp	lightKillRot_	{ bm::vc::vcNavKillRot_id,	bm::vc::vcNavKillRot_vrt,	bm::pnl::pnlNavKillrot_id,	bm::pnl::pnlNavKillrot_vrt,		ofs	};
	dsp	lightHorzLevel_	{ bm::vc::vcNavHorzLvl_id,	bm::vc::vcNavHorzLvl_vrt,	bm::pnl::pnlNavHorzLvl_id,	bm::pnl::pnlNavHorzLvl_vrt,		ofs	};
	dsp	lightPrograde_	{ bm::vc::vcNavProGrade_id,	bm::vc::vcNavProGrade_vrt,	bm::pnl::pnlNavPrograde_id,	bm::pnl::pnlNavPrograde_vrt,	ofs	};
	dsp	lightRetro_		{ bm::vc::vcNavRetro_id,	bm::vc::vcNavRetro_vrt,		bm::pnl::pnlNavRetro_id,	bm::pnl::pnlNavRetro_vrt,		ofs	};
	dsp	lightNormal_	{ bm::vc::vcNavNorm_id,		bm::vc::vcNavNorm_vrt,		bm::pnl::pnlNavNorm_id,		bm::pnl::pnlNavNorm_vrt,		ofs	};
	dsp	lightAntiNorm_	{ bm::vc::vcNavAntiNorm_id,	bm::vc::vcNavAntiNorm_vrt,	bm::pnl::pnlNavAntiNorm_id,	bm::pnl::pnlNavAntiNorm_vrt,	ofs	};

	// 2D panel mini-hud
	bco::panel_display	pnlHudFrame_	{ bm::pnl::pnlHUDNavTile_id,	bm::pnl::pnlHUDNavTile_vrt,		0.0610 };
	bco::panel_display	pnlHudMode_		{ bm::pnl::pnlHUDNavText_id,	bm::pnl::pnlHUDNavText_vrt,		0.0305 };
	bco::panel_display	pnlHudMode2_	{ bm::pnl::pnlHUDNavText2_id,	bm::pnl::pnlHUDNavText2_vrt,	0.0305 };
};