#pragma once

#include "Orbitersdk.h"

#include "bc_orbiter\BaseVessel.h"
#include "bc_orbiter\on_off_display.h"
#include "bc_orbiter\on_off_input.h"
#include "bc_orbiter\rotary_display.h"
#include "bc_orbiter\simple_event.h"
#include "bc_orbiter\transform_display.h"
#include "bc_orbiter\flat_roll.h"

#include "ShipMets.h"
#include "SR71r_mesh.h"
//#include "Avionics.h"
#include "SurfaceController.h"
#include "CryogenicTank.h"
#include "PropulsionController.h"
#include "HUD.h"
#include "RCSSystem.h"
#include "NavModes.h"
#include "PowerSystem.h"
#include "LeftMFD.h"
#include "RightMfd.h"
#include "CargoBayController.h"
#include "Canopy.h"
#include "APU.h"
#include "LandingGear.h"
#include "FuelCell.h"
#include "StatusBoard.h"
#include "AirBrake.h"
#include "Clock.h"
#include "Shutters.h"
//#include "FlightComputer.h"
#include "VesselControl.h"
#include "HoverEngines.h"
#include "RetroEngines.h"
#include "NavLights.h"
#include "Beacon.h"
#include "Strobe.h"
#include "AeroData.h"
#include "Altimeter.h"
//#include "VSI.h"
//#include "AttitudeIndicator.h"
//#include "TestComponent.h"
#include "HSI.h"
#include "Airspeed.h"

#include <vector>
#include <map>


class SR71Vessel : public bco::BaseVessel
{
public:
	SR71Vessel(OBJHANDLE hvessel, int flightmodel);
	~SR71Vessel();

	// Callbacks in 
	virtual bool			clbkLoadVC(int id) override;
	virtual void			clbkSetClassCaps(FILEHANDLE cfg) override;

	void					clbkHUDMode(int mode);
	void					clbkRCSMode(int mode);
	void					clbkNavMode(int mode, bool active);
	void					clbkMFDMode(int mfd, int mode);
	int						clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);
    virtual void            clbkPostCreation() override;

	void					clbkPostStep(double simt, double simdt, double mjd);
	
	bool					clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

	bool					clbkLoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) override;

	// Setup
	void					SetupVesselComponents();
	void					SetupAerodynamics();

	MESHHANDLE				VCMeshHandle() { return vcMeshHandle_; }

	// Interfaces:
	PropulsionController*	GetPropulsionControl() { return &propulsionController_; }

private:
	UINT					mainMeshIndex_{ 0 };

	MESHHANDLE				vcMeshHandle_{ nullptr };
	DEVMESHHANDLE			meshVirtualCockpit_{ nullptr };


	// Components:
	APU						apu_;
//	AutoPilot				autoPilot_;
//	Avionics				avionics_;
	CargoBayController		cargoBayController_;
    Canopy                  canopy_;
	FuelCell				fuelCell_;
	HUD						headsUpDisplay_;
	CryogenicTank			hydrogenTank_;
	LandingGear				landingGear_;
	LeftMFD					mfdLeft_;
	RightMFD				mfdRight_;
	NavModes				navModes_;
	CryogenicTank			oxygenTank_;
	PowerSystem				powerSystem_;
	PropulsionController	propulsionController_;
	RCSSystem				rcsSystem_;
	SurfaceController		surfaceControl_;
	StatusBoard				statusBoard_;
	AirBrake				airBrake_;
	Clock					clock_;
	Shutters				shutters_;
