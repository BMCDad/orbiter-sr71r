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
//avionics_(              this,   AVIONICS_AMPS),
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
//lights_(                this,   LIGHTS_AMPS),  TODO
clock_(                 this),
shutters_(              this),
computer_(              this,   COMPUTER_AMPS),
hoverEngines_(this, HOVER_AMPS),
retroEngines_(this, RETRO_AMPS)
{
	RegisterComponent(&apu_);
//	RegisterComponent(&avionics_);
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
	RegisterComponent(&clock_);
	RegisterComponent(&shutters_);
	RegisterComponent(&computer_);
	RegisterComponent(&hoverEngines_);
	RegisterComponent(&retroEngines_);

	// Add Controls
	/*  TOGGLE SWITCHES */
	AddControl(&switchMainPower);
	AddControl(&switchConnectExternalPower);
	AddControl(&switchConnectFuelCell);

	/*  LIGHT SWITCHES  */
	AddControl(&switchNavigationLights);
	AddControl(&switchBeaconLights);
	AddControl(&switchStrobeLights);

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
	AddControl(&btnLightHudDocking_);
	AddControl(&btnHudSurface_);
	AddControl(&btnLightHudSurface_);
	AddControl(&btnHudOrbit_);
	AddControl(&btnLightHudOrbit_);

	/*  Hydrogen tank  */
	AddControl(&gaugeHydroLevel_);
	AddControl(&lightHydrogenAvail_);
	AddControl(&btnHydroFill_);
	AddControl(&btnLightHydroFill_);

	/*  LOX tank  */
	AddControl(&gaugeO2Level_);
	AddControl(&lightO2Avail_);
	AddControl(&btnO2Fill_);
	AddControl(&btnLightO2Fill);

	/*  Airbrake  */
	AddControl(&btnDecreaseAirbrake_);
	AddControl(&btnIncreaseAirbrake_);

	/*  Landing Gear  */
	AddControl(&btnLowerLandingGear_);
	AddControl(&btnRaiseLandingGear_);

	/*  Nav Modes  */
	AddControl(&btnNavKillRot_);
	AddControl(&btnLightNavKillRot_);
	AddControl(&btnNavHorzLevel_);
	AddControl(&btnLightNavHorzLevel_);
	AddControl(&btnNavPrograde_);
	AddControl(&btnLightNavPrograde_);
	AddControl(&btnNavRetrograde_);
	AddControl(&btnLightNavRetrograde_);
	AddControl(&btnNavNormal_);
	AddControl(&btnLightNavNormal_);
	AddControl(&btnNavAntiNorm_);
	AddControl(&btnLightNavAntiNorm_);

	/*  Propulsion  */
	AddControl(&gaugeFuelFlow_);
	AddControl(&gaugeFuelMain_);
	AddControl(&gaugeFuelRCS_);
	AddControl(&switchThrustLimit_);
	AddControl(&switchFuelDump_);
	AddControl(&switchTransferSelect_);
	AddControl(&lightFuelAvail_);
	AddControl(&btnFuelTransferPump_);
	AddControl(&btnLightFuelTransferPump_);
	AddControl(&btnFuelValveOpen_);
	AddControl(&btnLightFuelValveOpen_);

	/*  RCS  */
	AddControl(&btnRCSLinear_);
	AddControl(&btnLightRCSLinear_);
	AddControl(&btnRCSRotate_);
	AddControl(&btnLightRCSRotate_);

	/*  Retro Doors  */
	AddControl(&switchRetroDoors_);

	/*  Shutters  */
	AddControl(&switchShutters_);

	/*  Avionics  */
	AddControl(&switchAvionMode_);
	AddControl(&switchAvionPower_);
	AddControl(&switchNavMode_);

	/*  Altimeter  */
	AddControl(&altimeter1Hand_);
	AddControl(&altimeter10Hand_);
	AddControl(&altimeter100Hand_);

	AddControl(&tdiAltOnes_);
	AddControl(&tdiAltTens_);
	AddControl(&tdiAltHunds_);
	AddControl(&tdiAltThou_);
	AddControl(&tdiAltTenThou_);

	/*  VSI  */
	AddControl(&vsiHand_);

	/*  Attitude  */
	AddControl(&attitudeDisplay_);
	AddControl(&attitudeFlag_);

	/*  HSI  */
	AddControl(&hsiRoseCompass_);
	AddControl(&hsiHeadingBug_);
	AddControl(&hsiCourse_);
	AddControl(&dialSetCourseIncrement_);
	AddControl(&dialSetCourseDecrement_);
	AddControl(&hsiCRSOnes_);
	AddControl(&hsiCRSTens_);
	AddControl(&hsiCRSHunds_);
	AddControl(&dialSetHeadingIncrement_);
	AddControl(&dialSetHeadingDecrement_);
	AddControl(&hsiBearing_);
	AddControl(&hsiMilesOnes_);
	AddControl(&hsiMilesTens_);
	AddControl(&hsiMilesHunds_);
	AddControl(&comStatusFlag_);
	AddControl(&hsiOffFlag_);
	AddControl(&hsiExoFlag_);

	/*  Airspeed  */
	AddControl(&airspeedKeasOnes_);
	AddControl(&airspeedKeasTens_);
	AddControl(&airspeedKeasHunds_);
	AddControl(&airspeedSpeed_);
	AddControl(&airspeedKies_);
	AddControl(&airspeedMaxMach_);
	AddControl(&speedIsEnabledFlag_);
	AddControl(&speedIsVelocityFlag_);
	AddControl(&airspeedMACHOnes_);
	AddControl(&airspeedMACHTens_);
	AddControl(&airspeedMACHHunds_);


	//
	AddComponent(&navLight_);
	AddComponent(&beacon_);
	AddComponent(&strobe_);
	AddComponent(&altimeter_);
	//AddComponent(&vsi_);
	//AddComponent(&attitude_);
	AddComponent(&hsi_);
	AddComponent(&aeroData_);

	// These are here because the template deduction does not seem to work in the header file.
	// These objects will die at the end of this method, but they will have done their job.

	//bco::connector  navLightSwitch	{ switchNavigationLights.Signal(),				navLight_.Slot() };

	// Lights
	bco::connect( switchNavigationLights.Signal(),			navLight_.Slot());
	bco::connect( switchBeaconLights.Signal(),				beacon_.Slot());
	bco::connect( switchStrobeLights.Signal(),				strobe_.Slot());

	// Power connections
	bco::connect( switchMainPower.Signal(),					powerSystem_.MainPowerSlot());
	bco::connect( switchConnectExternalPower.Signal(),		powerSystem_.ExternalConnectSlot());
	bco::connect( switchConnectFuelCell.Signal(),			powerSystem_.FuelCellConnectSlot());
	bco::connect( powerSystem_.FuelCellConnectedSignal(),	lightFuelCellConnected_.Slot());
	bco::connect( powerSystem_.ExternalAvailableSignal(),	lightExternalAvail_.Slot());
	bco::connect( powerSystem_.ExternalConnectedSignal(),	lightExternalConnected_.Slot());
	
	// Fuel cell					// A signal can drive more then one slot
	bco::connect( fuelCell_.AvailablePowerSignal(),			powerSystem_.FuelCellAvailablePowerSlot());
	bco::connect( fuelCell_.IsAvailableSignal(),			lightFuelCellAvail_.Slot());
	bco::connect( switchFuelCellPower.Signal(),				fuelCell_.IsEnabledSlot());
	bco::connect( powerSystem_.AmpLoadSignal(),				fuelCell_.AmpLoadSlot());

	// Power gauges
	bco::connect( powerSystem_.VoltLevelSignal(),			gaugePowerVolts_.Slot());
	bco::connect( powerSystem_.AmpLoadSignal(),				gaugePowerAmps_.Slot());

	// Canopy
	bco::connect( switchCanopyPower.Signal(),				canopy_.PowerEnabledSlot());
	bco::connect( switchCanopyOpen.Signal(),				canopy_.CanopyOpenSlot());

	// Cargo
	bco::connect( switchCargoPower.Signal(),				cargoBayController_.PowerEnabledSlot());
	bco::connect( switchCargoOpen.Signal(),					cargoBayController_.CargoOpenSlot());

	// APU
	bco::connect( switchApuPower.Signal(),					apu_.IsEnabledSlot());
	bco::connect( apu_.HydroPressSignal(),					gaugeAPULevel_.Slot());

	// Clock
	bco::connect( clock_.TimerSecondsSignal(),				clockTimerSecondsHand_.Slot());
	bco::connect( clock_.TimerMinutesSignal(),				clockTimerMinutesHand_.Slot());
	bco::connect( clock_.ElapsedMinutesSignal(),			clockElapsedMinutesHand_.Slot());
	bco::connect( clock_.ElapsedHoursSignal(),				clockElapsedHoursHand_.Slot());

	// Hover doors
	bco::connect( switchHoverOpen.Signal(),					hoverEngines_.HoverOpenSlot());

	// HUD
	bco::connect( btnHudDocking_.Signal(),					headsUpDisplay_.DockModeSlot());
	bco::connect( headsUpDisplay_.DockModeSignal(),			btnLightHudDocking_.Slot());
	bco::connect( btnHudSurface_.Signal(),					headsUpDisplay_.SurfaceModeSlot());
	bco::connect( headsUpDisplay_.SurfaceModeSignal(),		btnLightHudSurface_.Slot());
	bco::connect( btnHudOrbit_.Signal(),					headsUpDisplay_.OrbitModeSlot());
	bco::connect( headsUpDisplay_.OrbitModeSignal(),		btnLightHudOrbit_.Slot());

	// Hydrogen
	bco::connect( hydrogenTank_.LevelSignal(),				gaugeHydroLevel_.Slot());
	bco::connect( powerSystem_.ExternalAvailableSignal(),	hydrogenTank_.ExternAvailableSlot());
	bco::connect( hydrogenTank_.IsAvailableSignal(),		lightHydrogenAvail_.Slot());
	bco::connect( btnHydroFill_.Signal(),					hydrogenTank_.ToggleFillSlot());
	bco::connect( hydrogenTank_.IsFillingSignal(),			btnLightHydroFill_.Slot());

	// Oxygen
	bco::connect( oxygenTank_.LevelSignal(),				gaugeO2Level_.Slot());
	bco::connect( powerSystem_.ExternalAvailableSignal(),	oxygenTank_.ExternAvailableSlot());
	bco::connect( oxygenTank_.IsAvailableSignal(),			lightO2Avail_.Slot());
	bco::connect( btnO2Fill_.Signal(),						oxygenTank_.ToggleFillSlot());
	bco::connect( oxygenTank_.IsFillingSignal(),			btnLightO2Fill.Slot());

	// Airbrake
	bco::connect( btnDecreaseAirbrake_.Signal(),			airBrake_.DecreaseSlot());
	bco::connect( btnIncreaseAirbrake_.Signal(),			airBrake_.IncreaseSlot());
	bco::connect( apu_.HydroPressSignal(),					airBrake_.HydraulicPressSlot());

	// Landing Gear
	bco::connect( btnRaiseLandingGear_.Signal(),			landingGear_.GearUpSlot());
	bco::connect( btnLowerLandingGear_.Signal(),			landingGear_.GearDownSlot());
	bco::connect( apu_.HydroPressSignal(),					landingGear_.HydraulicPressSlot());

	// Nav Modes
	bco::connect( btnNavKillRot_.Signal(),					navModes_.IsKillRotSlot());
	bco::connect( navModes_.IsKillRotSignal(),				btnLightNavKillRot_.Slot());
	bco::connect( btnNavHorzLevel_.Signal(),				navModes_.IsHorzLevelSlot());
	bco::connect( navModes_.IsHorzLevelSignal(),			btnLightNavHorzLevel_.Slot());
	bco::connect( btnNavPrograde_.Signal(),					navModes_.IsProgradeSlot());
	bco::connect( navModes_.IsProgradeSignal(),				btnLightNavPrograde_.Slot());
	bco::connect( btnNavRetrograde_.Signal(),				navModes_.IsRetroGradeSlot());
	bco::connect( navModes_.IsRetroGradeSignal(),			btnLightNavRetrograde_.Slot());
	bco::connect( btnNavNormal_.Signal(),					navModes_.IsNormalSlot());
	bco::connect( navModes_.IsNormalSignal(),				btnLightNavNormal_.Slot());
	bco::connect( btnNavAntiNorm_.Signal(),					navModes_.IsAntiNormalSlot());
	bco::connect( navModes_.IsAntiNormalSignal(),			btnLightNavAntiNorm_.Slot());

	// Propulsion
	bco::connect( propulsionController_.FuelFlowSignal(),		gaugeFuelFlow_.Slot());
	bco::connect( propulsionController_.MainFuelLevelSignal(),	gaugeFuelMain_.Slot());
	bco::connect( propulsionController_.RCSFuelLevelSignal(),	gaugeFuelRCS_.Slot());
	bco::connect( switchThrustLimit_.Signal(),					propulsionController_.ThrottleLimitSlot());
	bco::connect( switchFuelDump_.Signal(),						propulsionController_.FuelDumpSlot());
	bco::connect( switchTransferSelect_.Signal(),				propulsionController_.FuelTransferSelSlot());
	bco::connect( propulsionController_.IsFuelAvailSignal(),	lightFuelAvail_.Slot());
	bco::connect( btnFuelTransferPump_.Signal(),				propulsionController_.TransferPumpSlot());
	bco::connect( propulsionController_.IsTransferPumpOn(),		btnLightFuelTransferPump_.Slot());
	bco::connect( btnFuelValveOpen_.Signal(),					propulsionController_.FuelValveOpenSlot());
	bco::connect( propulsionController_.IsFuelValveOpen(),		btnLightFuelValveOpen_.Slot());

	// RCS
	bco::connect( btnRCSLinear_.Signal(),					rcsSystem_.ToggleLinearSlot());
	bco::connect( rcsSystem_.IsLinearSignal(),				btnLightRCSLinear_.Slot());
	bco::connect( btnRCSRotate_.Signal(),					rcsSystem_.ToggleRotateSlot());
	bco::connect( rcsSystem_.IsRotateSignal(),				btnLightRCSRotate_.Slot());

	// Retro Doors
	bco::connect( switchRetroDoors_.Signal(),				retroEngines_.RetroDoorsSlot());

	// Altimeter
	bco::connect( switchAvionPower_.Signal(),				altimeter_.EnabledSlot());
	bco::connect( switchAvionMode_.Signal(),				altimeter_.AvionicsModeSlot());
	bco::connect( altimeter_.AltimeterHundredsSignal(),		altimeter1Hand_.Slot());
	bco::connect( altimeter_.AltimeterThousandsSignal(),	altimeter10Hand_.Slot());
	bco::connect( altimeter_.AltimeterTenThousandsSignal(),	altimeter100Hand_.Slot());

	bco::connect( altimeter_.AltimeterTensSignal(),			tdiAltOnes_.SlotTransform());
	bco::connect( altimeter_.AltimeterHundredsSignal(),		tdiAltTens_.SlotTransform());
	bco::connect( altimeter_.AltimeterThousandsSignal(),	tdiAltHunds_.SlotTransform());
	bco::connect( altimeter_.AltimeterTenThousandsSignal(),	tdiAltThou_.SlotTransform());
	bco::connect( altimeter_.AltimeterHundThousandsSignal(),tdiAltTenThou_.SlotTransform());

	bco::connect( altimeter_.IsExoModeSignal(),				altimeterExoModeFlag_.Slot());
	bco::connect( altimeter_.IsEnabledSignal(),				altimeterEnabledFlag_.Slot());

	// VSI
//	bco::connect( switchAvionPower_.Signal(),				vsi_.EnabledSlot());
//	bco::connect( vsi_.VSINeedleSignal(),					vsiHand_.Slot());
	bco::connect( switchAvionPower_.Signal(),				vsiActiveFlag_.Slot());

	// Attitude
//	bco::connect( switchAvionPower_.Signal(),				attitude_.EnabledSlot());
//	bco::connect( attitude_.BankSignal(),					attitudeDisplay_.SlotAngle());
//	bco::connect( attitude_.PitchSignal(),					attitudeDisplay_.SlotTransform());
	bco::connect( switchAvionPower_.Signal(),				attitudeFlag_.Slot());

	// HSI
	bco::connect( switchAvionPower_.Signal(),				hsi_.EnabledSlot());
	bco::connect( switchAvionMode_.Signal(),				hsi_.AvionicsModeSlot());
	bco::connect( hsi_.YawSignal(),							hsiRoseCompass_.Slot());
	bco::connect( hsi_.SetHeadingSignal(),					hsiHeadingBug_.Slot());
	bco::connect( hsi_.BearingSignal(),						hsiBearing_.Slot());
	bco::connect( hsi_.ComStatusSignal(),					comStatusFlag_.Slot());
	bco::connect( hsi_.ShowOffFlagSignal(),					hsiOffFlag_.Slot());
	bco::connect( hsi_.ShowExoFlagSignal(),					hsiExoFlag_.Slot());

			// Course/heading dials drive the aerodata course slots.  Source of 'truth' is in AeroData.
	bco::connect( dialSetCourseIncrement_.Signal(),			aeroData_.SetCourseIncSlot());
	bco::connect( dialSetCourseDecrement_.Signal(),			aeroData_.SetCourseDecSlot());
	bco::connect( dialSetHeadingIncrement_.Signal(),		aeroData_.SetHeadingIncSlot());
	bco::connect( dialSetHeadingDecrement_.Signal(),		aeroData_.SetHeadingDecSlot());
	// ...which in turn drive the HSI course and heading
	bco::connect( aeroData_.SetCourseSignal(),				hsi_.SetCourseSlot());
	bco::connect( aeroData_.SetHeadingSignal(),				hsi_.SetHeadingSlot());
			// ...which drives the course and heading needle and wheels.
	bco::connect( hsi_.SetCourseSignal(),					hsiCourse_.Slot());
	bco::connect( hsi_.SetCourseSignal(),					hsiCourseError_.SlotAngle());
	bco::connect( hsi_.NavErrorSignal(),					hsiCourseError_.SlotTransform());
	bco::connect( hsi_.CrsOnesSignal(),						hsiCRSOnes_.SlotTransform());
	bco::connect( hsi_.CrsTensSignal(),						hsiCRSTens_.SlotTransform());
	bco::connect( hsi_.CrsHundsSignal(),					hsiCRSHunds_.SlotTransform());
	bco::connect( hsi_.MilesOnesSignal(),					hsiMilesOnes_.SlotTransform());
	bco::connect( hsi_.MilesTensSignal(),					hsiMilesTens_.SlotTransform());
	bco::connect( hsi_.MilesHundsSignal(),					hsiMilesHunds_.SlotTransform());

	// Airspeed
	bco::connect( switchAvionPower_.Signal(),				airspeed_.EnabledSlot());
	bco::connect( switchAvionMode_.Signal(),				airspeed_.AvionicsModeSlot());
	bco::connect( airspeed_.KeasOnesSignal(),				airspeedKeasOnes_.SlotTransform());
	bco::connect( airspeed_.KeasTensSignal(),				airspeedKeasTens_.SlotTransform());
	bco::connect( airspeed_.KeasHundsSignal(),				airspeedKeasHunds_.SlotTransform());
	bco::connect( airspeed_.MACHMaxSignal(),				airspeedMaxMach_.Slot());
	bco::connect( airspeed_.SpeedSignal(),					airspeedSpeed_.Slot());
	bco::connect( airspeed_.KiasSpeedSignal(),				airspeedKies_.Slot());
	bco::connect( airspeed_.IsEnabledSignal(),				speedIsEnabledFlag_.Slot());
	bco::connect( airspeed_.IsVelocityFlagSignal(),			speedIsVelocityFlag_.Slot());
	bco::connect( airspeed_.MACHOnesSignal(),				airspeedMACHOnes_.SlotTransform());
	bco::connect( airspeed_.MACHTensSignal(),				airspeedMACHTens_.SlotTransform());
	bco::connect( airspeed_.MACHHundsSignal(),				airspeedMACHHunds_.SlotTransform());
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

	// Fuelcell
	fuelCell_.SetHydrogenSytem(&hydrogenTank_);
	fuelCell_.SetOxygenSystem(&oxygenTank_);

	// Status board
	statusBoard_.SetCargoBay(&cargoBayController_);
//	statusBoard_.SetAvionics(&avionics_);
	statusBoard_.SetPower(&powerSystem_);
	statusBoard_.SetPropulsion(&propulsionController_);
	statusBoard_.SetAPU(&apu_);
	statusBoard_.SetAirBrake(&airBrake_);
    statusBoard_.SetHover(&hoverEngines_);
    statusBoard_.SetRetro(&retroEngines_);
    statusBoard_.SetCanopy(&canopy_);

    // Flight Computer
//    computer_.SetAvionics(&avionics_);
	computer_.SetSurfaceControl(&surfaceControl_);
	computer_.SetPropulsionControl(&propulsionController_);



	// Setup power and add powered devices:
//	powerSystem_.AddMainCircuitDevice(&avionics_);
//	powerSystem_.AddMainCircuitDevice(&headsUpDisplay_);
//	powerSystem_.AddMainCircuitDevice(&rcsSystem_);
//	powerSystem_.AddMainCircuitDevice(&navModes_);
//	powerSystem_.AddMainCircuitDevice(&mfdLeft_);
//	powerSystem_.AddMainCircuitDevice(&mfdRight_);
//	powerSystem_.AddMainCircuitDevice(&cargoBayController_);
//    powerSystem_.AddMainCircuitDevice(&canopy_);
//	powerSystem_.AddMainCircuitDevice(&apu_);
//	powerSystem_.AddMainCircuitDevice(&fuelCell_);
//	powerSystem_.AddMainCircuitDevice(&statusBoard_);
//	powerSystem_.AddMainCircuitDevice(&propulsionController_);
////	powerSystem_.AddMainCircuitDevice(&lights_);		 // TODO
//	powerSystem_.AddMainCircuitDevice(&computer_);
//    powerSystem_.AddMainCircuitDevice(&hoverEngines_);
//    powerSystem_.AddMainCircuitDevice(&retroEngines_);
//    powerSystem_.AddMainCircuitDevice(&oxygenTank_);
//    powerSystem_.AddMainCircuitDevice(&hydrogenTank_);
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

