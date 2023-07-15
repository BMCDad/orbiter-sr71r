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

#include "bc_orbiter/BaseVessel.h"
#include "bc_orbiter/control.h"
#include "bc_orbiter/on_off_input.h"
#include "bc_orbiter/rotary_display.h"

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
	public bco::set_class_caps,
	public bco::post_step,
	public bco::power_consumer
{
public:
	APU(bco::power_provider& pwr, bco::consumable& main_tank) : 
		power_(pwr),
		main_tank_(main_tank)
	{
		power_.attach_consumer(this);

		signalHydPressure_.attach(gaugeAPULevel_.Slot());
	}

	// set_class_caps
	void handle_set_class_caps(bco::BaseVessel& vessel) override {
		vessel.AddControl(&switchEnabled_);
		vessel.AddControl(&gaugeAPULevel_);
	}
	// *** PoweredComponent ***
	/**
		Draws a fixed amp load when the main circuit is powered and
		the APU switch is on.
	*/

	double amp_draw() const override { return IsPowered() ? 5.0 : 0.0; }

	// post_step
	void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override {
		if (!IsPowered()) {
			signalHydPressure_.fire(0.0);
		}
		else {
			auto hasFuel = main_tank_.level() > 0.0;

			if (hasFuel) {
				main_tank_.draw(APU_BURN_RATE * simdt);
				signalHydPressure_.fire(1.0);
			}
			else {
				signalHydPressure_.fire(0.0);
			}
		}
	}

	// Inputs:
	bco::signal<double>&	HydroPressSignal()		{ return signalHydPressure_; }		// Drives the hydraulic pressure gauge

private:
	bco::power_provider&	power_;
	bco::consumable&		main_tank_;

	bool IsPowered() const {
		return 
			switchEnabled_.is_on() &&
			(power_.volts_available() > 24.0);
	}

	bco::signal<double>		signalHydPressure_;

	bco::on_off_input		switchEnabled_ {			// APU Power
												{ bm::vc::SwAPUPower_id },
												bm::vc::SwAPUPower_location, bm::vc::LeftPanelTopRightAxis_location,
												toggleOnOff,
												bm::pnl::pnlAPUSwitch_id,
												bm::pnl::pnlAPUSwitch_verts,
												bm::pnl::pnlAPUSwitch_RC
											};

	bco::rotary_display<bco::Animation>	gaugeAPULevel_{
												{ bm::vc::gaHydPress_id },
												bm::vc::gaHydPress_location, bm::vc::axisHydPress_location,
												bm::pnl::pnlHydPress_id,
												bm::pnl::pnlHydPress_verts,
												(300 * RAD),	// Clockwise
												0.2,
												[](double d) {return (d); }	// Transform to amps.
											};
};