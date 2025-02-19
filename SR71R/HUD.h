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
#include "../bc_orbiter/Vessel.h"
#include "../bc_orbiter/control.h"
#include "../bc_orbiter/VesselEvent.h"
#include "../bc_orbiter/VesselTextureElement.h"

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
	main power).  The HUD Draw a constant amp Level when powered on (one of the HUD modes
	is selected and main power is available).

	Configuration:
	The HUD mode is managed by Orbiter.
*/
class HUD : public bco::VesselComponent, public bco::PowerConsumer
{
public:
    HUD(bco::PowerProvider& pwr, bco::Vessel& vessel);

    bool HandleLoadVC(bco::Vessel& vessel, int vcid) override;

    void HandleDrawHud(bco::Vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;

    double AmpDraw() const override { return IsPowered() ? 4.0 : 0.0; }

    void OnHudMode(int mode);

private:
    bco::Vessel& vessel_;
    bco::PowerProvider& power_;

    bool IsPowered() const { return power_.VoltsAvailable() > 24.0; }

    void OnChanged(int mode);

    // *** HUD *** 
    bco::VesselEvent     btnDocking_{
        bm::vc::vcHUDDock_loc,
        0.01,
        cmn::vc::main,
        bm::pnl::pnlHUDDock_RC,
        cmn::panel::main
    };

    bco::VesselTextureElement       btnLightDocking_{
        bm::vc::vcHUDDock_id,
        bm::vc::vcHUDDock_vrt,
        cmn::vc::main,
        bm::pnl::pnlHUDDock_id,
        bm::pnl::pnlHUDDock_vrt,
        cmn::panel::main
    };

    bco::VesselEvent     btnOrbit_{
        bm::vc::vcHUDOrbit_loc,
        0.01,
        cmn::vc::main,
        bm::pnl::pnlHUDOrbit_RC,
        cmn::panel::main
    };

    bco::VesselTextureElement     btnLightOrbit_ {
        bm::vc::vcHUDOrbit_id,
        bm::vc::vcHUDOrbit_vrt,
        cmn::vc::main,
        bm::pnl::pnlHUDOrbit_id,
        bm::pnl::pnlHUDOrbit_vrt,
        cmn::panel::main
    };

    bco::VesselEvent     btnSurface_{
        bm::vc::vcHUDSURF_loc,
        0.01,
        cmn::vc::main,
        bm::pnl::pnlHUDSurf_RC,
        cmn::panel::main
    };

    bco::VesselTextureElement      btnLightSurface_ {
        bm::vc::vcHUDSURF_id,
        bm::vc::vcHUDSURF_vrt,
        cmn::vc::main,
        bm::pnl::pnlHUDSurf_id,
        bm::pnl::pnlHUDSurf_vrt,
        cmn::panel::main
    };
};

inline HUD::HUD(bco::PowerProvider& pwr, bco::Vessel& vessel) :
    power_(pwr),
    vessel_(vessel)
{
    power_.AttachConsumer(this),
    vessel.AddControl(&btnLightDocking_);
    vessel.AddControl(&btnLightSurface_);
    vessel.AddControl(&btnLightOrbit_);

    vessel.AddControl(&btnDocking_);
    vessel.AddControl(&btnSurface_);
    vessel.AddControl(&btnOrbit_);

    btnDocking_.Attach([&](VESSEL4&) { OnChanged(HUD_DOCKING); });
    btnOrbit_.Attach([&](VESSEL4&) { OnChanged(HUD_ORBIT); });
    btnSurface_.Attach([&](VESSEL4&) { OnChanged(HUD_SURFACE); });
}

inline bool HUD::HandleLoadVC(bco::Vessel& vessel, int vcid)
{
    // Register HUD
    static VCHUDSPEC huds = {
       1,                   // Mesh number (VC)
       bm::vc::HUD_id,      // mesh group
       { 0.0, 0.8, 15.25 }, // hud center (location)
       0.12                 // hud size
    };

    oapiVCRegisterHUD(&huds);	// HUD parameters
    return true;
}

inline void HUD::OnHudMode(int mode)
{
    btnLightDocking_.set_state(vessel_, mode == HUD_DOCKING);
    btnLightSurface_.set_state(vessel_, mode == HUD_SURFACE);
    btnLightOrbit_.set_state(vessel_, mode == HUD_ORBIT);

    // HUD mode is changing, if it is NOT changing to NONE, and we don't have power, turn it off.
    if (!IsPowered()) {
        oapiSetHUDMode(HUD_NONE);
    }
}

inline void HUD::OnChanged(int mode)
{
    auto currentMode = oapiGetHUDMode();
    auto newMode = ((mode == currentMode) || !IsPowered()) ? HUD_NONE : mode;
    oapiSetHUDMode(newMode);;
}

inline void HUD::HandleDrawHud(bco::Vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return;

    auto am = vessel.GetAttitudeMode();

    if (am != RCS_NONE) {
        int xLeft = 200;
        int yTop = hps->H - 30;

        skp->Rectangle(
            xLeft,
            yTop,
            xLeft + 45,
            yTop + 25);

        skp->Text(xLeft + 3, yTop + 1, "RCS", 3);

        // show RCS mode
        switch (am) {
        case RCS_ROT:
            skp->Text(xLeft + 50, yTop, "ROT", 3);
            break;
        case RCS_LIN:
            skp->Text(xLeft + 50, yTop, "LIN", 3);
            break;
        }
    }
}
