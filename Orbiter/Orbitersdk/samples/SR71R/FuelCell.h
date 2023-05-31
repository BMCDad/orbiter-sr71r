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

#include "bc_orbiter\OnOffSwitch.h"
#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\VCToggleSwitch.h"

#include "IConsumable.h"
#include "PowerSystem.h"
#include "SR71r_mesh.h"

class PowerSystem;

const double OXYGEN_BURN_RATE   = 0.00025;		// 2 lbs per hour @ 100 amps.  lbs->liter 0.453, so 0.906 per hour or 0.00025 per second.
const double HYDROGEN_BURN_RATE = 0.00004;		// 0.3 lbs per hour @ 100 amps.  

namespace bco = bc_orbiter;

/**	Fuel cell.
	Models the plane's fuel cell.

	The fuel cell uses oxygen and hydrogen to produce electricity.  The fuel cell requires the main curcuit
	to be powered in order to start, but can then power itself.  It has a fixed amp draw while operating.
	When operating the fuel call can provide 28 volts of power to the main circuit.

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
	public bco::PoweredComponent
{
public:
	FuelCell(bco::BaseVessel* vessel, double amps);

	virtual void OnSetClassCaps() override;
	virtual bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf) override { return false; }
	virtual bool OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	virtual void OnSaveConfiguration(FILEHANDLE scn) const override;

	bool OnLoadPanel2D(int id, PANELHANDLE hPanel) override;
	bool OnPanelMouseEvent(int id, int event) override;
	bool OnPanelRedrawEvent(int id, int event, SURFHANDLE surf) override;

	/**
		Draw down the oxygen and hydrogen levels based on the current amp load.
	*/
	void Step(double simt, double simdt, double mjd);
	virtual double CurrentDraw() override;

	virtual void ChangePowerLevel(double level) override
	{
		bco::PoweredComponent::ChangePowerLevel(level);
		SetIsFuelCellPowerAvailable(HasPower() && swPower_.IsOn());
	}


	bool IsFuelCellPowerAvailable()
	{
		return isFuelCellAvailable_; 
	}

	double AvailablePower() { return IsFuelCellPowerAvailable() ? 27.0 : 0.0; }
//	bco::OnOffSwitch&	PowerSwitch() { return swPower_; }


	void SetPowerSystem(	PowerSystem* ps)	{ powerSystem_ = ps; }
	void SetOxygenSystem(	IConsumable* os)	{ oxygenSystem_ = os; }
	void SetHydrogenSytem(	IConsumable* hs)	{ hydrogenSystem_ = hs; }

private:

	void SetIsFuelCellPowerAvailable(bool newValue);

	bool				isFuelCellAvailable_;
	double				availablePower_;

	PowerSystem*		powerSystem_;
	IConsumable*		oxygenSystem_;
	IConsumable*		hydrogenSystem_;

	const char*			ConfigKey = "FUELCELL";

	// Config members:

    bco::VCToggleSwitch swPower_        {   bm::vc::swFuelCellPower_id, 
                                            bm::vc::swFuelCellPower_location,  
                                            bm::vc::PowerTopRightAxis_location
                                        };

	const int ID_POWER = { GetBaseVessel()->GetIdForComponent(this) };
};