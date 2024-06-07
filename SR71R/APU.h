//	APU - SR-71r Orbiter Addon
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

#include "../bc_orbiter/vessel.h"
#include "../bc_orbiter/control.h"
#include "../bc_orbiter/on_off_input.h"
#include "../bc_orbiter/rotary_display.h"
#include "../bc_orbiter/status_display.h"

#include "SR71r_mesh.h"
#include "SR71r_common.h"

namespace bco = bc_orbiter;

// TEMP SPECS:

const double APU_BURN_RATE = 0.05;   // kg per second - 180 kg per hour (180 / 60) / 60.
const double APU_MIN_VOLT = 20.0;
class VESSEL3;

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
class APU :
    public bco::vessel_component,
    public bco::post_step,
    public bco::power_consumer,
    public bco::manage_state,
    public bco::hydraulic_provider
{
public:
    APU(bco::vessel & vessel, bco::power_provider & pwr) :
        power_(pwr)
    {
        power_.attach_consumer(this);
        vessel.AddControl(&switchEnabled_);
        vessel.AddControl(&gaugeAPULevel_);
        vessel.AddControl(&status_);
    }

    double amp_draw() const override { return IsPowered() ? 5.0 : 0.0; }

    // manage_state
    bool handle_load_state(bco::vessel & vessel, const std::string & line) override {
        std::stringstream in(line);
        in >> switchEnabled_;
        return true;
    }

    std::string handle_save_state(bco::vessel & vessel) override
    {
        std::ostringstream os;
        os << switchEnabled_;
        return os.str();
    }

    // haudralic_provider
    double level() const override { return level_; }

    // post_step
    void handle_post_step(bco::vessel & vessel, double simt, double simdt, double mjd) override {
        bool hasFuel = false;

        if (!IsPowered()) {
            level_ = 0.0;
        }
        else {
            hasFuel = slotFuelLevel_.value() > 0.0;

            if (hasFuel) {
                // Note:  We don't actually draw fuel, but when its gone the APU will shutdown.
                level_ = 1.0;
            }
            else {
                level_ = 0.0;
            }
        }

        gaugeAPULevel_.set_state(level_);

        status_.set_state(
            IsPowered()
            ? hasFuel
            ? bco::status_display::status::on
            : bco::status_display::status::warn
            : bco::status_display::status::off
        );
    }

    bco::slot<double>&FuelLevelSlot() { return slotFuelLevel_; }

private:
    bco::power_provider & power_;

    bool IsPowered() const {
        return switchEnabled_.is_on() && (power_.volts_available() > 24.0);
    }

    double                  level_{ 0.0 };
    bco::slot<double>       slotFuelLevel_;


    bco::on_off_input       switchEnabled_{
        { bm::vc::SwAPUPower_id },
        bm::vc::SwAPUPower_loc, bm::vc::LeftPanelTopRightAxis_loc,
        toggleOnOff,
        bm::pnlright::pnlAPUSwitch_id,
        bm::pnlright::pnlAPUSwitch_vrt,
        bm::pnlright::pnlAPUSwitch_RC,
        1
    };

    bco::rotary_display_target  gaugeAPULevel_{
        { bm::vc::gaHydPress_id },
        bm::vc::gaHydPress_loc, bm::vc::axisHydPress_loc,
        bm::pnlright::pnlHydPress_id,
        bm::pnlright::pnlHydPress_vrt,
        (300 * RAD),	// Clockwise
        0.2,
        1
    };

    bco::status_display         status_{
        bm::vc::MsgLightAPU_id,
        bm::vc::MsgLightAPU_vrt,
        bm::pnl::pnlMsgLightAPU_id,
        bm::pnl::pnlMsgLightAPU_vrt,
        0.0361
    };
};