//	FC::FlightComputer		computer_;
    HoverEngines            hoverEngines_;
    RetroEngines            retroEngines_;

	// DRAG
	double					bDrag{ 0.0 };
	// Collections:

	// TRY ADDING A NEW CONTROL


	// ** TOGGLE SWITCHES **

	/* control data for on/off up/down physical toggle switches */
	bco::on_off_input_meta toggleOnOff {
  		 1.5708,			// Rotation angle
		10.0,				// Anim speed
		 0.0,				// anim start
		 1.0,				// anim end
		 0.01,				// VC hit radius
		 0.0148,			// Panel offset
		PANEL_REDRAW_NEVER,	// vcRedrawFlags
		PANEL_MOUSE_LBDOWN, // vcMouseFlag
		PANEL_REDRAW_MOUSE, // panel redraw flag
		PANEL_MOUSE_LBDOWN	// panel mouse flag
	};

	bco::on_off_input		switchMainPower {		// On off switch for the main power supply
		GetControlId(),
		{ bm::vc::swMainPower_id },
		bm::vc::swMainPower_location, bm::vc::PowerTopRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlPwrMain_id,
		bm::pnl::pnlPwrMain_verts,
		bm::pnl::pnlPwrMain_RC
	};
	bco::on_off_input		switchConnectExternalPower {	// On off switch for connect to external power
		GetControlId(),
		{ bm::vc::swConnectExternalPower_id },
		bm::vc::swConnectExternalPower_location, bm::vc::PowerBottomRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlPwrExtBus_id,
		bm::pnl::pnlPwrExtBus_verts,
		bm::pnl::pnlPwrExtBus_RC
	};
	bco::on_off_input		switchConnectFuelCell {	// On off switch for connect to fuel cell
		GetControlId(),
		{ bm::vc::swConnectFuelCell_id },
		bm::vc::swConnectFuelCell_location, bm::vc::PowerBottomRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlPwrFCBus_id,
		bm::pnl::pnlPwrFCBus_verts,
		bm::pnl::pnlPwrFCBus_RC
	};
	bco::on_off_input		switchFuelCellPower {		// Main fuel cell power
		GetControlId(),
		{ bm::vc::swFuelCellPower_id },
		bm::vc::swFuelCellPower_location, bm::vc::PowerTopRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlPwrFC_id,
		bm::pnl::pnlPwrFC_verts,
		bm::pnl::pnlPwrFC_RC
	};
	bco::on_off_input		switchCanopyPower{		// Canopy Power
		GetControlId(),
		{ bm::vc::SwCanopyPower_id },
		bm::vc::SwCanopyPower_location, bm::vc::PowerTopRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlPwrCanopy_id,
		bm::pnl::pnlPwrCanopy_verts,
		bm::pnl::pnlPwrCanopy_RC
	};
	bco::on_off_input		switchCanopyOpen{		// Canopy Open
		GetControlId(),
		{ bm::vc::SwCanopyOpen_id },
		bm::vc::SwCanopyOpen_location, bm::vc::DoorsRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlDoorCanopy_id,
		bm::pnl::pnlDoorCanopy_verts,
		bm::pnl::pnlDoorCanopy_RC
	};
	bco::on_off_input		switchApuPower{			// APU Power
		GetControlId(),
		{ bm::vc::SwAPUPower_id },
		bm::vc::SwAPUPower_location, bm::vc::LeftPanelTopRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlAPUSwitch_id,
		bm::pnl::pnlAPUSwitch_verts,
		bm::pnl::pnlAPUSwitch_RC
	};
	bco::on_off_input		switchCargoPower{		// Cargo Power
		GetControlId(),
		{ bm::vc::SwCargoPower_id },
		bm::vc::SwCargoPower_location, bm::vc::PowerTopRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlPwrCargo_id,
		bm::pnl::pnlPwrCargo_verts,
		bm::pnl::pnlPwrCargo_RC
	};
	bco::on_off_input		switchCargoOpen{		// Cargo Open
		GetControlId(),
		{ bm::vc::SwCargoOpen_id },
		bm::vc::SwCargoOpen_location, bm::vc::DoorsRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlDoorCargo_id,
		bm::pnl::pnlDoorCargo_verts,
		bm::pnl::pnlDoorCargo_RC
	};
	bco::on_off_input		switchHoverOpen{		// Hover open
		GetControlId(),
		{ bm::vc::swHoverDoor_id },
		bm::vc::swHoverDoor_location, bm::vc::DoorsRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlDoorHover_id,
		bm::pnl::pnlDoorHover_verts,
		bm::pnl::pnlDoorHover_RC
	};

	// *** POWER GAUGES ***
	bco::rotary_display<bco::Animation>		gaugePowerVolts_{
		{ bm::vc::gaugeVoltMeter_id },
		bm::vc::gaugeVoltMeter_location, bm::vc::VoltMeterFrontAxis_location,
		bm::pnl::pnlVoltMeter_id,
		bm::pnl::pnlVoltMeter_verts,
		-(120 * RAD),
		0.2,
		[](double d) {return (d / 30); }	// Transform from volts to anim-range.
	};
	bco::rotary_display<bco::Animation>		gaugePowerAmps_{
		{ bm::vc::gaugeAmpMeter_id },
		bm::vc::gaugeAmpMeter_location, bm::vc::VoltMeterFrontAxis_location,
		bm::pnl::pnlAmpMeter_id,
		bm::pnl::pnlAmpMeter_verts,
		(120 * RAD),	// Clockwise
		0.2,
		[](double d) {return (d / 90); }	// Transform to amps.
	};

	// *** APU GAUGE ***
	bco::rotary_display<bco::Animation>		gaugeAPULevel_{
		{ bm::vc::gaHydPress_id },
		bm::vc::gaHydPress_location, bm::vc::axisHydPress_location,
		bm::pnl::pnlHydPress_id,
		bm::pnl::pnlHydPress_verts,
		(300 * RAD),	// Clockwise
		0.2,
		[](double d) {return (d); }	// Transform to amps.
	};

	// *** CLOCK HANDS ***
	bco::rotary_display<bco::AnimationWrap>	clockTimerSecondsHand_{
		{ bm::vc::ClockSecond_id },
		bm::vc::ClockSecond_location, bm::vc::ClockAxisFront_location,
		bm::pnl::pnlClockSecond_id,
		bm::pnl::pnlClockSecond_verts,
		(360 * RAD),	// Clockwise
		0.4,
		[](double d) {return (d / 60); }	// Transform to anim range.
	};
	bco::rotary_display<bco::AnimationWrap>	clockTimerMinutesHand_{
		{ bm::vc::ClockTimerMinute_id },
		bm::vc::ClockTimerMinute_location, bm::vc::ClockAxisFront_location,
		bm::pnl::pnlClockTimerMinute_id,
		bm::pnl::pnlClockTimerMinute_verts,
		(360 * RAD),	// Clockwise
		0.4,
		[](double d) {return (d / 60); }	// Transform to anim range.
	};
	bco::rotary_display<bco::AnimationWrap>	clockElapsedMinutesHand_{
		{ bm::vc::ClockMinute_id },
		bm::vc::ClockMinute_location, bm::vc::ClockAxisFront_location,
		bm::pnl::pnlClockMinute_id,
		bm::pnl::pnlClockMinute_verts,
		(360 * RAD),	// Clockwise
		0.4,
		[](double d) {return (d / 60); }	// Transform to anim range.
	};
	bco::rotary_display<bco::AnimationWrap>	clockElapsedHoursHand_{
		{ bm::vc::ClockHour_id },
		bm::vc::ClockHour_location, bm::vc::ClockAxisFront_location,
		bm::pnl::pnlClockHour_id,
		bm::pnl::pnlClockHour_verts,
		(360 * RAD),	// Clockwise
		0.4,
		[](double d) {return (d / 12); }	// Transform to anim range.
	};

	// *** POWER STATUS LIGHTS:
	bco::on_off_display	lightFuelCellAvail_{
		GetControlId(),
		bm::vc::FuelCellAvailableLight_id,
		bm::vc::FuelCellAvailableLight_verts,
		bm::pnl::pnlLgtFCPwrAvail_id,
		bm::pnl::pnlLgtFCPwrAvail_verts,
		0.0244 
	};
	bco::on_off_display	lightExternalAvail_{
		GetControlId(),
		bm::vc::ExtAvailableLight_id,
		bm::vc::ExtAvailableLight_verts,
		bm::pnl::pnlLgtExtPwrAvail_id,
		bm::pnl::pnlLgtExtPwrAvail_verts,
		0.0244
	};
	bco::on_off_display	lightFuelCellConnected_{
		GetControlId(),
		bm::vc::FuelCellConnectedLight_id,
		bm::vc::FuelCellConnectedLight_verts,
		bm::pnl::pnlLgtFCPwrOn_id,
		bm::pnl::pnlLgtFCPwrOn_verts,
		0.0244
	};
	bco::on_off_display	lightExternalConnected_{
		GetControlId(),
		bm::vc::ExtConnectedLight_id,
		bm::vc::ExtConnectedLight_verts,
		bm::pnl::pnlLgtExtPwrOn_id,
		bm::pnl::pnlLgtExtPwrOn_verts,
		0.0244
	};

	// *** HUD *** 
	bco::simple_event<>		btnHudDocking_{
		GetControlId(),
		bm::vc::vcHUDDock_location,
		0.01,
		bm::pnl::pnlHUDDock_RC
	};
	bco::on_off_display	btnLightHudDocking_{
		GetControlId(),
		bm::vc::vcHUDDock_id,
		bm::vc::vcHUDDock_verts,
		bm::pnl::pnlHUDDock_id,
		bm::pnl::pnlHUDDock_verts,
		0.0352
	};
	bco::simple_event<>		btnHudOrbit_{
		GetControlId(),
		bm::vc::vcHUDOrbit_location,
		0.01,
		bm::pnl::pnlHUDOrbit_RC
	};
	bco::on_off_display	btnLightHudOrbit_{
		GetControlId(),
		bm::vc::vcHUDOrbit_id,
		bm::vc::vcHUDOrbit_verts,
		bm::pnl::pnlHUDOrbit_id,
		bm::pnl::pnlHUDOrbit_verts,
		0.0352
	};
	bco::simple_event<>		btnHudSurface_{
		GetControlId(),
		bm::vc::vcHUDSURF_location,
		0.01,
		bm::pnl::pnlHUDSurf_RC
	};
	bco::on_off_display	btnLightHudSurface_{
		GetControlId(),
		bm::vc::vcHUDSURF_id,
		bm::vc::vcHUDSURF_verts,
		bm::pnl::pnlHUDSurf_id,
		bm::pnl::pnlHUDSurf_verts,
		0.0352
	};

	// ***  HYDROGEN SUPPLY  *** //
	bco::rotary_display<bco::Animation>		gaugeHydroLevel_{
		{ bm::vc::gaHydrogenLevel_id },
		bm::vc::gaHydrogenLevel_location, bm::vc::axisHydrogenLevel_location,
		bm::pnl::pnlLH2Press_id,
		bm::pnl::pnlLH2Press_verts,
		(300 * RAD),	// Clockwise
		0.2,
		[](double d) {return (d); }	// Transform to amps.
	};
	bco::on_off_display	lightHydrogenAvail_{
		GetControlId(),
		bm::vc::LH2SupplyOnLight_id,
		bm::vc::LH2SupplyOnLight_verts,
		bm::pnl::pnlLH2Avail_id,
		bm::pnl::pnlLH2Avail_verts,
		0.0244
	};
	bco::simple_event<>		btnHydroFill_{
		GetControlId(),
		bm::vc::LH2ValveOpenSwitch_location,
		0.01,
		bm::pnl::pnlLH2Switch_RC
	};
	bco::on_off_display	btnLightHydroFill_{
		GetControlId(),
		bm::vc::LH2ValveOpenSwitch_id,
		bm::vc::LH2ValveOpenSwitch_verts,
		bm::pnl::pnlLH2Switch_id,
		bm::pnl::pnlLH2Switch_verts,
		0.0352
	};

	// ***  LOX SUPPLY  *** //
	bco::rotary_display<bco::Animation>		gaugeO2Level_{
		{ bm::vc::gaugeOxygenLevel_id },
		bm::vc::gaugeOxygenLevel_location, bm::vc::axisOxygenLevel_location,
		bm::pnl::pnlLOXPress_id,
		bm::pnl::pnlLOXPress_verts,
		(300 * RAD),	// Clockwise
		0.2,
		[](double d) {return (d); }	// Transform to amps.
	};
	bco::on_off_display	lightO2Avail_{
		GetControlId(),
		bm::vc::LOXSupplyOnLight_id,
		bm::vc::LOXSupplyOnLight_verts,
		bm::pnl::pnlO2Avail_id,
		bm::pnl::pnlO2Avail_verts,
		0.0244
	};
	bco::simple_event<>		btnO2Fill_{
		GetControlId(),
		bm::vc::LOXValveOpenSwitch_location,
		0.01,
		bm::pnl::pnlO2Switch_RC
	};
	bco::on_off_display	btnLightO2Fill{
		GetControlId(),
		bm::vc::LOXValveOpenSwitch_id,
		bm::vc::LOXValveOpenSwitch_verts,
		bm::pnl::pnlO2Switch_id,
		bm::pnl::pnlO2Switch_verts,
		0.0352
	};

	// ***  AIR BRAKE  *** //
	bco::simple_event<>		btnDecreaseAirbrake_{
		GetControlId(),
		bm::vc::ABTargetDecrease_location,
		0.01,
		bm::pnl::pnlAirBrakeDecrease_RC
	};
	bco::simple_event<>		btnIncreaseAirbrake_{
		GetControlId(),
		bm::vc::ABTargetIncrease_location,
		0.01,
		bm::pnl::pnlAirBrakeIncrease_RC
	};

	// *** LANDING GEAR *** //
	bco::simple_event<>		btnRaiseLandingGear_{
		GetControlId(),
		bm::vc::GearLeverUpTarget_location,
		0.01,
		bm::pnl::pnlLandingGearUp_RC
	};
	bco::simple_event<>		btnLowerLandingGear_{
		GetControlId(),
		bm::vc::GearLeverDownTarget_location,
		0.01,
		bm::pnl::pnlLandingGearDown_RC
	};

	// ***  LIGHTS  *** //
	bco::on_off_input		switchNavigationLights{		// On off switch for external navigation lights.
		GetControlId(),
		{ bm::vc::SwitchNavLights_id },
		bm::vc::SwitchNavLights_location, bm::vc::LightsRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlLightNav_id,
		bm::pnl::pnlLightNav_verts,
		bm::pnl::pnlLightNav_RC
	};
	bco::on_off_input		switchBeaconLights{		// On off switch for external beacon lights.
		GetControlId(),
		{ bm::vc::SwitchBeaconLights_id },
		bm::vc::SwitchBeaconLights_location, bm::vc::LightsRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlLightBeacon_id,
		bm::pnl::pnlLightBeacon_verts,
		bm::pnl::pnlLightBeacon_RC
	};
	bco::on_off_input		switchStrobeLights{		// On off switch for external strobe lights.
		GetControlId(),
		{ bm::vc::SwitchStrobeLights_id },
		bm::vc::SwitchStrobeLights_location, bm::vc::LightsRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlLightStrobe_id,
		bm::pnl::pnlLightStrobe_verts,
		bm::pnl::pnlLightStrobe_RC
	};

	// ***  NAV MODES  *** //
	bco::simple_event<int>	btnNavKillRot_{
		GetControlId(),
		bm::vc::vcNavKillRot_location,
		0.01,
		bm::pnl::pnlNavKillrot_RC,
		NAVMODE_KILLROT
	};
	bco::on_off_display	btnLightNavKillRot_{
		GetControlId(),
		bm::vc::vcNavKillRot_id,
		bm::vc::vcNavKillRot_verts,
		bm::pnl::pnlNavKillrot_id,
		bm::pnl::pnlNavKillrot_verts,
		0.0352
	};
	bco::simple_event<int>	btnNavHorzLevel_{
		GetControlId(),
		bm::vc::vcNavHorzLvl_location,
		0.01,
		bm::pnl::pnlNavHorzLvl_RC,
		NAVMODE_HLEVEL
	};
	bco::on_off_display	btnLightNavHorzLevel_{
		GetControlId(),
		bm::vc::vcNavHorzLvl_id,
		bm::vc::vcNavHorzLvl_verts,
		bm::pnl::pnlNavHorzLvl_id,
		bm::pnl::pnlNavHorzLvl_verts,
		0.0352
	};
	bco::simple_event<int>	btnNavPrograde_{
		GetControlId(),
		bm::vc::vcNavProGrade_location,
		0.01,
		bm::pnl::pnlNavPrograde_RC,
		NAVMODE_PROGRADE
	};
	bco::on_off_display	btnLightNavPrograde_{
		GetControlId(),
		bm::vc::vcNavProGrade_id,
		bm::vc::vcNavProGrade_verts,
		bm::pnl::pnlNavPrograde_id,
		bm::pnl::pnlNavPrograde_verts,
		0.0352
	};
	bco::simple_event<int>	btnNavRetrograde_{
		GetControlId(),
		bm::vc::vcNavRetro_location,
		0.01,
		bm::pnl::pnlNavRetro_RC,
		NAVMODE_RETROGRADE
	};
	bco::on_off_display	btnLightNavRetrograde_{
		GetControlId(),
		bm::vc::vcNavRetro_id,
		bm::vc::vcNavRetro_verts,
		bm::pnl::pnlNavRetro_id,
		bm::pnl::pnlNavRetro_verts,
		0.0352
	};
	bco::simple_event<int>	btnNavNormal_{
		GetControlId(),
		bm::vc::vcNavNorm_location,
		0.01,
		bm::pnl::pnlNavNorm_RC,
		NAVMODE_NORMAL
	};
	bco::on_off_display	btnLightNavNormal_{
		GetControlId(),
		bm::vc::vcNavNorm_id,
		bm::vc::vcNavNorm_verts,
		bm::pnl::pnlNavNorm_id,
		bm::pnl::pnlNavNorm_verts,
		0.0352
	};
	bco::simple_event<int>	btnNavAntiNorm_{
		GetControlId(),
		bm::vc::vcNavAntiNorm_location,
		0.01,
		bm::pnl::pnlNavAntiNorm_RC,
		NAVMODE_ANTINORMAL
	};
	bco::on_off_display	btnLightNavAntiNorm_{
		GetControlId(),
		bm::vc::vcNavAntiNorm_id,
		bm::vc::vcNavAntiNorm_verts,
		bm::pnl::pnlNavAntiNorm_id,
		bm::pnl::pnlNavAntiNorm_verts,
		0.0352
	};

	// ***  PROPULSION  *** //
	bco::rotary_display<bco::Animation>		gaugeFuelFlow_{
		{ bm::vc::gaFuelFlow_id },
		bm::vc::gaFuelFlow_location, bm::vc::FuelFlowAxisFront_location,
		bm::pnl::pnlGaFuelFlow_id,
		bm::pnl::pnlGaFuelFlow_verts,
		(270 * RAD),	// Clockwise
		1.0,
		[](double d) {return (d); }	// Transform to amps.
	};
	bco::rotary_display<bco::Animation>		gaugeFuelMain_{
		{ bm::vc::gaMainFuel_id },
		bm::vc::gaMainFuel_location, bm::vc::FuelLevelAxisFront_location,
		bm::pnl::pnlGaFuelMain_id,
		bm::pnl::pnlGaFuelMain_verts,
		(256 * RAD),	// Clockwise
		1.0,
		[](double d) {return (d); }	// Transform to amps.
	};
	bco::rotary_display<bco::Animation>		gaugeFuelRCS_{
		{ bm::vc::gaRCSFuel_id },
		bm::vc::gaRCSFuel_location, bm::vc::RCSLevelAxisFront_location,
		bm::pnl::pnlGaFuelRCS_id,
		bm::pnl::pnlGaFuelRCS_verts,
		(264 * RAD),	// Clockwise
		1.0,
		[](double d) {return (d); }	// Transform to amps.
	};
	bco::on_off_input		switchThrustLimit_{		// Thrust Limit
		GetControlId(),
		{ bm::vc::swThrottleLimit_id },
		bm::vc::swThrottleLimit_location, bm::vc::TopRowSwitchRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlThrottleLimit_id,
		bm::pnl::pnlThrottleLimit_verts,
		bm::pnl::pnlThrottleLimit_RC
	};
	bco::on_off_input		switchFuelDump_{		// Fuel dump
		GetControlId(),
		{ bm::vc::swDumpFuel_id },
		bm::vc::swDumpFuel_location, bm::vc::FuelTransferRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlFuelDump_id,
		bm::pnl::pnlFuelDump_verts,
		bm::pnl::pnlFuelDump_RC
	};
	bco::on_off_input		switchTransferSelect_{		// Thrust Limit
		GetControlId(),
		{ bm::vc::swTransferSelect_id },
		bm::vc::swTransferSelect_location, bm::vc::FuelTransferRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlFuelTransferSelect_id,
		bm::pnl::pnlFuelTransferSelect_verts,
		bm::pnl::pnlFuelTransferSelect_RC
	};
	bco::on_off_display	lightFuelAvail_{
		GetControlId(),
		bm::vc::FuelSupplyOnLight_id,
		bm::vc::FuelSupplyOnLight_verts,
		bm::pnl::pnlFuelAvail_id,
		bm::pnl::pnlFuelAvail_verts,
		0.0244
	};
	bco::simple_event<>		btnFuelTransferPump_{
		GetControlId(),
		bm::vc::FuelTransferSwitch_location,
		0.01,
		bm::pnl::pnlFuelTransfer_RC
	};
	bco::on_off_display	btnLightFuelTransferPump_{
		GetControlId(),
		bm::vc::FuelTransferSwitch_id,
		bm::vc::FuelTransferSwitch_verts,
		bm::pnl::pnlFuelTransfer_id,
		bm::pnl::pnlFuelTransfer_verts,
		0.0352
	};
	bco::simple_event<>		btnFuelValveOpen_{
		GetControlId(),
		bm::vc::FuelValveOpenSwitch_location,
		0.01,
		bm::pnl::pnlFuelValveSwitch_RC
	};
	bco::on_off_display	btnLightFuelValveOpen_{
		GetControlId(),
		bm::vc::FuelValveOpenSwitch_id,
		bm::vc::FuelValveOpenSwitch_verts,
		bm::pnl::pnlFuelValveSwitch_id,
		bm::pnl::pnlFuelValveSwitch_verts,
		0.0352
	};

	// ***  RCS  *** //
	bco::simple_event<>		btnRCSLinear_{
		GetControlId(),
		bm::vc::vcRCSLin_location,
		0.01,
		bm::pnl::pnlRCSLin_RC
	};
	bco::on_off_display	btnLightRCSLinear_{
		GetControlId(),
		bm::vc::vcRCSLin_id,
		bm::vc::vcRCSLin_verts,
		bm::pnl::pnlRCSLin_id,
		bm::pnl::pnlRCSLin_verts,
		0.0352
	};
	bco::simple_event<>		btnRCSRotate_{
		GetControlId(),
		bm::vc::vcRCSRot_location,
		0.01,
		bm::pnl::pnlRCSRot_RC
	};
	bco::on_off_display	btnLightRCSRotate_{
		GetControlId(),
		bm::vc::vcRCSRot_id,
		bm::vc::vcRCSRot_verts,
		bm::pnl::pnlRCSRot_id,
		bm::pnl::pnlRCSRot_verts,
		0.0352
	};

	// ***  Retro Doors  *** //
	bco::on_off_input		switchRetroDoors_{		// Open close for retro doors.
		GetControlId(),
		{ bm::vc::swRetroDoors_id },
		bm::vc::swRetroDoors_location, bm::vc::DoorsRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlDoorRetro_id,
		bm::pnl::pnlDoorRetro_verts,
		bm::pnl::pnlDoorRetro_RC
	};

	// ***  Shutters  *** //
	bco::on_off_input		switchShutters_{		// Open close shutters
		GetControlId(),
		{ bm::vc::swShutter_id },
		bm::vc::swShutter_location, bm::vc::DoorsRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlScreenSwitch_id,
		bm::pnl::pnlScreenSwitch_verts,
		bm::pnl::pnlScreenSwitch_RC
	};

	// ***  Avionics  *** //
	bco::on_off_input		switchAvionPower_{		// Main avionics power
		GetControlId(),
		{ bm::vc::SwAvionics_id },
		bm::vc::SwAvionics_location, bm::vc::PowerTopRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlPwrAvion_id,
		bm::pnl::pnlPwrAvion_verts,
		bm::pnl::pnlPwrAvion_RC
	};
	bco::on_off_input		switchAvionMode_{		// Atmosphere=On, External=Off
		GetControlId(),
		{ bm::vc::vcAvionMode_id },
		bm::vc::vcAvionMode_location, bm::vc::vcAttitudeSwitchesAxis_location,
		toggleOnOff,
		bm::pnl::pnlAvionMode_id,
		bm::pnl::pnlAvionMode_verts,
		bm::pnl::pnlAvionMode_RC
	};
	bco::on_off_input		switchNavMode_{		// Nav mode 1 2
		GetControlId(),
		{ bm::vc::vcNavMode_id },
		bm::vc::vcNavMode_location, bm::vc::vcAttitudeSwitchesAxis_location,
		toggleOnOff,
		bm::pnl::pnlNavMode_id,
		bm::pnl::pnlNavMode_verts,
		bm::pnl::pnlNavMode_RC
	};

	// ***  Altimeter  *** //
	bco::rotary_display<bco::AnimationWrap>	altimeter1Hand_{
		{ bm::vc::gaAlt1Needle_id },
		bm::vc::gaAlt1Needle_location, bm::vc::AltimeterAxis_location,
		bm::pnl::pnlAlt1Needle_id,
		bm::pnl::pnlAlt1Needle_verts,
		(360 * RAD),	// Clockwise
		2.0,
		[](double d) {return ((double)d / 10.0); }	// Transform to anim range.
	};
	bco::rotary_display<bco::AnimationWrap>	altimeter10Hand_{
		{ bm::vc::gaAlt10Needle_id },
		bm::vc::gaAlt10Needle_location, bm::vc::AltimeterAxis_location,
		bm::pnl::pnlAlt10Needle_id,
		bm::pnl::pnlAlt10Needle_verts,
		(360 * RAD),	// Clockwise
		2.0,
		[](double d) {return (d / 10); }	// Transform to anim range.
	};
	bco::rotary_display<bco::AnimationWrap>	altimeter100Hand_{
		{ bm::vc::gaAlt100Needle_id },
		bm::vc::gaAlt100Needle_location, bm::vc::AltimeterAxis_location,
		bm::pnl::pnlAlt100Needle_id,
		bm::pnl::pnlAlt100Needle_verts,
		(360 * RAD),	// Clockwise
		2.0,
		[](double d) {return (d / 10); }	// Transform to anim range.
	};
	bco::flat_roll			tdiAltOnes_{
		bm::vc::vcTDIAltOnes_id,
		bm::vc::vcTDIAltOnes_verts,
		bm::pnl::pnlTDIAltOnes_id,
		bm::pnl::pnlTDIAltOnes_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::flat_roll			tdiAltTens_{
		bm::vc::vcTDIAltTens_id,
		bm::vc::vcTDIAltTens_verts,
		bm::pnl::pnlTDIAltTens_id,
		bm::pnl::pnlTDIAltTens_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::flat_roll			tdiAltHunds_{
		bm::vc::vcTDIAltHunds_id,
		bm::vc::vcTDIAltHunds_verts,
		bm::pnl::pnlTDIAltHund_id,
		bm::pnl::pnlTDIAltHund_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::flat_roll			tdiAltThou_{
		bm::vc::vcTDIAltThous_id,
		bm::vc::vcTDIAltThous_verts,
		bm::pnl::pnlTDIAltThous_id,
		bm::pnl::pnlTDIAltThous_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::flat_roll			tdiAltTenThou_{
		bm::vc::vcTDIAltTenThous_id,
		bm::vc::vcTDIAltTenThous_verts,
		bm::pnl::pnlTDIAltTenThou_id,
		bm::pnl::pnlTDIAltTenThou_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::on_off_display	altimeterEnabledFlag_{
		GetControlId(),
		bm::vc::AltimeterOffFlag_id,
		bm::vc::AltimeterOffFlag_verts,
		bm::pnl::pnlAltimeterOffFlag_id,
		bm::pnl::pnlAltimeterOffFlag_verts,
		0.0244
	};
	bco::on_off_display	altimeterExoModeFlag_{
		GetControlId(),
		bm::vc::AltimeterGround_id,
		bm::vc::AltimeterGround_verts,
		bm::pnl::pnlAltimeterGround_id,
		bm::pnl::pnlAltimeterGround_verts,
		0.0244
	};

	// ***   VSI  *** //
	bco::rotary_display<bco::Animation>		vsiHand_{
		{ bm::vc::gaVSINeedle_id },
		bm::vc::gaVSINeedle_location, bm::vc::VSIAxis_location,
		bm::pnl::pnlVSINeedle_id,
		bm::pnl::pnlVSINeedle_verts,
		(340 * RAD),	// Clockwise
		2.0,
		[](double d) {return (d); }	// Transform to amps.
	};
	bco::on_off_display	vsiActiveFlag_{
		GetControlId(),
		bm::vc::VSIOffFlag_id,
		bm::vc::VSIOffFlag_verts,
		bm::pnl::pnlVSIOffFlag_id,
		bm::pnl::pnlVSIOffFlag_verts,
		0.0244
	};

	// ***  Attitude  *** //
	bco::transform_display	attitudeDisplay_{
		bm::vc::AttitudeIndicator_id,
		bm::vc::AttitudeIndicator_verts,
		bm::pnl::pnlAttitudeIndicator_id,
		bm::pnl::pnlAttitudeIndicator_verts
	};
	bco::on_off_display	attitudeFlag_{
		GetControlId(),
		bm::vc::AttitudeFlagOff_id,
		bm::vc::AttitudeFlagOff_verts,
		bm::pnl::pnlAttitudeFlagOff_id,
		bm::pnl::pnlAttitudeFlagOff_verts,
		0.0244
	};
	bco::on_off_display	comStatusFlag_{
		GetControlId(),
		bm::vc::COMStatusPanel_id,
		bm::vc::COMStatusPanel_verts,
		bm::pnl::pnlCOMStatusPanel_id,
		bm::pnl::pnlCOMStatusPanel_verts,
		0.0244
	};

	// ***  HSI  *** //
	bco::rotary_display<bco::AnimationWrap>	hsiRoseCompass_{
		{ bm::vc::RoseCompass_id },
		bm::vc::RoseCompass_location, bm::vc::HSIAxis_location,
		bm::pnl::pnlRoseCompass_id,
		bm::pnl::pnlRoseCompass_verts,
		(360 * RAD),	// Clockwise
		1.0,
		[](double d) {return bco::AngleToState(-d); }	// Transform to anim range.
	};
	bco::rotary_display<bco::AnimationWrap>	hsiHeadingBug_{
		{ bm::vc::HSICompassHeading_id },
		bm::vc::HSICompassHeading_location, bm::vc::HSIAxis_location,
		bm::pnl::pnlHSICompassHeading_id,
		bm::pnl::pnlHSICompassHeading_verts,
		(360 * RAD),	// Clockwise
		1.0,
		[](double d) {return bco::AngleToState(-d); }	// Transform to anim range.
	};
	bco::rotary_display<bco::AnimationWrap> hsiCourse_{
		{ bm::vc::HSICourse_id },
		bm::vc::HSICourse_location, bm::vc::HSIAxis_location,
		bm::pnl::pnlHSICourse_id,
		bm::pnl::pnlHSICourse_verts,
		(360 * RAD),
		1.0,
		[](double d) {return bco::AngleToState(-d); }
	};
	bco::rotary_display<bco::AnimationWrap> hsiBearing_{
		{ bm::vc::HSIBearingArrow_id },
		bm::vc::HSIBearingArrow_location, bm::vc::HSIAxis_location,
		bm::pnl::pnlHSIBearingArrow_id,
		bm::pnl::pnlHSIBearingArrow_verts,
		(360 * RAD),
		1.0,
		[](double d) {return bco::AngleToState(-d); }
	};
	bco::transform_display	hsiCourseError_{
		bm::vc::HSICourseNeedle_id,
		bm::vc::HSICourseNeedle_verts,
		bm::pnl::pnlHSICourseNeedle_id,
		bm::pnl::pnlHSICourseNeedle_verts
	};
	bco::flat_roll			hsiCRSOnes_{
		bm::vc::vcCrsOnes_id,
		bm::vc::vcCrsOnes_verts,
		bm::pnl::pnlHSICRSOnes_id,
		bm::pnl::pnlHSICRSOnes_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::flat_roll			hsiCRSTens_{
		bm::vc::vcCrsTens_id,
		bm::vc::vcCrsTens_verts,
		bm::pnl::pnlHSICRSTens_id,
		bm::pnl::pnlHSICRSTens_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::flat_roll			hsiCRSHunds_{
		bm::vc::vcCrsHunds_id,
		bm::vc::vcCrsHunds_verts,
		bm::pnl::pnlHSICRSHunds_id,
		bm::pnl::pnlHSICRSHunds_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::flat_roll			hsiMilesOnes_{
		bm::vc::vcMilesOnes_id,
		bm::vc::vcMilesOnes_verts,
		bm::pnl::pnlHSIMilesOnes_id,
		bm::pnl::pnlHSIMilesOnes_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::flat_roll			hsiMilesTens_{
		bm::vc::vcMilesTens_id,
		bm::vc::vcMilesTens_verts,
		bm::pnl::pnlHSIMilesTens_id,
		bm::pnl::pnlHSIMilesTens_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::flat_roll			hsiMilesHunds_{
		bm::vc::vcMilesHunds_id,
		bm::vc::vcMilesHunds_verts,
		bm::pnl::pnlHSIMilesHunds_id,
		bm::pnl::pnlHSIMilesHunds_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::simple_event<>		dialSetCourseIncrement_{
		GetControlId(),
		bm::vc::CourseKnob_location,
		0.01,
		bm::pnl::pnlSetCourseInc_RC
	};
	bco::simple_event<>		dialSetCourseDecrement_{
		GetControlId(),
		bm::vc::CourseKnob_location,
		0.01,
		bm::pnl::pnlSetCourseDec_RC
	};
	bco::simple_event<>		dialSetHeadingIncrement_{
		GetControlId(),
		bm::vc::HeadingKnob_location,
		0.01,
		bm::pnl::pnlSetHeadingInc_RC
	};
	bco::simple_event<>		dialSetHeadingDecrement_{
		GetControlId(),
		bm::vc::HeadingKnob_location,
		0.01,
		bm::pnl::pnlSetHeadingDec_RC
	};
	bco::on_off_display	hsiOffFlag_{
		GetControlId(),
		bm::vc::HSIOffFlag_id,
		bm::vc::HSIOffFlag_verts,
		bm::pnl::pnlHSIOffFlag_id,
		bm::pnl::pnlHSIOffFlag_verts,
		0.0244
	};
	bco::on_off_display	hsiExoFlag_{
		GetControlId(),
		bm::vc::HSIExoFlag_id,
		bm::vc::HSIExoFlag_verts,
		bm::pnl::pnlHSIExoFlag_id,
		bm::pnl::pnlHSIExoFlag_verts,
		0.0244
	};

	// ***  Airspeed  *** //
	bco::rotary_display<bco::AnimationWrap>	airspeedSpeed_{
		{ bm::vc::SpeedNeedle_id },
		bm::vc::SpeedNeedle_location, bm::vc::SpeedAxisBack_location,
		bm::pnl::pnlSpeedNeedle_id,
		bm::pnl::pnlSpeedNeedle_verts,
		(300 * RAD),	// Clockwise
		2.0,
		[](double d) {return bco::AngleToState(d); }	// Transform to anim range.
	};
	bco::rotary_display<bco::AnimationWrap>	airspeedKies_{
		{ bm::vc::SpeedIndicatorKies_id },
		bm::vc::SpeedIndicatorKies_location, bm::vc::SpeedAxisBack_location,
		bm::pnl::pnlSpeedIndicatorKies_id,
		bm::pnl::pnlSpeedIndicatorKies_verts,
		(300 * RAD),	// Clockwise
		2.0,
		[](double d) {return bco::AngleToState(-d); }	// Transform to anim range.
	};
	bco::rotary_display<bco::AnimationWrap>	airspeedMaxMach_{
		{ bm::vc::SpeedNeedleMax_id },
		bm::vc::SpeedNeedleMax_location, bm::vc::SpeedAxisBack_location,
		bm::pnl::pnlSpeedNeedleMax_id,
		bm::pnl::pnlSpeedNeedleMax_verts,
		(300 * RAD),	// Clockwise
		2.0,
		[](double d) {return bco::AngleToState(-d); }	// Transform to anim range.
	};
	bco::on_off_display	speedIsEnabledFlag_{
		GetControlId(),
		bm::vc::SpeedFlagOff_id,
		bm::vc::SpeedFlagOff_verts,
		bm::pnl::pnlSpeedFlagOff_id,
		bm::pnl::pnlSpeedFlagOff_verts,
		0.0244
	};
	bco::on_off_display	speedIsVelocityFlag_{
		GetControlId(),
		bm::vc::SpeedVelocityFlag_id,
		bm::vc::SpeedVelocityFlag_verts,
		bm::pnl::pnlSpeedVelocityFlag_id,
		bm::pnl::pnlSpeedVelocityFlag_verts,
		0.0244
	};
	bco::flat_roll			airspeedKeasOnes_{
		bm::vc::vcTDIKeasOnes_id,
		bm::vc::vcTDIKeasOnes_verts,
		bm::pnl::pnlTDIKEASOnes_id,
		bm::pnl::pnlTDIKEASOnes_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::flat_roll			airspeedKeasTens_{
		bm::vc::vcTDIKeasTens_id,
		bm::vc::vcTDIKeasTens_verts,
		bm::pnl::pnlTDIKEASTens_id,
		bm::pnl::pnlTDIKEASTens_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::flat_roll			airspeedKeasHunds_{
		bm::vc::vcTDIKeasHunds_id,
		bm::vc::vcTDIKeasHunds_verts,
		bm::pnl::pnlTDIKEASHunds_id,
		bm::pnl::pnlTDIKEASHunds_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::flat_roll			airspeedMACHOnes_{
		bm::vc::vcTDIMachOne_id,
		bm::vc::vcTDIMachOne_verts,
		bm::pnl::pnlTDIMACHOne_id,
		bm::pnl::pnlTDIMACHOne_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::flat_roll			airspeedMACHTens_{
		bm::vc::vcTDIMachTens_id,
		bm::vc::vcTDIMachTens_verts,
		bm::pnl::pnlTDIMACHTens_id,
		bm::pnl::pnlTDIMACHTens_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};
	bco::flat_roll			airspeedMACHHunds_{
		bm::vc::vcTDIMachHunds_id,
		bm::vc::vcTDIMachHunds_verts,
		bm::pnl::pnlTDIMACHHunds_id,
		bm::pnl::pnlTDIMACHHunds_verts,
		0.1084,
		[](double v) {return floor(v) / 10; }
	};


	//// ** TEST ** //
	//bco::flat_roll<int>  numtest{
	//	bm::pnl::pnlMilesOnes_id,
	//	bm::pnl::pnlMilesOnes_verts,
	//	0.1084,		// Tex offset
	//	[](int v) {return (double)v / 10; }
	//};

	//bco::simple_event		testUp_{
	//	GetControlId(),
	//	bm::vc::vcRCSRot_location,
	//	0.01,
	//	bm::pnl::pnlMilesHunds_RC
	//};

	//bco::simple_event		testDown_{
	//	GetControlId(),
	//	bm::vc::vcRCSLin_location,
	//	0.01,
	//	bm::pnl::pnlMilesTens_RC
	//};

	// ** COMPONENTS **
	NavLight			navLight_;
	Beacon				beacon_;
	Strobe				strobe_;
	Altimeter			altimeter_;
	//VSI					vsi_;
	//AttitudeIndicator	attitude_;
	HSI					hsi_;
	AeroData			aeroData_;
	Airspeed			airspeed_;
//	TestComponent		test_;
};

