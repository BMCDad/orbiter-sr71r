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

#include "../bc_orbiter/PanelTextureElement.h"
#include "../bc_orbiter/Vessel.h"
#include "../bc_orbiter/VesselEvent.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "Avionics.h"
#include "SR71r_mesh.h"
#include "Common.h"

#include <vector>
#include <map>

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

/**
Models the nav mode selector function.
*/
class NavModes : public bco::VesselComponent
{
public:
    NavModes(bco::Vessel& baseVessel, Avionics& avionics);

    // These overrides come directly from SR71Vessel callbacks.
    void OnNavMode(int mode, bool active);
    void HandleDrawHud(bco::Vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override {
        if (oapiCockpitMode() != COCKPIT_PANELS) return;
        UpdatePanel();
    }

protected:
    void Update();

    void UpdatePanel() {
        pnlHudFrame_.setState(vessel_, ((navMode1_ + navMode2_) == 0) ? 1 : 0);
        pnlHudMode_.setState(vessel_, navMode1_);
        pnlHudMode2_.setState(vessel_, navMode2_);
    }
private:
    bco::Vessel& vessel_;
    Avionics& avionics_;

    void ToggleMode(int mode);

    int		vcUIArea_{ 0 };
    int		panelUIArea_{ 0 };
    int		navMode1_{ 0 };	// HUD nav left area
    int		navMode2_{ 0 };	// HUD nav right area

    // ***  NAV MODES  *** //
//    using evt = bco::SimpleEvent<int>;
    using evt = bco::VesselEvent;
    using dsp = bco::VesselTextureElement;
    const double rad = 0.01;		// hit radius
    const double ofs = 0.0352;		// tex offset

    evt btnKillRot_{    bm::vc::vcNavKillRot_loc,     rad, 0, bm::pnl::pnlNavKillrot_RC,     0 };
    evt btnHorzLevel_{  bm::vc::vcNavHorzLvl_loc,     rad, 0, bm::pnl::pnlNavHorzLvl_RC,     0 };
    evt btnPrograde_{   bm::vc::vcNavProGrade_loc,    rad, 0, bm::pnl::pnlNavPrograde_RC,    0 };
    evt btnRetrograde_{ bm::vc::vcNavRetro_loc,       rad, 0, bm::pnl::pnlNavRetro_RC,       0 };
    evt btnNormal_{     bm::vc::vcNavNorm_loc,        rad, 0, bm::pnl::pnlNavNorm_RC,        0 };
    evt btnAntiNorm_{   bm::vc::vcNavAntiNorm_loc,    rad, 0, bm::pnl::pnlNavAntiNorm_RC,    0 };

    dsp lightKillRot_{      bm::vc::vcNavKillRot_id,    bm::vc::vcNavKillRot_vrt,   cmn::vc::main, bm::pnl::pnlNavKillrot_id,   bm::pnl::pnlNavKillrot_vrt,  cmn::panel::main };
    dsp lightHorzLevel_{    bm::vc::vcNavHorzLvl_id,    bm::vc::vcNavHorzLvl_vrt,   cmn::vc::main, bm::pnl::pnlNavHorzLvl_id,   bm::pnl::pnlNavHorzLvl_vrt,  cmn::panel::main };
    dsp lightPrograde_{     bm::vc::vcNavProGrade_id,   bm::vc::vcNavProGrade_vrt,  cmn::vc::main, bm::pnl::pnlNavPrograde_id,  bm::pnl::pnlNavPrograde_vrt, cmn::panel::main };
    dsp lightRetro_{        bm::vc::vcNavRetro_id,      bm::vc::vcNavRetro_vrt,     cmn::vc::main, bm::pnl::pnlNavRetro_id,     bm::pnl::pnlNavRetro_vrt,    cmn::panel::main };
    dsp lightNormal_{       bm::vc::vcNavNorm_id,       bm::vc::vcNavNorm_vrt,      cmn::vc::main, bm::pnl::pnlNavNorm_id,      bm::pnl::pnlNavNorm_vrt,     cmn::panel::main };
    dsp lightAntiNorm_{     bm::vc::vcNavAntiNorm_id,   bm::vc::vcNavAntiNorm_vrt,  cmn::vc::main, bm::pnl::pnlNavAntiNorm_id,  bm::pnl::pnlNavAntiNorm_vrt, cmn::panel::main };

    // 2D panel mini-hud
    bco::PanelTextureControl  pnlHudFrame_ {
        bm::pnl::pnlHUDNavTile_id,
        bm::pnl::pnlHUDNavTile_vrt,
        cmn::panel::main 
    };

    bco::PanelTextureControl  pnlHudMode_ {
        bm::pnl::pnlHUDNavText_id,
        bm::pnl::pnlHUDNavText_vrt,
        cmn::panel::main
    };

    bco::PanelTextureControl  pnlHudMode2_ {
        bm::pnl::pnlHUDNavText2_id,
        bm::pnl::pnlHUDNavText2_vrt,
        cmn::panel::main
    };
};

inline NavModes::NavModes(bco::Vessel& baseVessel, Avionics& avionics) :
    vessel_(baseVessel)
    , avionics_(avionics)
{
    vessel_.AddControl(&btnKillRot_);
    vessel_.AddControl(&btnHorzLevel_);
    vessel_.AddControl(&btnAntiNorm_);
    vessel_.AddControl(&btnNormal_);
    vessel_.AddControl(&btnPrograde_);
    vessel_.AddControl(&btnRetrograde_);

    vessel_.AddControl(&lightKillRot_);
    vessel_.AddControl(&lightHorzLevel_);
    vessel_.AddControl(&lightAntiNorm_);
    vessel_.AddControl(&lightNormal_);
    vessel_.AddControl(&lightPrograde_);
    vessel_.AddControl(&lightRetro_);

    vessel_.AddControl(&pnlHudFrame_);
    vessel_.AddControl(&pnlHudMode_);
    vessel_.AddControl(&pnlHudMode2_);

    btnKillRot_.Attach(     [&](VESSEL4&) { ToggleMode(NAVMODE_KILLROT); });
    btnHorzLevel_.Attach(   [&](VESSEL4&) { ToggleMode(NAVMODE_HLEVEL); });
    btnAntiNorm_.Attach(    [&](VESSEL4&) { ToggleMode(NAVMODE_ANTINORMAL); });
    btnNormal_.Attach(      [&](VESSEL4&) { ToggleMode(NAVMODE_NORMAL); });
    btnPrograde_.Attach(    [&](VESSEL4&) { ToggleMode(NAVMODE_PROGRADE); });
    btnRetrograde_.Attach(  [&](VESSEL4&) { ToggleMode(NAVMODE_RETROGRADE); });
}

inline void NavModes::OnNavMode(int mode, bool active)
{
    switch (mode) {
    case NAVMODE_ANTINORMAL:
        lightAntiNorm_.SetState(vessel_, active);
        navMode1_ = active ? NAVMODE_ANTINORMAL : 0;
        break;
    case NAVMODE_HLEVEL:
        lightHorzLevel_.SetState(vessel_, active);
        navMode2_ = active ? NAVMODE_HLEVEL : 0;
        break;
    case NAVMODE_KILLROT:
        lightKillRot_.SetState(vessel_, active);
        navMode1_ = active ? NAVMODE_KILLROT : 0;
        break;
    case NAVMODE_NORMAL:
        lightNormal_.SetState(vessel_, active);
        navMode1_ = active ? NAVMODE_NORMAL : 0;
        break;
    case NAVMODE_PROGRADE:
        lightPrograde_.SetState(vessel_, active);
        navMode1_ = active ? NAVMODE_PROGRADE : 0;
        break;
    case NAVMODE_RETROGRADE:
        lightRetro_.SetState(vessel_, active);
        navMode1_ = active ? NAVMODE_RETROGRADE : 0;
        break;
    }

    UpdatePanel();
}

inline void NavModes::ToggleMode(int mode)
{
    if (avionics_.IsAeroActive())
    {
        vessel_.ToggleNavmode(mode);
    }

    Update();
}

inline void NavModes::Update()
{
    if (!avionics_.IsAeroActive())
    {
        // Shut down all modes:
        vessel_.DeactivateNavmode(NAVMODE_KILLROT);
        vessel_.DeactivateNavmode(NAVMODE_HLEVEL);
        vessel_.DeactivateNavmode(NAVMODE_PROGRADE);
        vessel_.DeactivateNavmode(NAVMODE_RETROGRADE);
        vessel_.DeactivateNavmode(NAVMODE_NORMAL);
        vessel_.DeactivateNavmode(NAVMODE_ANTINORMAL);
    }
}

inline void NavModes::HandleDrawHud(bco::Vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return;
    int xLeft = 2;
    int yTop = hps->H - 30;

    if ((navMode1_ != 0) || (navMode2_ != 0))
    {
        skp->Rectangle(
            xLeft,
            yTop,
            xLeft + 45,
            yTop + 25);

        skp->Text(xLeft + 3, yTop + 1, "NAV", 3);

        char* lbl1 = nullptr;

        switch (navMode1_)
        {
        case NAVMODE_ANTINORMAL:
            lbl1 = "ANRM";
            break;

        case NAVMODE_NORMAL:
            lbl1 = "NRM ";
            break;

        case NAVMODE_PROGRADE:
            lbl1 = "PROG";
            break;

        case NAVMODE_RETROGRADE:
            lbl1 = "RTRO";
            break;

        case NAVMODE_KILLROT:
            lbl1 = "KLRT";
            break;

        }

        if (lbl1) skp->Text(xLeft + 50, yTop + 1, lbl1, 4);

        skp->Line(xLeft + 100, yTop, xLeft + 100, yTop + 25);

        if (navMode2_ == NAVMODE_HLEVEL) skp->Text(xLeft + 105, yTop + 1, "HLVL", 4);
    }
}