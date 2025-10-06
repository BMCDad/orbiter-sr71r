/*
AirBrake - SR-71r Orbiter Addon
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

Airbrake
The vessel airbrake is comprised of the four elevon panel that open into the airflow,
two on top, two on the bottom.  The airbrake is hydraulically powered, and has four
settings from fully closed to fully open.  The airbrake is used to increase drag and slow
the vessel down, and is typically used during descent and landing.

Configuration:
AIRBRAKE a b
a = Lever position 0.0-1.0
b = Airbrake animation position 0.0-1.0

Status Panel:
The status panel will show the [ABRK] light in the 'WARN' position when the airbrake is not fully retracted.


*/
#pragma once

#include <cmath>
#include <string>
#include <sstream>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"
#include "..\bc_orbiter\Tools.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71r2DMain_mesh.h"

#include "SR71Light.h"
#include "ShipMets.h"

#include "IHydraulicProvider.h"

namespace bco = bc_orbiter;

class AirBrake
{
public:
    AirBrake(bco::Vessel& vessel);
    ~AirBrake() = default;
    void Setup(bco::Vessel& vessel);
    void UpdateState(bco::Vessel& vessel, double simdt, IHydraulicProvider& hydro);
    void UpdateVCUI(bco::Vessel& vessel);   // Called to update the VC UI when the VC is active.
    void UpdateMainPanelUI(MESHHANDLE mesh);    // Called to update the 2D panel UI when the panel is active.

    void LoadVC();       // Called when the VC is loaded to setup animations.
    void LoadPanel(bco::Vessel& vessel, PANELHANDLE handle);    // Called when the 2D panel is loaded to setup animations.

    void IncreaseBrake() { switchPosition_ = min(1.0, switchPosition_ + 0.33); }
    void DecreaseBrake() { switchPosition_ = max(0.0, switchPosition_ - 0.33); }

    void LoadState(const std::string& line);
    std::string GetState() const;

private:
    bco::AnimatedValue<bco::StateUpdateTarget> airBrakeAnim_;
    bco::AnimatedValue<bco::StateUpdateTarget> animAirBrakeSwitch_{ SR71R::ToggleAnimSpeed };

    UINT aidAirBrake_{ 0 };
    UINT aidAirBrakeSwitch_{ 0 };

    int eventId_Increase_{ -1 };
    int eventId_Decrease_{ -1 };

    double switchPosition_{ 0.0 }; // 0.0 = off, 1.0 = full brake
    
    const VECTOR3 sTrans{ bm::pnl::pnlSpeedBrakeFull_loc - bm::pnl::pnlSpeedBrakeOff_loc };

    SR71::Light status_ {
        bm::vc::MsgLightSpeedBrake_id,
        bm::vc::MsgLightSpeedBrake_vrt,
        bm::pnl::pnlMsgLightSpeedBrake_id,
        bm::pnl::pnlMsgLightSpeedBrake_vrt,
        SR71R::MainPanel_ID
    };
};

inline AirBrake::AirBrake(bco::Vessel& vessel)
{
    vessel.RegisterUIControl(status_);
}


inline void AirBrake::Setup(bco::Vessel& vessel)
{
    // Events
    
    eventId_Increase_ = vessel.RegisterEventHandler([this](int, int) { IncreaseBrake(); return true; });
    eventId_Decrease_ = vessel.RegisterEventHandler([this](int, int) { DecreaseBrake(); return true; });

    // Animated values
    aidAirBrakeSwitch_ = vessel.CreateVesselAnimation();
    aidAirBrake_ = vessel.CreateVesselAnimation();

    auto vcIndex = vessel.GetMeshIndex(bm::vc::MESH_NAME);
    auto mainIndex = vessel.GetMeshIndex(bm::main::MESH_NAME);

    // VC Switch
    vessel.AddAnimationGroup(
        aidAirBrakeSwitch_,
        vcIndex,
        { bm::vc::AirBrakeLever_id },
        bm::vc::SpBrakeAxisRight_loc, bm::vc::SpBrakeAxisLeft_loc,
        (58 * RAD),
        0.0, 1.0);

    vessel.AddAnimationGroup(
        aidAirBrake_,
        mainIndex,
        { bm::main::ElevonPIT_id },
        bm::main::AirBrakeAxisPTO_loc, bm::main::AirBrakeAxisPTI_loc,
        (70 * RAD),
        0.0, 1.0);

    vessel.AddAnimationGroup(
        aidAirBrake_,
        mainIndex,
        { bm::main::ElevonPIB_id },
        bm::main::AirBrakeAxisPTI_loc, bm::main::AirBrakeAxisPTO_loc,
        (70 * RAD),
        0.0, 1.0);

    vessel.AddAnimationGroup(
        aidAirBrake_,
        mainIndex,
        { bm::main::ElevonSIT_id },
        bm::main::AirBrakeAxisSTO_loc, bm::main::AirBrakeAxisSTI_loc,
        (70 * RAD),
        0.0, 1.0);

    vessel.AddAnimationGroup(
        aidAirBrake_,
        mainIndex,
        { bm::main::ElevonSIB_id },
        bm::main::AirBrakeAxisSBI_loc, bm::main::AirBrakeAxisSBO_loc,
        (70 * RAD),
        0.0, 1.0);
}

inline void AirBrake::UpdateState(bco::Vessel& vessel, double simdt, IHydraulicProvider& hydro)
{
      auto isAirBrakeMoving = false;
   
      animAirBrakeSwitch_.Update(simdt, switchPosition_);
      
      if (hydro.Level() > 0.0) {
         isAirBrakeMoving = airBrakeAnim_.Update(simdt, switchPosition_);
      }
   
      vessel.SetAnimation(aidAirBrake_, airBrakeAnim_.GetCurrent());
      auto status = airBrakeAnim_.GetCurrent() > 0.01 ? SR71R::PNL_WARN : SR71R::PNL_OFF;
      status_.SetState(status);
}

inline void AirBrake::UpdateVCUI(bco::Vessel& vessel)
{
    vessel.SetAnimation(aidAirBrakeSwitch_, animAirBrakeSwitch_.GetCurrent());
}

inline void AirBrake::UpdateMainPanelUI(MESHHANDLE mesh)
{
    bco::TranslateMesh(
        mesh,
        bm::pnl::pnlAirBrake_id,
        bm::pnl::pnlAirBrake_vrt,
        sTrans * animAirBrakeSwitch_.GetCurrent());
}

inline void AirBrake::LoadVC()
{ 
    bco::LoadVCSimpleEvent(eventId_Increase_, bm::vc::ABTargetIncrease_loc, 0.01);
    bco::LoadVCSimpleEvent(eventId_Decrease_, bm::vc::ABTargetDecrease_loc, 0.01);
}

inline void AirBrake::LoadPanel(bco::Vessel& vessel, PANELHANDLE handle)
{
    bco::LoadPanelSimpleEvent(vessel, eventId_Increase_, handle, bm::pnl::pnlAirBrakeIncrease_RC);
    bco::LoadPanelSimpleEvent(vessel, eventId_Decrease_, handle, bm::pnl::pnlAirBrakeDecrease_RC);
}

inline void AirBrake::LoadState(const std::string& line)
{
    std::istringstream iss(line);
    double animation;
    iss >> switchPosition_ >> animation;
    airBrakeAnim_.LoadState(animation, switchPosition_);
}

inline std::string AirBrake::GetState() const
{
    std::ostringstream os;
    os << switchPosition_ << " " << airBrakeAnim_.GetCurrent();
    return os.str();
}
