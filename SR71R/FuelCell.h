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
-----

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

#pragma once

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "SR71Toggle.h"

#include "IPowerProvider.h"
#include "IFuelCellSystem.h"
#include "IHydrogenProvider.h"
#include "ILiquidOxygenProvider.h"

const double OXYGEN_BURN_RATE_PER_SEC_100A = (0.2 / 3600) / 100;		// 2 lbs per hour per at 100 amps.
const double HYDROGEN_BURN_RATE_PER_SEC_100A = (0.1 / 3600) / 100;			// 0.3 lbs per hour @ 100 amps.  

namespace bco = bc_orbiter;

class FuelCell : public IFuelCellSystem
{
public:
    FuelCell(bco::Vessel& vessel);
    ~FuelCell() = default;

    // UpdateState should be called from the vessel Step() method after all other systems have had a chance to draw power.
    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power, IHydrogenProvider& hydrogen, ILiquidOxygenProvider& lox);

    void LoadState(const std::string& line);
    std::string GetState() const;

    void TogglePowerSwitch() { isPowerSwitchOn_ = !isPowerSwitchOn_; }

    // IFuelCellSystem
    double VoltsAvailable() const override { return voltsAvailable_; }

private:
    const double MAX_VOLTS = 28.0;
    const double MIN_VOLTS = 20.0;
    const double AMP_DRAW = 4.0;

    bool isPowerSwitchOn_{ false };

    UINT aidVCPowerSwitch_{ 0 };

    int eventId_Power_{ -1 };

    double voltsAvailable_{ 0.0 };

    bco::AnimatedValue<bco::StateUpdateTarget> animPowerSwitch_{ SR71R::ToggleAnimSpeed };

    SR71::Toggle togPower_{
        { bm::vc::swFuelCellPower_id },
        bm::vc::swFuelCellPower_loc, bm::vc::PowerTopRightAxis_loc,
        bm::pnlright::pnlPwrFC_id,
        bm::pnlright::pnlPwrFCBus_vrt,
        bm::pnlright::pnlPwrFCBus_RC,
        SR71R::RightPanel_ID
    };
};

inline FuelCell::FuelCell(bco::Vessel& vessel)
{
    vessel.RegisterUIControl(togPower_);
}

inline void FuelCell::UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power, IHydrogenProvider& hydrogen, ILiquidOxygenProvider& lox)
{
    if (!togPower_.IsOn() || power.GetPowerLevel() < 20.0) {
        voltsAvailable_ = 0.0;
        return;
    }

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
        voltsAvailable_ = FuelCell::MAX_VOLTS;
    }
}

inline void FuelCell::LoadState(const std::string& line) 
{
    std::istringstream is(line);
    int power;
    is >> power;
    togPower_.SetState(power != 0);
}

inline std::string FuelCell::GetState() const 
{
    std::ostringstream out;
    out << togPower_.IsOn() ? 1 : 0;
    return out.str();
}
