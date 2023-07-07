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

#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\Circuit.h"
#include "bc_orbiter\signals.h"

#include "FuelCell.h"
#include "SR71r_mesh.h"

#include <map>

namespace bco = bc_orbiter;

class FuelCell;

/**	Power System
	Manages the connections and distribution of the three main power sources:
	- External connection (ship is stopped or docked)
	- Fuelcell
	- Battery
	Allows the pilot to select the power source to use, and monitors voltage availability and consumption.

	What a powered component needs to do:
	Hook a reciever slot up to the VoltLevelSignal() and an amp signal up to the AmpDrawSlot().
	On a change to the receiver slot, check that the new voltage level is adequate.
	On each step, report through the amp signal the current amp usage for that component.
*/
class PowerSystem :	
	public bco::vessel_component,
	public bco::post_step,
	public bco::manage_state
{
public:
	PowerSystem();

	// post_step
	void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override
	{
		Update(vessel);
	}

	// manage_state
	bool handle_load_state(const std::string& line) override;
	std::string handle_save_state() override;

	void add_user(bco::power_consumer* user) { power_users_.push_back(user); }

	// Inputs:
	bco::slot<bool>&	MainPowerSlot()					{ return slotIsEnabled_; }				// Switch:  Main power
	bco::slot<bool>&	ExternalConnectSlot()			{ return slotConnectExternal_; }		// Switch:  External connected to bus
	bco::slot<bool>&	FuelCellConnectSlot()			{ return slotConnectFuelCell_; }		// Switch:  Fuelcell connected to bus
	bco::slot<double>&	AmpDrawSlot()					{ return slotAmpDraw_; }				// Receives and combines amp draw signals.
	bco::slot<double>&	FuelCellAvailablePowerSlot()	{ return slotFuelCellAvailablePower_; }	// Volt quantity available from fuelcell.

	// Outputs:
	bco::signal<double>&	VoltLevelSignal()			{ return signalVoltLevel_; }			// Current volts available
	bco::signal<double>&	AmpLoadSignal()				{ return signalAmpLoad_; }				// Current amp usage
	bco::signal<bool>&		ExternalAvailableSignal()	{ return signalExternalAvailable_; }	// Is external connection available (drives ext light)
	bco::signal<bool>&		ExternalConnectedSignal()	{ return signalExternalConnected_; }	// Is External connection providing power to bus (light)
	bco::signal<bool>&		FuelCellConnectedSignal()	{ return signalFuelCellConnected_; }	// Is fuelcell providing power to bus (light)
	bco::signal<bool>&		IsDrawingBatterySignal()	{ return signalIsDrawingBattery_; }		// Is the battery being used.

	// Todo:
	// Battery level signal

private:
	void Update(bco::BaseVessel& vessel);

	const double			FULL_POWER		=  28.0;
	const double			USEABLE_POWER	=  24.0;
	const double			AMP_OVERLOAD	= 100.0;


	bco::signal<double>		signalVoltLevel_;
	bco::signal<double>		signalAmpLoad_;
	bco::signal<bool>		signalExternalAvailable_;
	bco::signal<bool>		signalExternalConnected_;
	bco::signal<bool>		signalFuelCellConnected_;
	bco::signal<bool>		signalIsDrawingBattery_;

	// Slots for the on / off switches.
	bco::slot<bool>			slotIsEnabled_;				// Input for the main power switch
	bco::slot<bool>			slotConnectExternal_;
	bco::slot<bool>			slotConnectFuelCell_;
	bco::slot<double>		slotFuelCellAvailablePower_;

	bco::slot<double>		slotAmpDraw_;				// Components call this to report amp draw, will be summed over a step.
	double					ampDraw_{ 0.0 };			// Collects the total amps drawn during a step.
	double					batteryLevel_;

	std::vector<bco::power_consumer*>	power_users_;
};