/*
FuelCell - SR-71r Orbiter Addon
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

const double OXYGEN_BURN_RATE_PER_SEC_100A = (0.2 / 3600) / 100;		// 2 lbs per hour per at 100 amps.
const double HYDROGEN_BURN_RATE_PER_SEC_100A = (0.1 / 3600) / 100;			// 0.3 lbs per hour @ 100 amps.  

namespace bco = bc_orbiter;

/**	Fuel cell.
Models the plane's fuel cell.

The fuel cell uses oxygen and hydrogen to produce electricity.  The fuel cell does require a 28v source to run (start up)
so there must be battery or external connection to start the fuel cell.  When operating it provides 28 volts of power.  
Resource burn rate will be dependent on the current ships amp usage, which comes from the main power system.

To Start:
- Connect external power to the main circuit.
- Turn on 'Main' power switch (up).
- Turn the fuel cell switch (right panel) on (up).
- When the 'AVAIL' light next to the FUEL CELL connect switch lights, turn the FUEL CELL connect switch on (up).
- The external power connect switch can now be turned OFF.

The fuel cell will continue to provide power until it is turned off, or runs out of fuel (hydrogen and oxygen
gauges on the left panel).

Configuration:
FUELCELL a
a = 0/1 fuel cell power switch off/on.
	
*/
class FuelCell 
{
public:
    FuelCell() = default;
    ~FuelCell() = default;

    void Setup(bco::Vessel& vessel);

    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power, IHydrogenProvider& hydrogen, ILiquidOxygenProvider& lox);
    void UpdateVCUI(bco::Vessel& vessel);   // Called to update the VC UI when the VC is active.
    void UpdateRightPanelUI(MESHHANDLE mesh);    // Called to update the 2D panel UI when the panel is active.

    void LoadVC();       // Called when the VC is loaded to setup animations.
    void LoadPanel(bco::Vessel& vessel, PANELHANDLE handle);    // Called when the 2D panel is loaded to setup animations.

    void LoadState(const std::string& line);
    std::string GetState() const;

    void TogglePowerSwitch() { isPowerSwitchOn_ = !isPowerSwitchOn_; }

    double VoltsAvailable() const { return voltsAvailable_; }

private:
    const double MAX_VOLTS = 28.0;
    const double MIN_VOLTS = 20.0;
    const double AMP_DRAW = 4.0;

    bool isPowerSwitchOn_{ false };

    UINT aidVCPowerSwitch_{ 0 };

    int eventId_Power_{ -1 };

    double voltsAvailable_{ 0.0 };

    bco::AnimatedValue<bco::StateUpdateTarget> animPowerSwitch_{ SR71R::ToggleAnimSpeed };
};

inline void FuelCell::Setup(bco::Vessel& vessel)
{
    eventId_Power_ = vessel.GetEventId();

    vessel.RegisterEventHandler(eventId_Power_, [this](int, int) { TogglePowerSwitch(); return true; });

    auto vcIndex = vessel.GetMeshIndex(bm::vc::MESH_NAME);
    aidVCPowerSwitch_ = vessel.CreateVesselAnimation();
    vessel.AddAnimationGroup(
        aidVCPowerSwitch_,
        vcIndex,
        { bm::vc::swFuelCellPower_id },
        bm::vc::swFuelCellPower_loc, bm::vc::PowerTopRightAxis_loc,
        SR71R::ToggleAnimAngle,
        0, 1);
}

inline void FuelCell::UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power, IHydrogenProvider& hydrogen, ILiquidOxygenProvider& lox)
{
    auto hasPower = power.GetPowerLevel() > 0.0;
    auto ampLoad = power.CurrentAmps();

    // We are enable, have power.
    auto reqHydrogen = ampLoad * HYDROGEN_BURN_RATE_PER_SEC_100A * simdt;
    auto actHydrogen = hydrogen.Draw(reqHydrogen);
    auto reqOxy = ampLoad * OXYGEN_BURN_RATE_PER_SEC_100A * simdt;
    auto actOxy = lox.Draw(reqOxy);

    if ((reqHydrogen > actHydrogen) || (reqOxy > actOxy)) {
        voltsAvailable_ = 0.0;
    }
    else {
        voltsAvailable_ = FuelCell::MIN_VOLTS;
    }

    animPowerSwitch_.Update(simdt, isPowerSwitchOn_ ? 1.0 : 0.0);
}

inline void FuelCell::UpdateVCUI(bco::Vessel& vessel)
{
    vessel.SetAnimation(aidVCPowerSwitch_, animPowerSwitch_.GetCurrent());
}

inline void FuelCell::UpdateRightPanelUI(MESHHANDLE mesh)
{
    bco::DrawPanelOnOff(mesh, bm::pnlright::pnlPwrFC_id, bm::pnlright::pnlPwrFCBus_vrt, isPowerSwitchOn_, SR71R::TogglePnlOffset);
}

inline void FuelCell::LoadState(const std::string& line) {}
inline std::string FuelCell::GetState() const {}

inline void FuelCell::LoadVC()
{
    bco::LoadVCSimpleEvent(eventId_Power_, bm::vc::swFuelCellPower_loc, SR71R::ToggleHitRadius);
}

inline void FuelCell::LoadPanel(bco::Vessel& vessel, PANELHANDLE handle)
{
    bco::LoadPanelSimpleEvent(vessel, eventId_Power_, handle, bm::pnlright::pnlPwrFC_RC);
}
