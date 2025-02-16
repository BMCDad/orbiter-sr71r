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

#include "../bc_orbiter/Animation.h"
#include "../bc_orbiter/vessel.h"
#include "../bc_orbiter/control.h"
#include "../bc_orbiter/simple_event.h"
#include "../bc_orbiter/display_full.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"
#include "SR71rPanelRight_mesh.h"

#include "Common.h"


namespace bco = bc_orbiter;
namespace cmn = sr71_common;

/**
	Manage the heads up display (HUD).

	The HUD requires power from the main circuit to operate (see Power System to enable
	main power).  The HUD draw a constant amp level when powered on (one of the HUD modes
	is selected and main power is available).

	Configuration:
	The HUD mode is managed by Orbiter.
*/
class HUD :
    public bco::vessel_component,
    public bco::power_consumer,
    public bco::load_vc,
    public bco::draw_hud
{
public:
    HUD(bco::power_provider& pwr, bco::vessel& vessel);

    bool handle_load_vc(bco::vessel& vessel, int vcid) override;

    void handle_draw_hud(bco::vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;

    double amp_draw() const override { return IsPowered() ? 4.0 : 0.0; }

    void OnHudMode(int mode);

private:
    bco::vessel& vessel_;
    bco::power_provider& power_;

    bool IsPowered() const { return power_.volts_available() > 24.0; }

    void OnChanged(int mode);

    // *** HUD *** 
    bco::simple_event<>     btnDocking_{
        bm::vc::vcHUDDock_loc,
        0.01,
        0,
        bm::pnl::pnlHUDDock_RC,
        0
    };

    bco::display_full       btnLightDocking_{
        bm::vc::vcHUDDock_id,
        bm::vc::vcHUDDock_vrt,
        cmn::vc::main,
        bm::pnl::pnlHUDDock_id,
        bm::pnl::pnlHUDDock_vrt,
        cmn::panel::main
    };

    bco::simple_event<>     btnOrbit_{
        bm::vc::vcHUDOrbit_loc,
        0.01,
        0,
        bm::pnl::pnlHUDOrbit_RC,
        0
    };

    bco::display_full     btnLightOrbit_ {
        bm::vc::vcHUDOrbit_id,
        bm::vc::vcHUDOrbit_vrt,
        cmn::vc::main,
        bm::pnl::pnlHUDOrbit_id,
        bm::pnl::pnlHUDOrbit_vrt,
        cmn::panel::main
    };

    bco::simple_event<>     btnSurface_ {
        bm::vc::vcHUDSURF_loc,
        0.01,
        0,
        bm::pnl::pnlHUDSurf_RC,
        0
    };

    bco::display_full      btnLightSurface_ {
        bm::vc::vcHUDSURF_id,
        bm::vc::vcHUDSURF_vrt,
        cmn::vc::main,
        bm::pnl::pnlHUDSurf_id,
        bm::pnl::pnlHUDSurf_vrt,
        cmn::panel::main
    };
};
