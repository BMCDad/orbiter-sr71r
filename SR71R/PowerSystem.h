/*
PowerSystem - SR-71r Orbiter Addon
Copyright(C) 2015  Blake Christensen

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

POWER a b c
a = 0/1 Main power switch off/on.
b = 0/1 External power connect switch off/on.
c = 0/1 Fuel cell connect switch off/on.

*/

#pragma once

#include "..\bc_orbiter\Vessel.h"

#include "SR71Gauge.h"
#include "SR71Light.h"
#include "SR71Toggle.h"

#include "IPowerProvider.h"
#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

/**	Power System
	Manages the connections and distribution of the three main power sources:
	- External connection (ship is stopped or docked)
	- Fuelcell
	- Battery
	Allows the pilot to select the power source to use, and monitors voltage availability and consumption.

	What a powered component needs to do:
	Hook a receiver slot up to the VoltLevelSignal() and an amp signal up to the AmpDrawSlot().
	On a change to the receiver slot, check that the new voltage level is adequate.
	On each step, report through the amp signal the current amp usage for that component.
*/
class PowerSystem : public IPowerProvider {
public:
    PowerSystem(bco::Vessel& vessel);
    ~PowerSystem() = default;

    // *PreStep should be placed at the start of the vessel Step() method.
    void UpdateStatePreStep(bco::Vessel& vessel, double simdt);

    // *PostStep should be placed at the end of the vessel Step() method, after all components have had a chance to draw power.
    void UpdateStatePostStep(bco::Vessel& vessel, double simdt, IFuelCellSystem& fuelCell);

    // IPowerProvider
    double GetPowerLevel() const override { return availVolts_; }
    void DrawPower(double amps) override;
    double CurrentAmps() const override { return ampStepDraw_; }

    void LoadState(const std::string& line);
    std::string GetState() const;

private:

    const double    FULL_POWER = 28.0;
    const double    USEABLE_POWER = 24.0;
    const double    AMP_OVERLOAD = 100.0;

    double      ampStepDraw_        { 0.0 };        // Collects the total amps drawn during a step.
    double      batteryLevel_       { 27.0 };
    bool        isDrawingBattery_   { false };
    double      prevStep_           { 0.0 };
    double      prevVolts_          { -1.0 };
    
    double      availVolts_         { 0.0 };   // The voltage currently available to the system.
    double      voltLevelValue_     { 0.0 }; // The ratio value used to drive the volt gauge.

    double      ampLevelValue_      { 0.0 }; // The ratio value used to drive the amp gauge.

    SR71::Toggle togEnabled {
        { bm::vc::swMainPower_id },
        bm::vc::swMainPower_loc, bm::vc::PowerTopRightAxis_loc,
        bm::pnlright::pnlPwrMain_id,
        bm::pnlright::pnlPwrMain_vrt,
        bm::pnlright::pnlPwrMain_RC,
        SR71R::RightPanel_ID
    };

    SR71::Toggle togConnectExternal_{
        { bm::vc::swConnectExternalPower_id },
        bm::vc::swConnectExternalPower_loc, bm::vc::PowerBottomRightAxis_loc,
        bm::pnlright::pnlPwrExtBus_id,
        bm::pnlright::pnlPwrExtBus_vrt,
        bm::pnlright::pnlPwrExtBus_RC,
        SR71R::RightPanel_ID
    };

    SR71::Toggle togConnectFuelCell_{
        { bm::vc::swConnectFuelCell_id },
        bm::vc::swConnectFuelCell_loc, bm::vc::PowerBottomRightAxis_loc,
        bm::pnlright::pnlPwrFCBus_id,
        bm::pnlright::pnlPwrFCBus_vrt,
        bm::pnlright::pnlPwrFCBus_RC,
        SR71R::RightPanel_ID
    };

    SR71::Light lightFuelCellAvailable_{
        bm::vc::FuelCellAvailableLight_id,
        bm::vc::FuelCellAvailableLight_vrt,
        bm::pnlright::pnlLgtFCPwrAvail_id,
        bm::pnlright::pnlLgtFCPwrAvail_vrt,
        SR71R::RightPanel_ID
    };

    SR71::Light lightFuelCellConnected_{
        bm::vc::FuelCellConnectedLight_id,
        bm::vc::FuelCellConnectedLight_vrt,
        bm::pnlright::pnlLgtFCPwrOn_id,
        bm::pnlright::pnlLgtFCPwrOn_vrt,
        SR71R::RightPanel_ID
    };

    SR71::Light lightExternalAvailable_{
        bm::vc::ExtAvailableLight_id,
        bm::vc::ExtAvailableLight_vrt,
        bm::pnlright::pnlLgtExtPwrAvail_id,
        bm::pnlright::pnlLgtExtPwrAvail_vrt,
        SR71R::RightPanel_ID
    };

    SR71::Light lightExternalConnected_{
        bm::vc::ExtConnectedLight_id,
        bm::vc::ExtConnectedLight_vrt,
        bm::pnlright::pnlLgtExtPwrOn_id,
        bm::pnlright::pnlLgtExtPwrOn_vrt,
        SR71R::RightPanel_ID
    };

