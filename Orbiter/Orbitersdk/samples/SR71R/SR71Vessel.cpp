//	SR-71r Orbiter Addon
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

#include "StdAfx.h"

#include "SR71Vessel.h"
#include "LiftCoeff.h"
#include "ShipMets.h"

SR71Vessel::SR71Vessel(OBJHANDLE hvessel, int flightmodel) : 
bco::BaseVessel(hvessel, flightmodel),
meshVirtualCockpit_(nullptr),
apu_(                   this,   APU_AMPS),
//autoPilot_(             this,   AUTOP_AMPS),
avionics_(              this,   AVIONICS_AMPS),
cargoBayController_(    this,   CARGO_AMPS),
canopy_(                this,   CANOPY_AMPS),
fuelCell_(              this,   FUELCELL_AMPS),
headsUpDisplay_(        this,   HUD_AMPS),
hydrogenTank_(          this,   HYDRO_SUPPLY),
landingGear_(           this),
mfdLeft_(               this,   MFD_AMPS),
mfdRight_(              this,   MFD_AMPS),
navModes_(              this,   NAV_AMPS),
oxygenTank_(            this,   O2_SUPPLY),
powerSystem_(           this),
propulsionController_(  this,   PROPULS_AMPS),
rcsSystem_(             this,   RCS_AMPS),
surfaceControl_(        this),
statusBoard_(           this,   STATUS_AMPS),
airBrake_(              this),
lights_(                this,   LIGHTS_AMPS),
clock_(                 this),
shutters_(              this),
computer_(              this,   COMPUTER_AMPS),
hoverEngines_(          this,   HOVER_AMPS),
retroEngines_(          this,   RETRO_AMPS)
{
	RegisterComponent(&apu_);
	RegisterComponent(&avionics_);
	RegisterComponent(&cargoBayController_);
    RegisterComponent(&canopy_);
	RegisterComponent(&fuelCell_);
	RegisterComponent(&headsUpDisplay_);
	RegisterComponent(&hydrogenTank_);
	RegisterComponent(&landingGear_);
	RegisterComponent(&mfdLeft_);
	RegisterComponent(&mfdRight_);
	RegisterComponent(&navModes_);
	RegisterComponent(&oxygenTank_);
	RegisterComponent(&powerSystem_);
	RegisterComponent(&propulsionController_);
	RegisterComponent(&rcsSystem_);
	RegisterComponent(&surfaceControl_);
	RegisterComponent(&statusBoard_);
	RegisterComponent(&airBrake_);
	RegisterComponent(&lights_);
	RegisterComponent(&clock_);
	RegisterComponent(&shutters_);
	RegisterComponent(&computer_);
    RegisterComponent(&hoverEngines_);
    RegisterComponent(&retroEngines_);

	// Add Controls
	/*  TOGGLE SWITCHES */
	AddControl(&toggleNavigationLights);
	AddControl(&toggleMainPower);
	AddControl(&togglePowerConnectionExternal);
	AddControl(&togglePowerConnectionFuelCell);

	/*  POWER LIGHTS  */
	AddControl(&lightFuelCellAvail_);
	AddControl(&lightExternalAvail_);
	AddControl(&lightFuelCellConnected_);
	AddControl(&lightExternalConnected_);

	/*  Gauges  */
	AddControl(&gaugePowerVolts_);
	AddControl(&gaugePowerAmps_);

	//
	AddComponent(&lightNav_);

	// These are here because the template deduction does not seem to work in the header file.
	// These objects will die at the end of this method, but they will have done their job.

	bco::Connector  navLightSwitch	{ toggleNavigationLights.StateSignal(),			lightNav_.Slot() };

	// Power connections
	bco::Connector	pwrMain			{ toggleMainPower.StateSignal(),				powerSystem_.SlotMainPower() };
	bco::Connector	pwrExtConn		{ togglePowerConnectionExternal.StateSignal(),	powerSystem_.SlotExternalConnect() };
	bco::Connector	pwrFCConn		{ togglePowerConnectionFuelCell.StateSignal(),	powerSystem_.SlotFuelCellConnect() };
	
	// Fuel cell
	bco::Connector  vcPower			{ fuelCell_.SignalAvailablePower(),				powerSystem_.SlotFuelCellAvailablePower()};

	// Power gauges
	bco::Connector	gaVolts			{ powerSystem_.SignalVoltLevel(),				gaugePowerVolts_.State() };
	bco::Connector	gaAmps			{ powerSystem_.SignalAmpLevel(),				gaugePowerAmps_.State() };

	// Power status lights
	bco::Connector	lgtFCAvail		{ powerSystem_.SignalFuelCellAvailable(),		lightFuelCellAvail_.SlotState() };
	bco::Connector	lgtFCConnect	{ powerSystem_.SignalFuelCellConnected(),		lightFuelCellConnected_.SlotState() };
	bco::Connector	lgtExtAvail		{ powerSystem_.SignalExternalAvailable(),		lightExternalAvail_.SlotState() };
	bco::Connector	lgtExtConnect	{ powerSystem_.SignalExternalConnected(),		lightExternalConnected_.SlotState() };
}


