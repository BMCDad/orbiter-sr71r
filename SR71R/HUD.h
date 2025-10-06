/*
HUD - SR-71r Orbiter Addon
Copyright(C) 2025  Blake Christensen

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Orbitersdk.h"

#include "../bc_orbiter/Vessel.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71r2DMain_mesh.h"
#include "SR71Button.h"
#include "IPowerProvider.h"

namespace bco = bc_orbiter;

/**
    Manage the heads up display (HUD).

    The HUD requires power from the main circuit to operate (see Power System to enable
    main power).  The HUD draw a constant amp level when powered on (one of the HUD modes
    is selected and main power is available).

    Configuration:
    The HUD mode is managed by Orbiter.
*/
class HUD
{
public:
    HUD(bco::Vessel& vessel);
    ~HUD() = default;

    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power);

    void OnHudMode(int mode);
    void LoadVC();
    void LoadState(const std::string& line);
    std::string GetState() const;


private:

    bool    hasPower_{ false };

    void OnChanged(int mode);

    // *** HUD *** 
    SR71::Button btnDocking_{
        bm::vc::vcHUDDock_id,
        bm::vc::vcHUDDock_loc,
        bm::vc::vcHUDDock_vrt,
        bm::pnl::pnlHUDDock_id,
        bm::pnl::pnlHUDDock_vrt,
        bm::pnl::pnlHUDDock_RC,
        SR71R::MainPanel_ID,
        [this](bool) {OnChanged(HUD_DOCKING); }
    };

    SR71::Button btnOrbit_{
        bm::vc::vcHUDOrbit_id,
        bm::vc::vcHUDOrbit_loc,
        bm::vc::vcHUDOrbit_vrt,
        bm::pnl::pnlHUDOrbit_id,
        bm::pnl::pnlHUDOrbit_vrt,
        bm::pnl::pnlHUDOrbit_RC,
        SR71R::MainPanel_ID,
        [this](bool) {OnChanged(HUD_ORBIT); }
    };

    SR71::Button btnSurface_{
        bm::vc::vcHUDSURF_id,
        bm::vc::vcHUDSURF_loc,
        bm::vc::vcHUDSURF_vrt,
        bm::pnl::pnlHUDSurf_id,
        bm::pnl::pnlHUDSurf_vrt,
        bm::pnl::pnlHUDSurf_RC,
        SR71R::MainPanel_ID,
        [this](bool) {OnChanged(HUD_SURFACE); }
    };
};

inline HUD::HUD(bco::Vessel& vessel)
{
    vessel.RegisterUIControl(btnDocking_);
    vessel.RegisterUIControl(btnOrbit_);
    vessel.RegisterUIControl(btnSurface_);
}

inline void HUD::UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power)
{
    hasPower_ = power.GetPowerLevel() > 20.0;

    if (hasPower_) {
        power.DrawPower(4.0);  // Guess at 4 amps for now
    }
    else {
        // No power
        OnChanged(HUD_NONE);
    }
}

inline void HUD::LoadVC()
{
    // Register HUD
    static VCHUDSPEC huds =
    {
       1,                   // Number of mesh groups
       bm::vc::HUD_id,      // mesh group
       { 0.0, 0.8, 15.25 }, // hud center (location)
       0.12                 // hud size
    };

    oapiVCRegisterHUD(&huds);	// HUD parameters
}

inline void HUD::OnChanged(int mode)
{
    auto currentMode = oapiGetHUDMode();
    if ((currentMode == HUD_NONE) && (mode == HUD_NONE)) return;

    auto newMode = ((mode == currentMode) || !hasPower_) ? HUD_NONE : mode;
    
    if (!oapiSetHUDMode(newMode)) {
        OnHudMode(newMode); // We may need to update the buttons ourselves.
    }
}

inline void HUD::OnHudMode(int mode)
{
    btnDocking_.SetState(mode == HUD_DOCKING);
    btnSurface_.SetState(mode == HUD_SURFACE);
    btnOrbit_.SetState(mode == HUD_ORBIT);

    // HUD mode is changing, if it is NOT changing to NONE, and we don't have power, turn it off.
    if (!hasPower_)
    {
        oapiSetHUDMode(HUD_NONE);
    }
}
