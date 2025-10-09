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

APU

Auxiliary Power Unit.  The APU provide power to the hydraulic system of the aircraft
which in turn powers the flight control surfaces and landing gear. The APU draws fuel
from the main fuel supply.

When running, the 'APU' light will be illuminated on the status board.

Hydraulic level is currently all or nothing. All = 1.0, Nothing = 0.0;

The APU requires electrical power to function.

Configuration:
APU = 0/1
*/

#pragma once

#include <cmath>
#include <string>
#include <sstream>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"

#include "SR71r_mesh.h"
#include "SR71r2DRight_mesh.h"
#include "SR71Light.h"

#include "ShipMets.h"
#include "IPowerProvider.h"
#include "IFuelSystem.h"
#include "SR71Toggle.h"
#include "SR71Gauge.h"
#include "IAvionics.h"

namespace bco = bc_orbiter;

// TEMP SPECS:

const double APU_BURN_RATE = 0.05;   // kg per second - 180 kg per hour (180 / 60) / 60.
const double APU_MIN_VOLT = 20.0;

class APU
{
public:
    APU(bco::Vessel& vessel);
    ~APU() = default;

    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power, IFuelSystem& fuel);

    void LoadState(const std::string& line);
    std::string GetState() const;

    void TogglePowerSwitch()
    {
        togPower_.ToggleState();
    }

private:
    const double    MIN_VOLTS = 20.0;   // Minimum voltage required to run the APU.
    double          level_{ 0.0 };

    SR71::Toggle togPower_{
        { bm::vc::SwAPUPower_id },
        bm::vc::SwAPUPower_loc, bm::vc::LeftPanelTopRightAxis_loc,
        bm::pnlright::pnlAPUSwitch_id,
        bm::pnlright::pnlAPUSwitch_vrt,
        bm::pnlright::pnlAPUSwitch_RC,
        SR71R::RightPanel_ID
    };

    SR71::Gauge gaugeAPULevel_{
         bm::vc::gaHydPress_id,
         bm::vc::gaHydPress_loc, bm::vc::axisHydPress_loc,
         bm::pnlright::pnlHydPress_id,
         bm::pnlright::pnlHydPress_vrt,
         (300 * RAD),
         level_,
         SR71R::RightPanel_ID
    };

    SR71::Light status_{
        bm::vc::MsgLightAPU_id,
        bm::vc::MsgLightAPU_vrt,
        bm::pnl::pnlMsgLightAPU_id,
        bm::pnl::pnlMsgLightAPU_vrt,
        SR71R::MainPanel_ID
    };
};

inline APU::APU(bco::Vessel& vessel)
{
    vessel.RegisterUIControl(togPower_);
    vessel.RegisterUIControl(gaugeAPULevel_);
    vessel.RegisterUIControl(status_);
}

inline void APU::UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power, IFuelSystem& fuel)
{ 
    bool hasFuel = fuel.GetMainFuelLevel() > 0.0; // Check if the fuel system has fuel.
    bool hasPower = power.GetPowerLevel() > MIN_VOLTS;
    bool isRunning = hasFuel && hasPower && togPower_.IsOn();

    level_ = isRunning ? 1.0 : 0.0; // Set level to 1.0 if both fuel and power are available, otherwise 0.0.
    power.DrawPower(isRunning ? SR71R::APU_AMPS : 0.0); // Draw power if running.

    status_.SetState(hasPower && togPower_.IsOn()
        ? (hasFuel ? SR71::StatusOn : SR71::StatusWarn)
        : SR71::StatusOff);
}

inline void APU::LoadState(const std::string& line)
{
      if (line.empty()) return;
   
      std::istringstream is(line);
      int powerState;
      is >> powerState;
      togPower_.SetState(powerState != 0);
}

inline std::string APU::GetState() const 
{
    std::ostringstream os;
    os << togPower_.IsOn() ? 1 : 0;
    return os.str();
}
