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
#include "bc_orbiter\OnOffSwitch.h"
#include "bc_orbiter\Circuit.h"
#include "bc_orbiter\VCGauge.h"
#include "bc_orbiter\VCToggleSwitch.h"
#include "bc_orbiter\signals.h"

#include "FuelCell.h"
#include "SR71r_mesh.h"

#include <map>

namespace bco = bc_orbiter;

class FuelCell;

/**	Power System
	Models the main power circuit for the ship.  Contains a 'Circuit' class which all of 
	the powered components of the ship are attached to.  At each step the circuit is queried
	to determine what the current amp draw is.  Changes to the power availability are also
	communicated to the components via the circuit.

	Three energy sources are available:
	- External: when parked on the ground.
	- Fuel cell:
	- Backup battery: <not imlemented>
		Battery - 50 amp hour should last 40 minutes with nothing on.

	Enable power:
	To enable power you need to attach an available power source to the main circuit.  For
	external and fuel cell, there is an 'AVAIL' light that will come on when that power
	source is available.  The backup battery will engage with the main power switch is on 
	but no power source is attached to the main circuit.  The [BATT] light will illuminate
	in the message panel when power is being drawn off of the backup battery.

	Configuration:
	POWER a b c d e
	a - Main power switch			0/1 - 0ff/on.
	b - External power attached		0/1 - off/attached.
	c - Fuel cell attached			0/1 - off/attached.
	d - Buss voltage				double 27.0
	e - Battery level				double (0 - 1)
*/
class PowerSystem :	public bco::PoweredComponent
{
public:
	PowerSystem(bco::BaseVessel* vessel);

	virtual bool OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	virtual void OnSaveConfiguration(FILEHANDLE scn) const override;

//	double GetAmpDraw()								{ return mainCircuit_.GetTotalAmps(); }	// TODO: Make signal

	bco::signal<double>& VoltLevelSignal()			{ return signalVoltLevel_; }			// Changes in volt level
	bco::signal<double>& AmpLoadSignal()			{ return signalAmpLoad_; }				// Changes in amp load 
	bco::signal<bool>& ExternalAvailableSignal()	{ return signalExternalAvailable_; }	// External resources available
	bco::signal<bool>& ExternalConnectedSignal()	{ return signalExternalConnected_; }	// External connection status
	bco::signal<bool>& FuelCellConnectedSignal()	{ return signalFuelCellConnected_; }	// Fuelcell connection status
	bco::signal<bool>& IsDrawingBatterSignal()		{ return signalIsDrawingBattery_; }		// Are we on batter power?

	// These slots driven by toggle switches.
	bco::slot<bool>& MainPowerSlot()				{ return slotIsEnabled_; }				// Main power switch
	bco::slot<bool>& ExternalConnectSlot()			{ return slotConnectExternal_; }		// External connect switch
	bco::slot<bool>& FuelCellConnectSlot()			{ return slotConnectFuelCell_; }		// Fuelcell connect switch
	bco::slot<double>& AmpDrawSlot()				{ return slotAmpDraw_; }				// Recieve amp draw signals.

	bco::slot<double>& FuelCellAvailablePowerSlot() { return slotFuelCellAvailablePower_; }	// Availability of fuelcell power

	void PostAmpStep() { 
		signalAmpLoad_.fire(ampDraw_);	// Should be the sum of all component amp draws for the current step.
		Update();
		ampDraw_ = 0.0; 
	}

private:
	void Update();

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

//	bco::Circuit			mainCircuit_;

	// Power ports:
//	double					powerExternal_;
	const char*				ConfigKey = "POWER";

	double					batteryLevel_;
};