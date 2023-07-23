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

#include "bc_orbiter/vessel.h"
#include "bc_orbiter/simple_event.h"
#include "bc_orbiter/on_off_display.h"

#include "SR71r_mesh.h"

#include <vector>
#include <map>

namespace bco = bc_orbiter;

/**
Models the nav mode selector function.
*/
class NavModes :
	bco::vessel_component,
	bco::draw_hud
{
public:
	NavModes(bco::vessel& baseVessel);

	// These overrides come directly from SR71Vessel callbacks.
	void OnNavMode(int mode, bool active);
	void handle_draw_hud(bco::vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

	bco::slot<bool>&					IsEnabledSlot()			{ return slotIsEnabled_; }		// Will come from aerodata (avion enabled)

protected:
	void Update();

private:
	bco::vessel&					baseVessel_;
	void ToggleMode(int mode);

	bco::slot<bool>						slotIsEnabled_;

	int		vcUIArea_{ 0 };
	int		panelUIArea_{ 0 };

	int		navMode1_{ 0 };	// HUD nav left area
	int		navMode2_{ 0 };	// HUD nav right area

	// ***  NAV MODES  *** //
	bco::simple_event<int>	btnKillRot_{
		bm::vc::vcNavKillRot_location,
			0.01,
			bm::pnl::pnlNavKillrot_RC,
			NAVMODE_KILLROT
	};

	bco::on_off_display		lightKillRot_{
		bm::vc::vcNavKillRot_id,
			bm::vc::vcNavKillRot_verts,
			bm::pnl::pnlNavKillrot_id,
			bm::pnl::pnlNavKillrot_verts,
			0.0352
	};

	bco::simple_event<int>	btnHorzLevel_{
		bm::vc::vcNavHorzLvl_location,
			0.01,
			bm::pnl::pnlNavHorzLvl_RC,
			NAVMODE_HLEVEL
	};

	bco::on_off_display		lightHorzLevel_{
		bm::vc::vcNavHorzLvl_id,
			bm::vc::vcNavHorzLvl_verts,
			bm::pnl::pnlNavHorzLvl_id,
			bm::pnl::pnlNavHorzLvl_verts,
			0.0352
	};

	bco::simple_event<int>	btnPrograde_{
		bm::vc::vcNavProGrade_location,
			0.01,
			bm::pnl::pnlNavPrograde_RC,
			NAVMODE_PROGRADE
	};

	bco::on_off_display		lightPrograde_{
		bm::vc::vcNavProGrade_id,
			bm::vc::vcNavProGrade_verts,
			bm::pnl::pnlNavPrograde_id,
			bm::pnl::pnlNavPrograde_verts,
			0.0352
	};

	bco::simple_event<int>	btnRetrograde_{
		bm::vc::vcNavRetro_location,
			0.01,
			bm::pnl::pnlNavRetro_RC,
			NAVMODE_RETROGRADE
	};

	bco::on_off_display		lightRetrograde_{
		bm::vc::vcNavRetro_id,
			bm::vc::vcNavRetro_verts,
			bm::pnl::pnlNavRetro_id,
			bm::pnl::pnlNavRetro_verts,
			0.0352
	};

	bco::simple_event<int>	btnNormal_{
		bm::vc::vcNavNorm_location,
			0.01,
			bm::pnl::pnlNavNorm_RC,
			NAVMODE_NORMAL
	};

	bco::on_off_display		lightNormal_{
		bm::vc::vcNavNorm_id,
			bm::vc::vcNavNorm_verts,
			bm::pnl::pnlNavNorm_id,
			bm::pnl::pnlNavNorm_verts,
			0.0352
	};

	bco::simple_event<int>	btnAntiNorm_{
		bm::vc::vcNavAntiNorm_location,
			0.01,
			bm::pnl::pnlNavAntiNorm_RC,
			NAVMODE_ANTINORMAL
	};

	bco::on_off_display		lightAntiNorm_{
		bm::vc::vcNavAntiNorm_id,
			bm::vc::vcNavAntiNorm_verts,
			bm::pnl::pnlNavAntiNorm_id,
			bm::pnl::pnlNavAntiNorm_verts,
			0.0352
	};

};