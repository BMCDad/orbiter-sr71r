//	FuelCell - SR-71r Orbiter Addon
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

#include "Orbitersdk.h"

#include "bc_orbiter/Animation.h"
#include "bc_orbiter/vessel.h"
#include "bc_orbiter/control.h"
#include "bc_orbiter/on_off_input.h"
#include "bc_orbiter/on_off_display.h"

#include "IConsumable.h"
#include "PowerSystem.h"
#include "SR71r_mesh.h"
#include "SR71r_common.h"

class PowerSystem;

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
class FuelCell : 
	  public bco::vessel_component
	, public bco::post_step
	, public bco::power_consumer
	, public bco::manage_state
{
	const double MAX_VOLTS = 28.0;
	const double MIN_VOLTS = 20.0;
	const double AMP_DRAW =	  4.0;

public:
	FuelCell(bco::power_provider& pwr, bco::vessel& vessel, bco::consumable& lox, bco::consumable& hydro);

	/**
		Draw down the oxygen and hydrogen levels based on the current amp load.
	*/

	void handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd) override;

	// power_consumer
	double amp_draw() const override { return IsPowered() ? AMP_DRAW : 0.0; }

	// manage_state
	bool handle_load_state(bco::vessel& vessel, const std::string& line) override;
	std::string handle_save_state(bco::vessel& vessel) override;

	// Outputs
	bco::signal<double>&	AvailablePowerSignal()	{ return sigAvailPower_; }			// Volts available from fuel cell.

private:
	bco::power_provider&	power_;
	bco::consumable&		lox_;
	bco::consumable&		hydro_;

	bool IsPowered() const {
		return 
			switchEnabled_.is_on() &&
			(power_.volts_available() > MIN_VOLTS); 
	}

	void SetIsFuelCellPowerAvailable(bool newValue);

	bco::signal<double>	sigAvailPower_;

	bool				isFuelCellAvailable_;
	double				ampDrawFactor_{ 0.0 };

	bco::on_off_input	switchEnabled_		{ { bm::vc::swFuelCellPower_id },
												bm::vc::swFuelCellPower_loc, bm::vc::PowerTopRightAxis_loc,
												toggleOnOff,
												bm::pnl::pnlPwrFC_id,
												bm::pnl::pnlPwrFC_vrt,
												bm::pnl::pnlPwrFC_RC
											};

	bco::on_off_display	lightAvailable_		{
												bm::vc::FuelCellAvailableLight_id,
												bm::vc::FuelCellAvailableLight_vrt,
												bm::pnl::pnlLgtFCPwrAvail_id,
												bm::pnl::pnlLgtFCPwrAvail_vrt,
												0.0244
											};
};