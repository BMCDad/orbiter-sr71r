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
hydrogenTank_(          this,   HYDRO_SUPPLY, HYDROGEN_FILL_RATE, HYDRO_NER, "HYDROGEN"),
landingGear_(           this),
mfdLeft_(               this,   MFD_AMPS),
mfdRight_(              this,   MFD_AMPS),
navModes_(              this,   NAV_AMPS),
oxygenTank_(            this,   O2_SUPPLY, OXYGEN_FILL_RATE, O2_NER, "OXYGEN"),
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
	AddControl(&switchNavigationLights);
	AddControl(&switchMainPower);
	AddControl(&switchConnectExternalPower);
	AddControl(&switchConnectFuelCell);

	/*  FUELCELL SWITCHES */
	AddControl(&switchFuelCellPower);

	/*  Canopy SWITCHES */
	AddControl(&switchCanopyPower);
	AddControl(&switchCanopyOpen);

	/*	APU Switch */
	AddControl(&switchApuPower);

	/*  Cargo SWITCHES */
	AddControl(&switchCargoPower);
	AddControl(&switchCargoOpen);

	/*  POWER LIGHTS  */
	AddControl(&lightFuelCellAvail_);
	AddControl(&lightExternalAvail_);
	AddControl(&lightFuelCellConnected_);
	AddControl(&lightExternalConnected_);

	/*  Gauges  */
	AddControl(&gaugePowerVolts_);
	AddControl(&gaugePowerAmps_);
	AddControl(&gaugeAPULevel_);

	/*  Clock  */
	AddControl(&clockTimerSecondsHand_);
	AddControl(&clockTimerMinutesHand_);
	AddControl(&clockElapsedMinutesHand_);
	AddControl(&clockElapsedHoursHand_);

	/*  Hover switch  */
	AddControl(&switchHoverOpen);

	/*  HUD  */
	AddControl(&btnHudDocking_);
	AddControl(&dspHudDocking_);
	AddControl(&btnHudSurface_);
	AddControl(&dspHudSurface_);
	AddControl(&btnHudOrbit_);
	AddControl(&dspHudOrbit_);

	/*  Hydrogen tank  */
	AddControl(&gaugeHydroLevel_);
	AddControl(&lightHydrogenAvail_);
	AddControl(&btnHydroFill_);
	AddControl(&dspHydroFill_);

	/*  LOX tank  */
	AddControl(&gaugeO2Level_);
	AddControl(&lightO2Avail_);
	AddControl(&btnO2Fill_);
	AddControl(&dspO2Fill_);

	//
	AddComponent(&lightNav_);

	// These are here because the template deduction does not seem to work in the header file.
	// These objects will die at the end of this method, but they will have done their job.

	bco::connector  navLightSwitch	{ switchNavigationLights.Signal(),				lightNav_.Slot() };

	// Power connections
	bco::connector	pwrMain			{ switchMainPower.Signal(),						powerSystem_.MainPowerSlot() };
	bco::connector	pwrExtConn		{ switchConnectExternalPower.Signal(),			powerSystem_.ExternalConnectSlot() };
	bco::connector	pwrFCConn		{ switchConnectFuelCell.Signal(),				powerSystem_.FuelCellConnectSlot() };
	
	// Fuel cell					// A signal can drive more then one slot
	bco::connector  vcPower			{ fuelCell_.AvailablePowerSignal(),				powerSystem_.FuelCellAvailablePowerSlot()};
	bco::connector	lgtFCAvail		{ fuelCell_.IsAvailableSignal(),				lightFuelCellAvail_.Slot() };
	bco::connector  pwrFCMain		{ switchFuelCellPower.Signal(),					fuelCell_.MainPowerSlot() };
	bco::connector	fcAmpLoad		{ powerSystem_.AmpLoadSignal(),					fuelCell_.AmpLoadSlot() };

	// Power gauges
	bco::connector	gaVolts			{ powerSystem_.VoltLevelSignal(),				gaugePowerVolts_.Slot() };
	bco::connector	gaAmps			{ powerSystem_.AmpLoadSignal(),					gaugePowerAmps_.Slot() };

	// Power status lights
	bco::connector	lgtFCConnect	{ powerSystem_.FuelCellConnectedSignal(),		lightFuelCellConnected_.Slot() };
	bco::connector	lgtExtAvail		{ powerSystem_.ExternalAvailableSignal(),		lightExternalAvail_.Slot() };
	bco::connector	lgtExtConnect	{ powerSystem_.ExternalConnectedSignal(),		lightExternalConnected_.Slot() };

	// Canopy
	bco::connector	cpPwr			{ switchCanopyPower.Signal(),					canopy_.PowerEnabledSlot() };
	bco::connector  cpOpen			{ switchCanopyOpen.Signal(),					canopy_.CanopyOpenSlot() };

	// Cargo
	bco::connector	cgPwr			{ switchCargoPower.Signal(),					cargoBayController_.PowerEnabledSlot() };
	bco::connector  cgOpen			{ switchCargoOpen.Signal(),						cargoBayController_.CargoOpenSlot() };

	// APU
	bco::connector	apuPwr			{ switchApuPower.Signal(),						apu_.APUPowerSlot() };
	bco::connector  apuGa			{ apu_.HydroPressSignal(),						gaugeAPULevel_.Slot() };

	// Clock
	bco::connector clkTS			{ clock_.TimerSecondsSignal(),					clockTimerSecondsHand_.Slot() };
	bco::connector clkTM			{ clock_.TimerMinutesSignal(),					clockTimerMinutesHand_.Slot() };
	bco::connector clkEM			{ clock_.ElapsedMinutesSignal(),				clockElapsedMinutesHand_.Slot() };
	bco::connector clkEH			{ clock_.ElapsedHoursSignal(),					clockElapsedHoursHand_.Slot() };

	// Hover doors
	bco::connector hvD				{ switchHoverOpen.Signal(),						hoverEngines_.HoverOpenSlot() };

	// HUD
	bco::connector hdD				{ btnHudDocking_.Signal(),						headsUpDisplay_.DockModeSlot() };
	bco::connector hdDs				{ headsUpDisplay_.DockModeSignal(),				dspHudDocking_.Slot() };
	bco::connector hdO				{ btnHudSurface_.Signal(),						headsUpDisplay_.SurfaceModeSlot() };
	bco::connector hdOs				{ headsUpDisplay_.SurfaceModeSignal(),			dspHudSurface_.Slot() };
	bco::connector hdS				{ btnHudOrbit_.Signal(),						headsUpDisplay_.OrbitModeSlot() };
	bco::connector hdSs				{ headsUpDisplay_.OrbitModeSignal(),			dspHudOrbit_.Slot() };

	// Hydrogen
	bco::connector hy				{ hydrogenTank_.LevelSignal(),					gaugeHydroLevel_.Slot() };
	bco::connector hya				{ powerSystem_.ExternalAvailableSignal(),		hydrogenTank_.ExternAvailableSlot() };
	bco::connector hyb				{ hydrogenTank_.IsAvailableSignal(),			lightHydrogenAvail_.Slot() };
	bco::connector hdH				{ btnHydroFill_.Signal(),						hydrogenTank_.ToggleFillSlot()};
	bco::connector hdHs				{ hydrogenTank_.IsFillingSignal(),				dspHydroFill_.Slot()};

	// Oxygen
	bco::connector oy				{ oxygenTank_.LevelSignal(),					gaugeO2Level_.Slot() };
	bco::connector oya				{ powerSystem_.ExternalAvailableSignal(),		oxygenTank_.ExternAvailableSlot() };
	bco::connector oyb				{ oxygenTank_.IsAvailableSignal(),				lightO2Avail_.Slot() };
	bco::connector odH				{ btnO2Fill_.Signal(),							oxygenTank_.ToggleFillSlot() };
	bco::connector odHs				{ oxygenTank_.IsFillingSignal(),				dspO2Fill_.Slot() };
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

