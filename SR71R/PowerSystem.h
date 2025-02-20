//	PowerSystem - SR-71r Orbiter Addon
//	Copyright(C) 2015  Blake Christensen
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

#include "../bc_orbiter/OnOffInput.h"
#include "../bc_orbiter/RotaryDisplay.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "FuelCell.h"
#include "Common.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"
#include "SR71rPanelRight_mesh.h"

#include <map>

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

class FuelCell;

/**	Power System
	Manages the connections and distribution of the three main power sources:
	- External connection (ship is stopped or docked)
	- Fuelcell
	- Battery
	Allows the pilot to select the power source to use, and monitors voltage availability and consumption.

	What a powered component needs to do:
	Hook a reciever slot up to the VoltLevelSignal() and an amp signal up to the AmpDrawSlot().
	On a change to the receiver slot, check that the new voltage Level is adequate.
	On each step, report through the amp signal the current amp usage for that component.
*/
class PowerSystem : public bco::VesselComponent, public bco::PowerProvider
{
public:
    PowerSystem(bco::Vessel& vessel, FuelCell& fuelCell);

    // PostStep
    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override;

    // ManageState
    bool HandleLoadState(bco::Vessel& vessel, const std::string& line) override;
    std::string HandleSaveState(bco::Vessel& vessel) override;

    void AttachConsumer(bco::PowerConsumer* consumer) override { consumers_.push_back(consumer); }

    double VoltsAvailable() const override { return prevVolts_; }
    double AmpLoad() const override { return ampDraw_; }

private:
    void Update(bco::Vessel& vessel);

    const double			FULL_POWER = 28.0;
    const double			USEABLE_POWER = 24.0;
    const double			AMP_OVERLOAD = 100.0;

    std::vector<bco::PowerConsumer*>  consumers_;

    FuelCell&           fuelCell_;
    double              ampDraw_{ 0.0 };			// Collects the total amps drawn during a step.
    double              batteryLevel_;
    bool                isDrawingBattery_{ false };
    double              prevStep_{ 0.0 };
    double              prevVolts_{ -1.0 };

    bco::OnOffInput       switchEnabled{
        { bm::vc::swMainPower_id },
        bm::vc::swMainPower_loc, bm::vc::PowerTopRightAxis_loc,
        cmn::toggleOnOff,
        bm::pnlright::pnlPwrMain_id,
        bm::pnlright::pnlPwrMain_vrt,
        bm::pnlright::pnlPwrMain_RC,
        1
    };

    bco::OnOffInput       switchConnectExternal_{
        { bm::vc::swConnectExternalPower_id },
        bm::vc::swConnectExternalPower_loc, bm::vc::PowerBottomRightAxis_loc,
        cmn::toggleOnOff,
        bm::pnlright::pnlPwrExtBus_id,
        bm::pnlright::pnlPwrExtBus_vrt,
        bm::pnlright::pnlPwrExtBus_RC,
        1
    };

    bco::OnOffInput       switchConnectFuelCell_{
        { bm::vc::swConnectFuelCell_id },
        bm::vc::swConnectFuelCell_loc, bm::vc::PowerBottomRightAxis_loc,
        cmn::toggleOnOff,
        bm::pnlright::pnlPwrFCBus_id,
        bm::pnlright::pnlPwrFCBus_vrt,
        bm::pnlright::pnlPwrFCBus_RC,
        1
    };

    bco::VesselTextureElement           lightFuelCellConnected_ {
        bm::vc::FuelCellConnectedLight_id,
        bm::vc::FuelCellConnectedLight_vrt,
        cmn::vc::main,
        bm::pnlright::pnlLgtFCPwrOn_id,
        bm::pnlright::pnlLgtFCPwrOn_vrt,
        cmn::panel::right
    };

    bco::VesselTextureElement           lightExternalAvail_ {
        bm::vc::ExtAvailableLight_id,
        bm::vc::ExtAvailableLight_vrt,
        cmn::vc::main,
        bm::pnlright::pnlLgtExtPwrAvail_id,
        bm::pnlright::pnlLgtExtPwrAvail_vrt,
        cmn::panel::right
    };

    bco::VesselTextureElement           lightExternalConnected_{
        bm::vc::ExtConnectedLight_id,
        bm::vc::ExtConnectedLight_vrt,
        cmn::vc::main,
        bm::pnlright::pnlLgtExtPwrOn_id,
        bm::pnlright::pnlLgtExtPwrOn_vrt,
        cmn::panel::right
    };

    bco::RotaryDisplayTarget  gaugePowerVolts_{
        { bm::vc::gaugeVoltMeter_id },
        bm::vc::gaugeVoltMeter_loc, bm::vc::VoltMeterFrontAxis_loc,
        bm::pnlright::pnlVoltMeter_id,
        bm::pnlright::pnlVoltMeter_vrt,
        -(120 * RAD),
        0.2,
        1
    };

