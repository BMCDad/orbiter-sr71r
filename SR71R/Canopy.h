//	Canopy - SR-71r Orbiter Addon
//	Copyright(C) 2025  Blake Christensen
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

#include "OrbiterSDK.h"

#include "../bc_orbiter/Animation.h"
#include "../bc_orbiter/control.h"
#include "../bc_orbiter/Vessel.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "Toggle.h"
#include "IndicatorLight.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"
#include "SR71rPanelRight_mesh.h"

#include "ShipMets.h"
#include "common.h"

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

/**	Canopy
Controls the canopy door.
The canopy runs on the main electrical circuit and can have a high Draw
when in motion.

To operate the canopy:
- Power the main circuit (external or fuel cell).
- Turn 'Main' power on (up).
- Turn 'Canopy' (CNPY) power switch (right panel) on (up).
- To Open switch 'Canopy' switch (left panel) to OPEN.
- The message board [CNPY] light will show orange while the canopy is
in motion, and white when fully open.

Configuration:

CANOPY a b c
a - 0/1 Power switch off/on.
b - 0/1 Open close switch closed/open.
c - 0.0-1.0 current canopy position.

: rewrite :
related
OnOffInput (canopy power):
: signal canopy slot: has_power
OnOffInput (canopy open):
: signal to canopy slot: open/close

- inputs:
:slot - has power
*/
class Canopy : public bco::VesselComponent, public bco::PowerConsumer
{
public:
    Canopy(bco::PowerProvider& pwr, bco::Vessel& vessel);

    // SetClassCaps
    void HandleSetClassCaps(bco::Vessel& vessel) override;

    // PowerConsumer
    double AmpDraw() const override { return CanopyIsMoving() ? 4.0 : 0.0; }

    // PostStep
    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override;

    // ManageState
    bool HandleLoadState(bco::Vessel& vessel, const std::string& line) override;
    std::string HandleSaveState(bco::Vessel& vessel) override;

private:
    const double MIN_VOLTS = 20.0;
    
    bco::PowerProvider& power_;
    
    bool IsPowered() const { 
        return 
            (power_.VoltsAvailable() > MIN_VOLTS) &&
            togPowerSwitch_.IsOn();
    }
    
    bool CanopyIsMoving() const { 
        return 
            IsPowered() && 
            togOpenCloseSwitch_.IsOn() &&
            (animCanopy_.GetState() > 0.0) && 
            (animCanopy_.GetState() < 1.0); 
    }

    bco::AnimationTarget    animCanopy_     { 0.2 };

    bco::AnimationGroup     gpCanopy_ {
        { bm::main::Canopy_id,
        bm::main::CanopyWindowInside_id,
        bm::main::CanopyWindowOutside_id},
        bm::main::CockpitAxisS_loc, bm::main::CockpitAxisP_loc,
        (55 * RAD),
        0, 1
    };

    bco::AnimationGroup     gpCanopyVC_ {
        { bm::vc::CanopyFI_id,
        bm::vc::CanopyFO_id,
        bm::vc::CanopyWindowInside_id,
        bm::vc::CanopyWindowSI_id },
        bm::main::CockpitAxisS_loc, bm::main::CockpitAxisP_loc,
        (55 * RAD),
        0, 1
    };

    sr71::Toggle togPowerSwitch_{
        bm::vc::SwCanopyPower_id,
        bm::vc::SwCanopyPower_loc,
        bm::vc::PowerTopRightAxis_loc,
        bm::pnlright::pnlPwrCanopy_id,
        bm::pnlright::pnlPwrCanopy_vrt,
        bm::pnlright::pnlPwrCanopy_RC,
        cmn::panel::right
    };

    sr71::Toggle togOpenCloseSwitch_{
        bm::vc::SwCanopyOpen_id,
        bm::vc::SwCanopyOpen_loc,
        bm::vc::DoorsRightAxis_loc,
        bm::pnlright::pnlDoorCanopy_id,
        bm::pnlright::pnlDoorCanopy_vrt,
        bm::pnlright::pnlDoorCanopy_RC,
        cmn::panel::right
    };

    sr71::IndicatorLight       status_ {
        bm::vc::MsgLightCanopy_id,
        bm::vc::MsgLightCanopy_vrt,
        bm::pnl::pnlMsgLightCanopy_id,
        bm::pnl::pnlMsgLightCanopy_vrt,
        cmn::panel::main
    };
};

inline Canopy::Canopy(bco::PowerProvider& pwr, bco::Vessel& vessel) :
    power_(pwr)
{
    power_.AttachConsumer(this);
//    vessel.AddControl(&status_);
}

inline void Canopy::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd)
{
    if (IsPowered()) {
        animCanopy_.Update(vessel, togOpenCloseSwitch_.IsOn() ? 1.0 : 0.0, simdt);
    }

    togPowerSwitch_.Update(vessel, simdt);
    togOpenCloseSwitch_.Update(vessel, simdt);

    /*
        off     - no power OR closed
        warn    - yes power AND is moving
        on      - yes power AND open
    */
    auto status = cmn::status::off;
    if (power_.VoltsAvailable() > MIN_VOLTS) {
        if ((animCanopy_.GetState() > 0.0) && (animCanopy_.GetState() < 1.0)) {
            status = cmn::status::warn;
        }
        else {
            if (animCanopy_.GetState() == 1.0) {
                status = cmn::status::on;
            }
        }
    }

    status_.SetStatus(vessel, status);
}

inline bool Canopy::HandleLoadState(bco::Vessel& vessel, const std::string& line)
{
    int tpw = 0;
    int toc = 0;

    std::istringstream in(line);
    in >> tpw;
    in >> toc;
    in >> animCanopy_;
    vessel.SetAnimationState(animCanopy_);

    togPowerSwitch_.SetIsOn(vessel, (tpw == 0) ? false : true);
    togOpenCloseSwitch_.SetIsOn(vessel, (toc == 0) ? false : true);

    return true;
}

inline std::string Canopy::HandleSaveState(bco::Vessel& vessel)
{
    std::ostringstream os;
    os << (togPowerSwitch_.IsOn() ? 1 : 0) << " " << (togOpenCloseSwitch_.IsOn() ? 1 : 0) << " " << animCanopy_;
    return os.str();
}

inline void Canopy::HandleSetClassCaps(bco::Vessel& vessel)
{
    auto vcIdx = vessel.GetVCMeshIndex();
    auto mIdx = vessel.GetMainMeshIndex();

    togPowerSwitch_.Register(vessel);
    togOpenCloseSwitch_.Register(vessel);
    status_.Register(vessel);

    auto idAnim = vessel.CreateVesselAnimation(animCanopy_);
    animCanopy_.VesselId(idAnim);
    vessel.AddVesselAnimationComponent(idAnim, vcIdx, &gpCanopyVC_);
    vessel.AddVesselAnimationComponent(idAnim, mIdx, &gpCanopy_);
}