    SR71::Gauge  gaugePowerVolts_{
        { bm::vc::gaugeVoltMeter_id },
        bm::vc::gaugeVoltMeter_loc, bm::vc::VoltMeterFrontAxis_loc,
        bm::pnlright::pnlVoltMeter_id,
        bm::pnlright::pnlVoltMeter_vrt,
        -120 * RAD, // max angle
        voltLevelValue_,
        SR71R::RightPanel_ID
    };

    SR71::Gauge  gaugePowerAmps_{
        { bm::vc::gaugeAmpMeter_id },
        bm::vc::gaugeAmpMeter_loc, bm::vc::VoltMeterFrontAxis_loc,
        bm::pnlright::pnlAmpMeter_id,
        bm::pnlright::pnlAmpMeter_vrt,
        120 * RAD, // max angle
        ampLevelValue_,
        SR71R::RightPanel_ID
    };

    //bco::status_display         statusBattery_{
    //    bm::vc::MsgLightBattery_id,
    //    bm::vc::MsgLightBattery_vrt,
    //    bm::pnl::pnlMsgLightBattery_id,
    //    bm::pnl::pnlMsgLightBattery_vrt,
    //    0.0361
    //};
};

inline PowerSystem::PowerSystem(bco::Vessel& vessel)
{
    vessel.RegisterUIControl(togEnabled);
    vessel.RegisterUIControl(togConnectExternal_);
    vessel.RegisterUIControl(togConnectFuelCell_);
    vessel.RegisterUIControl(lightFuelCellAvailable_);
    vessel.RegisterUIControl(lightFuelCellConnected_);
    vessel.RegisterUIControl(lightExternalAvailable_);
    vessel.RegisterUIControl(lightExternalConnected_);
    vessel.RegisterUIControl(gaugePowerVolts_);
    vessel.RegisterUIControl(gaugePowerAmps_);
}
inline void PowerSystem::DrawPower(double amps)
{
    ampStepDraw_ += amps;
}

inline void PowerSystem::UpdateStatePreStep(bco::Vessel& vessel, double simdt)
{
    ampStepDraw_ = 0.0; // Reset the amp draw for this step.
}

inline void PowerSystem::UpdateStatePostStep(bco::Vessel& vessel, double simdt, IFuelCellSystem& fuelCell)
{
    ampStepDraw_ = fmin(ampStepDraw_, AMP_OVERLOAD);
    ampLevelValue_ = ampStepDraw_ / AMP_OVERLOAD;

    /* Power system update:
    *	Determine if we have a power source available and how much it provides.
    *	Check the current amp total and determine if we have an overload.
    *	Report the voltage available signal so components know what they have to work with.
    */

    auto externalConnected = vessel.IsStoppedOrDocked();
    lightExternalAvailable_.SetState(externalConnected);

    // handle connected power
    auto availExternal =
        externalConnected &&        // External power is available
        togConnectExternal_.IsOn()  // External power is connected to the bus
        ? FULL_POWER : 0.0;

    lightExternalConnected_.SetState(availExternal > USEABLE_POWER);

    // handle fuelcell power
    lightFuelCellAvailable_.SetState(fuelCell.VoltsAvailable() > USEABLE_POWER);
    auto availFuelCellVolts = togConnectFuelCell_.IsOn() ? fuelCell.VoltsAvailable() : 0.0;
    lightFuelCellConnected_.SetState(availFuelCellVolts > USEABLE_POWER);

    // handle battery power
    auto availBattery = batteryLevel_ * FULL_POWER;
    isDrawingBattery_ = false;

    if (togEnabled.IsOn()) {
        availVolts_ = fmax(availExternal, availFuelCellVolts);
        if (availVolts_ < USEABLE_POWER) {
            isDrawingBattery_ = true;
            availVolts_ = availBattery;
        }
    }
    else {
        availVolts_ = 0.0;
    }

    // Somehow notify powered components of the available voltage.
    //if (availPower != prevVolts_) {
    //    prevVolts_ = availPower;

    //    for (auto& c : consumers_) {
    //        c->on_change(prevVolts_);
    //    }
    //}

    voltLevelValue_ = availVolts_ / FULL_POWER;

    //statusBattery_.set_state(
    //    (switchEnabled.is_on() && isDrawingBattery_)
    //    ? bco::status_display::status::warn
    //    : bco::status_display::status::off
    //);
}

inline void PowerSystem::LoadState(const std::string& line)
{
    std::istringstream in(line);
    int power, external, fuelcell;

    in >> power >> external >> fuelcell;
    togEnabled.SetState(power != 0);
    togConnectExternal_.SetState(external != 0);
    togConnectFuelCell_.SetState(fuelcell != 0);
}

inline std::string PowerSystem::GetState() const
{
    std::ostringstream out;
    out << (togEnabled.IsOn() ? 1 : 0) << " " << (togConnectExternal_.IsOn() ? 1 : 0) << " " << (togConnectFuelCell_.IsOn() ? 1 : 0);
    return out.str();
}
