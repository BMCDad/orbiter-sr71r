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
#include "../bc_orbiter/IVessel.h"
#include "../bc_orbiter/OnOffInput.h"
#include "../bc_orbiter/Vessel.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"
#include "SR71rPanelRight_mesh.h"
#include "ToggleSwitch.h"
#include "StatusLight.h"

#include "ShipMets.h"
#include "common.h"

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

struct ICanopy
{
    virtual auto IsPowerSwitchOn() -> bool = 0;
    virtual auto SetPowerSwitch(bool power) -> void = 0;
    virtual auto IsOpenSwitchOn() -> bool = 0;
    virtual auto SetOpenSwitch(bool open) -> void = 0;
    virtual auto IsClosed() -> bool = 0;
    virtual auto IsMoving() -> bool = 0;

    virtual ~ICanopy() = default;
};

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


// Canopy open close togggle switch
inline ToggleData togCanopyOpen = {
    bm::vc::MESH_NAME,
    bm::pnlright::MESH_NAME,
    bm::vc::SwCanopyOpen_id,
    bm::vc::SwCanopyOpen_loc,
    bm::vc::DoorsRightAxis_loc,
    cmn::vc::main,
    cmn::panel::right,
    bm::pnlright::pnlDoorCanopy_RC,
    bm::pnlright::pnlDoorCanopy_id,
    bm::pnlright::pnlDoorCanopy_vrt
};

// Canopy power toggle switch
inline ToggleData togCanopyPower = {
    bm::vc::MESH_NAME,
    bm::pnlright::MESH_NAME,
    bm::vc::SwCanopyPower_id,
    bm::vc::SwCanopyPower_loc,
    bm::vc::PowerTopRightAxis_loc,
    cmn::vc::main,
    cmn::panel::right,
    bm::pnlright::pnlPwrCanopy_RC,
    bm::pnlright::pnlPwrCanopy_id,
    bm::pnlright::pnlPwrCanopy_vrt
};

// Status board canopy light
inline StatusData lgtStatus_ = {
    bm::vc::MESH_NAME,
    bm::pnl::MESH_NAME,
    bm::vc::MsgLightCanopy_id,
    bm::vc::MsgLightCanopy_vrt,
    cmn::vc::main,
    bm::pnl::pnlMsgLightCanopy_id,
    bm::pnl::pnlMsgLightCanopy_vrt,
    cmn::panel::main
};

//bm::vc::MsgLightCanopy_id,
//bm::vc::MsgLightCanopy_vrt,
//cmn::vc::main,
//bm::pnl::pnlMsgLightCanopy_id,
//bm::pnl::pnlMsgLightCanopy_vrt,
//cmn::panel::main


class Canopy : public ICanopy, public bco::VesselComponent, public bco::PowerConsumer
{
public:
    Canopy(bco::PowerProvider& pwr, bco::IVessel& vessel);

    // ICanopy
    auto IsPowerSwitchOn() -> bool          override    { return swPower_.IsOn(); }
    auto SetPowerSwitch(bool power) -> void override    { swPower_.SetState(power); }
    auto IsOpenSwitchOn() -> bool           override    { return swOpen_.IsOn(); }
    auto SetOpenSwitch(bool open) -> void   override    { swOpen_.SetState(open); }
    auto IsClosed() -> bool                 override    { return animCanopy_.GetState() == 0.0; }
    auto IsMoving() -> bool                 override    { return (!IsClosed() && (animCanopy_.GetState() != 1.0)); }


    // SetClassCaps
    void HandleSetClassCaps(bco::IVessel& vessel) override;

    // PowerConsumer
    double AmpDraw() const override { return CanopyIsMoving() ? 4.0 : 0.0; }

    // PostStep
    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override;

    // ManageState
    //bool HandleLoadState(bco::Vessel& vessel, const std::string& line) override;
    //std::string HandleSaveState(bco::Vessel& vessel) override;

private:
    bco::IVessel& vessel_;
    ToggleSwitch<ToggleData> swOpen_{ togCanopyOpen };
    ToggleSwitch<ToggleData> swPower_{ togCanopyPower };

    StatusLight<StatusData> lightStatus_{ lgtStatus_ };

    const double MIN_VOLTS = 20.0;

    bco::PowerProvider& power_;
    
    // bool IsPowered() const { return (power_.VoltsAvailable() > MIN_VOLTS) && swPower_.IsOn(); }
    bool IsPowered() const { return swPower_.IsOn(); }
    
    bool CanopyIsMoving() const { 
        return 
            IsPowered() && 
            swOpen_.IsOn() &&
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

    //bco::VesselTextureElement       status_ {
    //    bm::vc::MsgLightCanopy_id,
    //    bm::vc::MsgLightCanopy_vrt,
    //    cmn::vc::main,
    //    bm::pnl::pnlMsgLightCanopy_id,
    //    bm::pnl::pnlMsgLightCanopy_vrt,
    //    cmn::panel::main
    //};
};

inline Canopy::Canopy(bco::PowerProvider& pwr, bco::IVessel& vessel) 
  : power_(pwr),
    vessel_(vessel)
{
    power_.AttachConsumer(this);
}

inline void Canopy::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd)
{
    if (IsPowered()) {
        animCanopy_.Update(vessel, swOpen_.IsOn() ? 1.0 : 0.0, simdt);
    }
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
    lightStatus_.UpdateStatus(vessel, status);
    swOpen_.Step(vessel_, simdt);
    swPower_.Step(vessel_, simdt);

//    status_.SetState(vessel, status);
}

//inline bool Canopy::HandleLoadState(bco::Vessel& vessel, const std::string& line)
//{
//    std::istringstream in(line);
//    in >> switchPower_;
//    in >> switchOpen_;
//    in >> animCanopy_;
//    vessel.SetAnimationState(animCanopy_);
//
////    switchPower_.SetState(vessel, switchPower);
//    return true;
//}

//inline std::string Canopy::HandleSaveState(bco::Vessel& vessel)
//{
//    std::ostringstream os;
//    os << switchPower_.IsOn() << " " << switchOpen_ << " " << animCanopy_;
//    return os.str();
//}

inline void Canopy::HandleSetClassCaps(bco::IVessel& vessel)
{
    auto vcIdx = vessel.GetMeshIndex(bm::vc::MESH_NAME);
    auto mIdx = vessel.GetMeshIndex(bm::main::MESH_NAME);
    
    auto idAnim = vessel.CreateVesselAnimation(animCanopy_);
    animCanopy_.VesselId(idAnim);
    vessel.AddVesselAnimationComponent(idAnim, vcIdx, &gpCanopyVC_);
    vessel.AddVesselAnimationComponent(idAnim, mIdx, &gpCanopy_);

    // Switches
    swOpen_.Register(vessel);
    swPower_.Register(vessel);

    // Lights
    lightStatus_.Register(vessel);
}