    bco::RotaryDisplayTarget  gaugePowerAmps_{
        { bm::vc::gaugeAmpMeter_id },
        bm::vc::gaugeAmpMeter_loc, bm::vc::VoltMeterFrontAxis_loc,
        bm::pnlright::pnlAmpMeter_id,
        bm::pnlright::pnlAmpMeter_vrt,
        (120 * RAD),	// Clockwise
        0.2,
        1
    };

    bco::VesselTextureElement           statusBattery_ {
        bm::vc::MsgLightBattery_id,
        bm::vc::MsgLightBattery_vrt,
        cmn::vc::main,
        bm::pnl::pnlMsgLightBattery_id,
        bm::pnl::pnlMsgLightBattery_vrt,
        cmn::panel::main
    };
};

inline PowerSystem::PowerSystem(bco::Vessel& vessel, FuelCell& fuelCell) 
  : batteryLevel_(1.0),     // Always available for now.
    fuelCell_(fuelCell)
    //, slotFuelCellAvailablePower_([&](double v) {})
{
    vessel.AddControl(&switchEnabled);
    vessel.AddControl(&switchConnectExternal_);
    vessel.AddControl(&switchConnectFuelCell_);
    vessel.AddControl(&lightFuelCellConnected_);
    vessel.AddControl(&lightExternalAvail_);
    vessel.AddControl(&lightExternalConnected_);
    vessel.AddControl(&gaugePowerAmps_);
    vessel.AddControl(&gaugePowerVolts_);
    vessel.AddControl(&statusBattery_);
}

inline bool PowerSystem::HandleLoadState(bco::Vessel& vessel, const std::string& line)
{
    // sscanf_s(configLine + 5, "%i%i%i%lf%lf", &main, &external, &fuelcell, &volt, &batLvl);
    double volt, bl; // Not used, but we read them.

    std::stringstream ss(line);
    ss >> switchEnabled >> switchConnectExternal_ >> switchConnectFuelCell_ >> volt >> bl;
    return true;
}

inline std::string PowerSystem::HandleSaveState(bco::Vessel& vessel)
{
    std::stringstream ss;
    ss << switchEnabled << " " << switchConnectExternal_ << " " << switchConnectFuelCell_ << " 0.0 0.0";
    return ss.str();
}

//void PowerSystem::AddMainCircuitDevice(bco::PoweredComponent* device)
//{
//	mainCircuit_.AddDevice(device);
//}

inline void PowerSystem::Update(bco::Vessel& vessel)
{
    /* Power system update:
    *	Determine if we have a power source available and how much it provides.
    *	Check the current amp total and determine if we have an overload.
    *	Report the voltage available signal so components know what they have to work with.
    */
    auto externalConnected = vessel.IsStoppedOrDocked();
    lightExternalAvail_.SetState(vessel, externalConnected);

    // handle connected power
    auto availExternal =
        externalConnected &&				// External power is available
        switchConnectExternal_.IsOn() 		// External power is connected to the bus
        ? FULL_POWER : 0.0;

    lightExternalConnected_.SetState(vessel, availExternal > USEABLE_POWER);

    // handle fuelcell power
    auto availFuelCell = switchConnectFuelCell_.IsOn() ? fuelCell_.AvailablePower() : 0.0;
    lightFuelCellConnected_.SetState(vessel, availFuelCell > USEABLE_POWER);

    // handle battery power
    auto availBattery = batteryLevel_ * FULL_POWER;
    auto availPower = 0.0;
    isDrawingBattery_ = false;

    if (switchEnabled.IsOn()) {
        availPower = fmax(availExternal, availFuelCell);
        if (availPower < USEABLE_POWER) {
            isDrawingBattery_ = true;
            availPower = availBattery;
        }
    }

    if (availPower != prevVolts_) {
        prevVolts_ = availPower;

        for (auto& c : consumers_) {
            c->OnChange(prevVolts_);
        }
    }

    gaugePowerVolts_.SetState(availPower / FULL_POWER);

    statusBattery_.SetState(vessel,
        (switchEnabled.IsOn() && isDrawingBattery_)
        ? cmn::status::warn
        : cmn::status::off
    );
}

inline void PowerSystem::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd)
{
    double draw = 0.0;

    for (auto& c : consumers_) {
        draw += c->AmpDraw();
    }

    ampDraw_ = fmin(draw, AMP_OVERLOAD);
    gaugePowerAmps_.SetState(ampDraw_ / AMP_OVERLOAD);
    Update(vessel);
}
