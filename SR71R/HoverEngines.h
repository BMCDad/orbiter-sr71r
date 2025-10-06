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

Hover persist state:
- Hover door switch: 0/1
- Hover door position: 0.0 - 1.0

*/
#pragma once

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "SR71Toggle.h"

#include "IPowerProvider.h"
#include "IFuelSystem.h"
#include "IHydrogenProvider.h"
#include "ILiquidOxygenProvider.h"

namespace bco = bc_orbiter;

class HoverEngines 
{
public:
    HoverEngines(bco::Vessel& vessel);
    ~HoverEngines() = default;

    void Setup(bco::Vessel& vessel, IFuelSystem& fuel);

    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power);

    void LoadState(const std::string& line);
    std::string GetState() const;

    void ToggleDoorSwitch() { togPower_.ToggleState(); }

private:
    const double MIN_VOLTS = 20.0;

    UINT aidHoverDoors_{ 0 };

    bco::AnimatedValue<bco::StateUpdateTarget>  animHoverDoors_;
 
    SR71::Toggle togPower_{
        { bm::vc::swHoverDoor_id },
        bm::vc::swHoverDoor_loc, bm::vc::DoorsRightAxis_loc,
        bm::pnlright::pnlHoverDoor_id,
        bm::pnlright::pnlHoverDoor_vrt,
        bm::pnlright::pnlHoverDoor_RC,
        SR71R::RightPanel_ID
    };

    THRUSTER_HANDLE         hoverThrustHandles_[3];
};

inline HoverEngines::HoverEngines(bco::Vessel& vessel)
: hoverThrustHandles_ { nullptr, nullptr, nullptr }
{
    vessel.RegisterUIControl(togPower_);
}

inline void HoverEngines::Setup(bco::Vessel& vessel, IFuelSystem& fuel)
{
    // Animations:
    auto mainIndex = vessel.GetMeshIndex(bm::main::MESH_NAME);

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
        fuel.GetMainProppelantHandle(),
        SR71R::THRUST_ISP);

    hoverThrustHandles_[1] = vessel.CreateThruster(
        _V(-3.0, 0.0, -5.0),
        _V(0, 1, 0),
        SR71R::HOVER_THRUST * 0.5,
        fuel.GetMainProppelantHandle(),
        SR71R::THRUST_ISP);

    hoverThrustHandles_[2] = vessel.CreateThruster(
        _V(3.0, 0.0, -5.0),
        _V(0, 1, 0),
        SR71R::HOVER_THRUST * 0.5,
        fuel.GetMainProppelantHandle(),
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
    bool areDoorsMoving = false;
    if (power.GetPowerLevel() > 20.0) {
        animHoverDoors_.Update(simdt, togPower_.IsOn() ? 1.0 : 0.0);
        auto currentDoorPos = animHoverDoors_.GetCurrent();
        areDoorsMoving = currentDoorPos > 0.01 && currentDoorPos < 0.99;
    }

    power.DrawPower(areDoorsMoving ? SR71R::HOVER_AMPS : 0.0);
}

inline void HoverEngines::LoadState(const std::string& line)
{
      std::istringstream is(line);
   
      int doorState;
      double doorPos;
      is >> doorState >> doorPos;
      togPower_.SetState(doorState != 0);
      animHoverDoors_.LoadState(doorPos, togPower_.IsOn() ? 1.0 : 0.0);

}
inline std::string HoverEngines::GetState() const 
{
    std::ostringstream out;
    out << (togPower_.IsOn() ? 1 : 0) << " " << animHoverDoors_.GetCurrent();
    return out.str();
}