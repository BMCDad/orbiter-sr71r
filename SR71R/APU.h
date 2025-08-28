/*
APU - SR-71r Orbiter Addon
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

#include <cmath>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "IPowerProvider.h"
#include "IFuelSystem.h"

#include "IAvionics.h"

namespace bco = bc_orbiter;

// TEMP SPECS:

const double APU_BURN_RATE = 0.05;   // kg per second - 180 kg per hour (180 / 60) / 60.
const double APU_MIN_VOLT = 20.0;

/**	APU
	Auxiliary Power Unit.  The APU provide power to the hydraulic system of the aircraft
	which in turn powers the flight control surfaces and landing gear.	The APU draws 
    from the same fuel source as the RCS system.
	
    When running, the 'APU' light will be illuminated on the status board.

	Hydraulic level is currently all or nothing. All = 1.0, Nothing = 0.0;

	The APU requires electrical power to function.
	
	Configuration:
	APU = 0/1

	Short cuts:
	None.
*/
class APU
{
public:
    APU() = default;
    ~APU() = default;

    void Setup(bco::Vessel& vessel);

    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power, IFuelSystem& fuel);
    void UpdateVCUI(bco::Vessel& vessel);   // Called to update the VC UI when the VC is active.
    void UpdateRightPanelUI(MESHHANDLE mesh);    // Called to update the 2D panel UI when the panel is active.

    void LoadVC();       // Called when the VC is loaded to setup animations.
    void LoadPanel(bco::Vessel& vessel, PANELHANDLE handle);    // Called when the 2D panel is loaded to setup animations.

    void LoadState(const std::string& line);
    std::string GetState() const;

    void TogglePowerSwitch()
    {
        isPowerSwitchOn_ = !isPowerSwitchOn_;
    }

private:

    const double GaugeAngle = (RAD * 300);

    bco::AnimatedValue<bco::StateUpdateTarget> animPowerSwitch_{ SR71R::ToggleAnimSpeed };
    bco::AnimatedValue<bco::StateUpdateTarget> animAPULevel_{ 0.2 };

    UINT aidVCPowerSwitch_{ 0 };  // Animation ID for the power switch.
    UINT aidVCAPULevel_{ 0 };        // Animation ID for the APU level gauge.

    int eventId_Power_{ -1 };

    bool isPowerSwitchOn_{ false }; // Is the APU power switch on?

    double                  level_{ 0.0 };
};

inline void APU::Setup(bco::Vessel& vessel)
{
    eventId_Power_ = vessel.GetEventId();

    vessel.RegisterEventHandler(eventId_Power_, [this](int, int) { TogglePowerSwitch(); return true; });

    // Animations:
    auto vcIndex = vessel.GetMeshIndex(bm::vc::MESH_NAME);
    auto mainIndex = vessel.GetMeshIndex(bm::main::MESH_NAME);

    // VC Power switch
    aidVCPowerSwitch_ = vessel.CreateVesselAnimation();
    vessel.AddAnimationGroup(
        aidVCPowerSwitch_,
        vcIndex,
        { bm::vc::SwAPUPower_id },
        bm::vc::SwCanopyPower_loc, bm::vc::LeftPanelTopRightAxis_loc,
        SR71R::ToggleAnimAngle,
        0.0, 1.0);
}

inline void APU::UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power, IFuelSystem& fuel)
{ 
    bool hasFuel = fuel.GetMainFuelLevel() > 0.0; // Check if the fuel system has fuel.
    bool hasPower = power.GetPowerLevel() > 0.0;

    level_ = (hasFuel && hasPower) ? 1.0 : 0.0; // Set level to 1.0 if both fuel and power are available, otherwise 0.0.    

    animAPULevel_.Update(simdt, level_); // Update the APU level animation.
    animPowerSwitch_.Update(simdt, isPowerSwitchOn_ ? 1.0 : 0.0); // Update the power switch animation.

    //auto st = (IsPowered()
    //    ? (hasFuel ? bco::status_display::status::on : bco::status_display::status::warn)
    //    : bco::status_display::status::off);
    //status_.set_state(st);
}

inline void APU::UpdateVCUI(bco::Vessel& vessel)
{
    vessel.SetAnimation(aidVCPowerSwitch_, animAPULevel_.GetCurrent());
    vessel.SetAnimation(aidVCAPULevel_, animAPULevel_.GetCurrent());
}

inline void APU::UpdateRightPanelUI(MESHHANDLE mesh)
{ 
    bco::DrawPanelOnOff(mesh, bm::pnlright::pnlAPUSwitch_id, bm::pnlright::pnlAPUSwitch_vrt, isPowerSwitchOn_, SR71R::TogglePnlOffset);
    bco::RotateMesh(mesh, bm::pnlright::pnlHydPress_id, bm::pnlright::pnlHydPress_vrt, GaugeAngle);
}

inline void APU::LoadState(const std::string& line) {}
inline std::string APU::GetState() const {}

inline void APU::LoadVC()
{
    bco::LoadVCSimpleEvent(eventId_Power_, bm::vc::SwAPUPower_loc, SR71R::ToggleHitRadius);
}

inline void APU::LoadPanel(bco::Vessel& vessel, PANELHANDLE handle)
{
    bco::LoadPanelSimpleEvent(vessel, eventId_Power_, handle, bm::pnlright::pnlAPUSwitch_RC);
}