SR71Vessel::~SR71Vessel()
{
}

void SR71Vessel::SetupVesselComponents()
{
	// Setup surface controller:
	surfaceControl_.SetAPU(&apu_);

	// APU
	apu_.SetPropulsionControl(&propulsionController_);

	// Landing gear
	landingGear_.SetAPU(&apu_);

	// Fuelcell
	fuelCell_.SetHydrogenSytem(&hydrogenTank_);
	fuelCell_.SetOxygenSystem(&oxygenTank_);
	fuelCell_.SetPowerSystem(&powerSystem_);

	// Status board
	statusBoard_.SetCargoBay(&cargoBayController_);
	statusBoard_.SetAvionics(&avionics_);
	statusBoard_.SetPower(&powerSystem_);
	statusBoard_.SetPropulsion(&propulsionController_);
	statusBoard_.SetAPU(&apu_);
	statusBoard_.SetAirBrake(&airBrake_);
    statusBoard_.SetHover(&hoverEngines_);
    statusBoard_.SetRetro(&retroEngines_);
    statusBoard_.SetCanopy(&canopy_);

	// Airbrake
	airBrake_.SetAPU(&apu_);

    // Flight Computer
    computer_.SetAvionics(&avionics_);
	computer_.SetSurfaceControl(&surfaceControl_);
	computer_.SetPropulsionControl(&propulsionController_);



	// Setup power and add powered devices:
	powerSystem_.SetFuelCell(&fuelCell_);

	powerSystem_.AddMainCircuitDevice(&avionics_);
	powerSystem_.AddMainCircuitDevice(&headsUpDisplay_);
	powerSystem_.AddMainCircuitDevice(&rcsSystem_);
	powerSystem_.AddMainCircuitDevice(&navModes_);
	powerSystem_.AddMainCircuitDevice(&mfdLeft_);
	powerSystem_.AddMainCircuitDevice(&mfdRight_);
	powerSystem_.AddMainCircuitDevice(&cargoBayController_);
    powerSystem_.AddMainCircuitDevice(&canopy_);
	powerSystem_.AddMainCircuitDevice(&apu_);
	powerSystem_.AddMainCircuitDevice(&fuelCell_);
	powerSystem_.AddMainCircuitDevice(&statusBoard_);
	powerSystem_.AddMainCircuitDevice(&propulsionController_);
	powerSystem_.AddMainCircuitDevice(&lights_);
	powerSystem_.AddMainCircuitDevice(&computer_);
    powerSystem_.AddMainCircuitDevice(&hoverEngines_);
    powerSystem_.AddMainCircuitDevice(&retroEngines_);
    powerSystem_.AddMainCircuitDevice(&oxygenTank_);
    powerSystem_.AddMainCircuitDevice(&hydrogenTank_);
}

void SR71Vessel::SetupAerodynamics()
{
	// Aerodynamics - see notes in ShipMets file.
	CreateAirfoil3(
		LIFT_VERTICAL, 
		_V(0, 0, -0.3), 
		VLiftCoeff, 
		0, 
		VERT_WING_CHORD, 
		VERT_WING_AREA, 
		VERT_WING_AR);

	CreateAirfoil3(
		LIFT_HORIZONTAL, 
		_V(0, 0, -4), 
		HLiftCoeff, 
		0, 
		HORZ_WING_CHORD,
		HORZ_WING_AREA,
		HORZ_WING_AR);
}

