/*
Landing Gear - SR-71r Orbiter Addon
Copyright(C) 2025  Blake Christensen

Manages the landing gear of the SR-71r.  The landing gear is powered by hydraulic power
via the apu.

-----

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

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"

#include "IHydraulicProvider.h"

namespace bco = bc_orbiter;

class LandingGear
{
public:
    LandingGear() = default;
    ~LandingGear() = default;

    void Setup(bco::Vessel& vessel);

    void UpdateState(bco::Vessel& vessel, double simdt, IHydraulicProvider& hydro);
    void UpdateVCUI(bco::Vessel& vessel);   // Called to update the VC UI when the VC is active.
    void UpdateMainPanelUI(MESHHANDLE mesh);    // Called to update the 2D panel UI when the panel is active.

    void LoadVC();       // Called when the VC is loaded to setup animations.
    void LoadPanel(bco::Vessel& vessel, PANELHANDLE handle);    // Called when the 2D panel is loaded to setup animations.

    void RaiseLandingGear() {
        isLandingGearDeployed_ = false;
        isLandingGearChanged_ = true;
    }

    void LowerLandingGear() {
        isLandingGearDeployed_ = true;
        isLandingGearChanged_ = true;
    }

    void LoadState(const std::string& line);
    std::string GetState() const;
private:

    bco::AnimatedValue<bco::StateUpdateTarget> animVCLandingGearSwitch_{ SR71R::ToggleAnimSpeed };
    bco::AnimatedValue<bco::StateUpdateTarget> animLandingGear_{ SR71R::ToggleAnimSpeed };

    UINT aidVCLandingGearSwitch_{ 0 };
    UINT aidLandingGear_{ 0 };

    int eventId_GearUp_{ -1 };
    int eventId_GearDown_{ -1 };

    bool isLandingGearDeployed_{ false }; // True if the landing gear is deployed, false if retracted.
    bool isLandingGearChanged_{ false }; // True if the landing gear has changed state since the last step.

    const VECTOR3 pnlSwitchTrans{ bm::pnl::pnlLandingGearKnobDown_loc - bm::pnl::pnlLandingGearKnobUp_loc };
};

inline void LandingGear::Setup(bco::Vessel& vessel)
{
    // Events
    eventId_GearUp_ = vessel.GetEventId();
    eventId_GearDown_ = vessel.GetEventId();

    vessel.RegisterEventHandler(eventId_GearUp_,    [this](int, int) { RaiseLandingGear(); return true; });
    vessel.RegisterEventHandler(eventId_GearDown_,  [this](int, int) { LowerLandingGear(); return true; });

    aidVCLandingGearSwitch_ = vessel.CreateVesselAnimation();
    aidLandingGear_ = vessel.CreateVesselAnimation();

    auto vcIndex = vessel.GetMeshIndex(bm::vc::MESH_NAME);
    auto mainIndex = vessel.GetMeshIndex(bm::main::MESH_NAME);

    // VC
    vessel.AddAnimationGroup(
        aidVCLandingGearSwitch_,
        vcIndex,
        { bm::vc::LGHandle_id },
        bm::vc::LGHandle_loc, bm::vc::LGHandleAxis_loc,
        (40 * RAD), 
        0.0, 1.0);

    // External Landing Gear sequence
    // *** Front gear
    // Right front door.
    vessel.AddAnimationGroup(
        aidLandingGear_,
        mainIndex,
        { bm::main::LGDoorSF_id },
        bm::main::LGFRBackPivot_loc, bm::main::LGFRFrontPivot_loc,
        (90 * RAD),
        0.0, 0.3);

    // Left front door.
    vessel.AddAnimationGroup(
        aidLandingGear_,
        mainIndex,
        { bm::main::LGDoorPF_id },
        bm::main::LGFLFrontPivot_loc, bm::main::LGFLBackPivot_loc,
        (90 * RAD),
        0.0, 0.3);

    // Front gear.
    vessel.AddAnimationGroup(
        aidLandingGear_,
        mainIndex,
        { bm::main::LGFrontArm_id,
            bm::main::LGFrontAxel_id,
            bm::main::LGCrossF_id,
            bm::main::LGFrontLeftWheel_id,
            bm::main::LGFrontRightWheel_id },
        bm::main::LGFrontRightPivot_loc, bm::main::LGFrontLeftPivot_loc,
        (90 * RAD),
        0.2, 1.0);

    // ***Left gear
    // Left outer door
    vessel.AddAnimationGroup(
        aidLandingGear_,
        mainIndex,
        { bm::main::LGDoorPO_id },
        bm::main::LGDoorAxisPOA_loc, bm::main::LGDoorAxisPOF_loc,
        (-100 * RAD),
        0.1, 1.0);

    // Left inner door
    vessel.AddAnimationGroup(
        aidLandingGear_,
        mainIndex,
        { bm::main::LGDoorPI_id },
        bm::main::LGDoorAxisPIA_loc, bm::main::LGDoorAxisPIF_loc,
        (100 * RAD),
        0.0, 0.3);

    // Left gear upper
    auto parent = vessel.AddAnimationGroup(
        aidLandingGear_,
        mainIndex,
        { bm::main::LGLArm_id },
        bm::main::LGCrossAxisPA_loc, bm::main::LGCrossAxisPF_loc,
        (-90 * RAD),
        0.2, 1.0);

    // Left gear lower
    vessel.AddAnimationGroup(
        aidLandingGear_,
        mainIndex,
        { bm::main::LGLFork_id,
            bm::main::LGLWheel1_id,
            bm::main::LGLWheel2_id,
            bm::main::LGLWheel3_id },
        _V(-0.39, 0, 0),
        0.5, 0.8,
        parent);

    // ***Right gear
    // Right outer door
    vessel.AddAnimationGroup(
        aidLandingGear_,
        mainIndex,
        { bm::main::LGDoorSO_id },
        bm::main::LGDoorAxisSOA_loc, bm::main::LGDoorAxisSOF_loc,
        (100 * RAD),
        0.2, 1.0);

    // Right inner door
    vessel.AddAnimationGroup(
        aidLandingGear_,
        mainIndex,
        { bm::main::LGDoorSI_id },
        bm::main::LGDoorAxisSIA_loc, bm::main::LGDoorAxisSIF_loc,
        (-100 * RAD),
        0.0, 0.3);

    // Right gear upper
    parent = vessel.AddAnimationGroup(
        aidLandingGear_,
        mainIndex,
        { bm::main::LGRArm_id },
        bm::main::LGCrossAxisSA_loc, bm::main::LGCrossAxisSF_loc,
        (90 * RAD),
        0.2, 1.0);

    // Right gear lower
    vessel.AddAnimationGroup(
        aidLandingGear_,
        mainIndex,
        { bm::main::LGRFork_id,
            bm::main::LGRWheel1_id,
            bm::main::LGRWheel2_id,
            bm::main::LGRWheel3_id },
        _V(0.39, 0, 0),
        0.5, 0.8,
        parent);
}

inline void LandingGear::UpdateState(bco::Vessel& vessel, double simdt, IHydraulicProvider& hydro)
{
    auto isLandingGearMoving = false;
    animVCLandingGearSwitch_.Update(simdt, isLandingGearDeployed_ ? 1.0 : 0.0); // Always updates.

    if (hydro.Level() > 0.0) {
         isLandingGearMoving = animLandingGear_.Update(simdt, isLandingGearDeployed_ ? 1.0 : 0.0);
    }

    vessel.SetAnimation(aidLandingGear_, animLandingGear_.GetCurrent());
}

inline void LandingGear::UpdateVCUI(bco::Vessel& vessel)
{
    vessel.SetAnimation(aidVCLandingGearSwitch_, animVCLandingGearSwitch_.GetCurrent());
}

inline void LandingGear::UpdateMainPanelUI(MESHHANDLE mesh)
{
    bco::TranslateMesh(
        mesh,
        bm::pnl::pnlLandingGear_id,
        bm::pnl::pnlLandingGear_vrt,
        pnlSwitchTrans * animVCLandingGearSwitch_.GetCurrent());
}

inline void LandingGear::LoadVC()
{
    bco::LoadVCSimpleEvent(eventId_GearUp_, bm::vc::GearLeverUpTarget_loc, 0.01);
    bco::LoadVCSimpleEvent(eventId_GearDown_, bm::vc::GearLeverDownTarget_loc, 0.01);
}

inline void LandingGear::LoadPanel(bco::Vessel& vessel, PANELHANDLE handle)
{
    bco::LoadPanelSimpleEvent(vessel, eventId_GearUp_, handle, bm::pnl::pnlLandingGearUp_RC);
    bco::LoadPanelSimpleEvent(vessel, eventId_GearDown_, handle, bm::pnl::pnlLandingGearDown_RC);
}
