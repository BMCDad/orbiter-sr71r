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

#include "bc_orbiter\BaseVessel.h"
#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\IAnimationState.h"
#include "bc_orbiter\BaseVessel.h"
#include "bc_orbiter\Control.h"

#include "PropulsionController.h"
#include "SR71r_mesh.h"

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
	public bco::power_consumer
{
public:
	APU(bco::consumable& main_fuel);

	// *** PoweredComponent ***
	/**
		Draws a fixed amp load when the main circuit is powered and
		the APU switch is on.
	*/

	// post_step
	void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override;

	// power_consumer
	double amp_load() override { return IsPowered() ? AMP_DRAW : 0.0; }

	// Inputs:
	bco::slot<bool>&		IsEnabledSlot()		{ return slotIsEnabled_; }			// Switch: main APU on/off switch
	bco::slot<double>&		VoltsInputSlot()	{ return slotVoltsInput_; }			// Power supply volts.

	bco::signal<double>&	HydroPressSignal()	{ return signalHydPressure_; }		// Drives the hydraulic pressure gauge

private:
	const double MIN_VOLTS = 24.0;
	const double AMP_DRAW = 5.0;

	bool IsPowered() { return slotIsEnabled_.value() && (slotVoltsInput_.value() > MIN_VOLTS); }


	bco::slot<bool>			slotIsEnabled_;
	bco::slot<double>		slotVoltsInput_;

	bco::signal<double>		signalHydPressure_;

	bco::consumable&		main_fuel_;
};