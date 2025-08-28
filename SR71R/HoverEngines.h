/*
HoverEngines - SR-71r Orbiter Addon
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

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"

#include "IPowerProvider.h"
#include "IHydrogenProvider.h"
#include "ILiquidOxygenProvider.h"

//const double OXYGEN_BURN_RATE_PER_SEC_100A = (0.2 / 3600) / 100;		// 2 lbs per hour per at 100 amps.
//+const double HYDROGEN_BURN_RATE_PER_SEC_100A = (0.1 / 3600) / 100;			// 0.3 lbs per hour @ 100 amps.  

namespace bco = bc_orbiter;

class HoverEngines 
{
public:
    HoverEngines() = default;
    ~HoverEngines() = default;

    void Setup(bco::Vessel& vessel, PROPELLANT_HANDLE mainPropellant);

    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power);
    void UpdateVCUI(bco::Vessel& vessel);   // Called to update the VC UI when the VC is active.
    void UpdateRightPanelUI(MESHHANDLE mesh);    // Called to update the 2D panel UI when the panel is active.

    void LoadVC();       // Called when the VC is loaded to setup animations.
    void LoadPanel(bco::Vessel& vessel, PANELHANDLE handle);    // Called when the 2D panel is loaded to setup animations.

    void LoadState(const std::string& line);
    std::string GetState() const;

    void ToggleDoorSwitch() { isDoorSwitchOpen_ = !isDoorSwitchOpen_; }

private:
    const double MIN_VOLTS = 20.0;

    bool isDoorSwitchOpen_{ false };

    UINT aidVCDoorSwitch_{ 0 };
    UINT aidHoverDoors_{ 0 };

    int eventId_Door_{ -1 };

    bco::AnimatedValue<bco::StateUpdateTarget>  animVCDoorSwitch_;
    bco::AnimatedValue<bco::StateUpdateTarget>  animHoverDoors_;
    
    THRUSTER_HANDLE         hoverThrustHandles_[3];
};

inline void HoverEngines::Setup(bco::Vessel& vessel, PROPELLANT_HANDLE mainPropellant)
{
    eventId_Door_ = vessel.GetEventId();

    vessel.RegisterEventHandler(eventId_Door_, [this](int, int) { ToggleDoorSwitch(); return true; });

    // Animations:
    auto vcIndex = vessel.GetMeshIndex(bm::vc::MESH_NAME);
    auto mainIndex = vessel.GetMeshIndex(bm::main::MESH_NAME);

    aidVCDoorSwitch_ = vessel.CreateVesselAnimation();
    vessel.AddAnimationGroup(
        aidVCDoorSwitch_,
        vcIndex,
        { bm::vc::swHoverDoor_id },
        bm::vc::swHoverDoor_loc, bm::vc::DoorsRightAxis_loc,
        SR71R::ToggleAnimAngle,
        0.0, 1.0);

    aidHoverDoors_ = vessel.CreateVesselAnimation();
    vessel.AddAnimationGroup(
        aidHoverDoors_,
        mainIndex,
        { bm::main::HoverDoorPF_id },
        bm::main::HoverDoorAxisPFF_loc, bm::main::HoverDoorAxisPFA_loc,
        (140 * RAD),
        0, 1);

    vessel.AddAnimationGroup(
        aidHoverDoors_,
        mainIndex,
        { bm::main::HoverDoorSF_id },
        bm::main::HoverDoorAxisSFA_loc, bm::main::HoverDoorAxisSFF_loc,
        (140 * RAD),
        0, 1);

    vessel.AddAnimationGroup(
        aidHoverDoors_,
        mainIndex,
        { bm::main::HoverDoorPA_id },
        bm::main::HoverDoorAxisPF_loc, bm::main::HoverDoorAxisPA_loc,
        (140 * RAD),
        0, 1);

    vessel.AddAnimationGroup(
        aidHoverDoors_,
        mainIndex,
        { bm::main::HoverDoorSA_id },
        bm::main::HoverDoorAxisSA_loc, bm::main::HoverDoorAxisSF_loc,
        (140 * RAD),
        0, 1);

    //  Hover engines : positions are simplified.
    hoverThrustHandles_[0] = vessel.CreateThruster(
        _V(0.0, 0.0, 5.0),
        _V(0, 1, 0),
        SR71R::HOVER_THRUST,
        mainPropellant,
        SR71R::THRUST_ISP);

    hoverThrustHandles_[1] = vessel.CreateThruster(
        _V(-3.0, 0.0, -5.0),
        _V(0, 1, 0),
        SR71R::HOVER_THRUST * 0.5,
        mainPropellant,
        SR71R::THRUST_ISP);

    hoverThrustHandles_[2] = vessel.CreateThruster(
        _V(3.0, 0.0, -5.0),
        _V(0, 1, 0),
        SR71R::HOVER_THRUST * 0.5,
        mainPropellant,
        SR71R::THRUST_ISP);

    vessel.CreateThrusterGroup(hoverThrustHandles_, 3, THGROUP_HOVER);

    EXHAUSTSPEC es_hover[3] =
    {
        { hoverThrustHandles_[0], NULL, NULL, NULL, 4, .25, 0, 0.1, NULL },
        { hoverThrustHandles_[1], NULL, NULL, NULL, 4, .25, 0, 0.1, NULL },
        { hoverThrustHandles_[2], NULL, NULL, NULL, 4, .25, 0, 0.1, NULL }
    };

    PARTICLESTREAMSPEC exhaust_hover = {
        0, 2.0, 13, 150, 0.1, 0.2, 16, 1.0, PARTICLESTREAMSPEC::EMISSIVE,
        PARTICLESTREAMSPEC::LVL_SQRT, 0, 1,
        PARTICLESTREAMSPEC::ATM_PLOG, 1e-5, 0.1
    };

    for (auto i = 0; i < 3; i++) vessel.AddExhaust(es_hover + i);
    vessel.AddExhaustStream(hoverThrustHandles_[0], bm::main::ThrustHoverF_loc, &exhaust_hover);
    vessel.AddExhaustStream(hoverThrustHandles_[1], bm::main::ThrustHoverP_loc, &exhaust_hover);
    vessel.AddExhaustStream(hoverThrustHandles_[2], bm::main::ThrustHoverS_loc, &exhaust_hover);
}

inline void HoverEngines::UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power)
{
    animVCDoorSwitch_.Update(simdt, isDoorSwitchOpen_ ? 1.0 : 0.0);

    if (power.GetPowerLevel() > 0.0) {
        animHoverDoors_.Update(simdt, isDoorSwitchOpen_ ? 1.0 : 0.0);
    }
}


inline void HoverEngines::UpdateVCUI(bco::Vessel& vessel)
{
    vessel.SetAnimation(aidVCDoorSwitch_, animVCDoorSwitch_.GetCurrent());
}

inline void HoverEngines::UpdateRightPanelUI(MESHHANDLE mesh)
{
    bco::DrawPanelOnOff(mesh, bm::pnlright::pnlHoverDoor_id, bm::pnlright::pnlHoverDoor_vrt, isDoorSwitchOpen_, SR71R::TogglePnlOffset);
}

inline void HoverEngines::LoadState(const std::string& line){}
inline std::string HoverEngines::GetState() const {}

inline void HoverEngines::LoadVC()
{
    bco::LoadVCSimpleEvent(eventId_Door_, bm::vc::swHoverDoor_loc, SR71R::ToggleHitRadius);
}

inline void HoverEngines::LoadPanel(bco::Vessel& vessel, PANELHANDLE handle)
{
    bco::LoadPanelSimpleEvent(vessel, eventId_Door_, handle, bm::pnlright::pnlHoverDoor_RC);
}
