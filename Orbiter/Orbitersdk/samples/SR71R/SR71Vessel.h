#pragma once

#include "Orbitersdk.h"

#include "bc_orbiter\BaseVessel.h"
#include "bc_orbiter\on_off_display.h"
#include "bc_orbiter\on_off_input.h"
#include "bc_orbiter\rotary_display.h"
#include "bc_orbiter\simple_event.h"

#include "ShipMets.h"
#include "SR71r_mesh.h"
#include "Avionics.h"
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
#include "FlightComputer.h"
#include "VesselControl.h"
#include "HoverEngines.h"
#include "RetroEngines.h"
#include "NavLights.h"
#include "Beacon.h"
#include "Strobe.h"

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
	Avionics				avionics_;
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
	FC::FlightComputer		computer_;
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
	bco::on_off_display		lightFuelCellAvail_{
		GetControlId(),
		bm::vc::FuelCellAvailableLight_id,
		bm::vc::FuelCellAvailableLight_verts,
		bm::pnl::pnlLgtFCPwrAvail_id,
		bm::pnl::pnlLgtFCPwrAvail_verts,
		0.0244 
	};

	bco::on_off_display		lightExternalAvail_{
		GetControlId(),
		bm::vc::ExtAvailableLight_id,
		bm::vc::ExtAvailableLight_verts,
		bm::pnl::pnlLgtExtPwrAvail_id,
		bm::pnl::pnlLgtExtPwrAvail_verts,
		0.0244
	};

	bco::on_off_display		lightFuelCellConnected_{
		GetControlId(),
		bm::vc::FuelCellConnectedLight_id,
		bm::vc::FuelCellConnectedLight_verts,
		bm::pnl::pnlLgtFCPwrOn_id,
		bm::pnl::pnlLgtFCPwrOn_verts,
		0.0244
	};

	bco::on_off_display		lightExternalConnected_{
		GetControlId(),
		bm::vc::ExtConnectedLight_id,
		bm::vc::ExtConnectedLight_verts,
		bm::pnl::pnlLgtExtPwrOn_id,
		bm::pnl::pnlLgtExtPwrOn_verts,
		0.0244
	};

	// *** HUD *** 
	bco::simple_event	btnHudDocking_{
		GetControlId(),
		bm::vc::vcHUDDock_location,
		0.01,
		bm::pnl::pnlHUDDock_RC
	};

	bco::on_off_display dspHudDocking_{
		GetControlId(),
		bm::vc::vcHUDDock_id,
		bm::vc::vcHUDDock_verts,
		bm::pnl::pnlHUDDock_id,
		bm::pnl::pnlHUDDock_verts,
		0.0352
	};

	bco::simple_event	btnHudOrbit_{
		GetControlId(),
		bm::vc::vcHUDOrbit_location,
		0.01,
		bm::pnl::pnlHUDOrbit_RC
	};

	bco::on_off_display dspHudOrbit_{
		GetControlId(),
		bm::vc::vcHUDOrbit_id,
		bm::vc::vcHUDOrbit_verts,
		bm::pnl::pnlHUDOrbit_id,
		bm::pnl::pnlHUDOrbit_verts,
		0.0352
	};

	bco::simple_event	btnHudSurface_{
		GetControlId(),
		bm::vc::vcHUDSURF_location,
		0.01,
		bm::pnl::pnlHUDSurf_RC
	};

	bco::on_off_display dspHudSurface_{
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

	bco::on_off_display		lightHydrogenAvail_{
		GetControlId(),
		bm::vc::LH2SupplyOnLight_id,
		bm::vc::LH2SupplyOnLight_verts,
		bm::pnl::pnlLH2Avail_id,
		bm::pnl::pnlLH2Avail_verts,
		0.0244
	};

	bco::simple_event	btnHydroFill_{
		GetControlId(),
		bm::vc::LH2ValveOpenSwitch_location,
		0.01,
		bm::pnl::pnlLH2Switch_RC
	};

	bco::on_off_display dspHydroFill_{
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

	bco::on_off_display		lightO2Avail_{
		GetControlId(),
		bm::vc::LOXSupplyOnLight_id,
		bm::vc::LOXSupplyOnLight_verts,
		bm::pnl::pnlO2Avail_id,
		bm::pnl::pnlO2Avail_verts,
		0.0244
	};

	bco::simple_event	btnO2Fill_{
		GetControlId(),
		bm::vc::LOXValveOpenSwitch_location,
		0.01,
		bm::pnl::pnlO2Switch_RC
	};

	bco::on_off_display dspO2Fill_{
		GetControlId(),
		bm::vc::LOXValveOpenSwitch_id,
		bm::vc::LOXValveOpenSwitch_verts,
		bm::pnl::pnlO2Switch_id,
		bm::pnl::pnlO2Switch_verts,
		0.0352
	};

	// ***  AIR BRAKE  *** //
	bco::simple_event btnDecreaseAirbrake_{
		GetControlId(),
		bm::vc::ABTargetDecrease_location,
		0.01,
		bm::pnl::pnlAirBrakeDecrease_RC
	};

	bco::simple_event btnIncreaseAirbrake_{
		GetControlId(),
		bm::vc::ABTargetIncrease_location,
		0.01,
		bm::pnl::pnlAirBrakeIncrease_RC
	};

	// *** LANDING GEAR *** //
	bco::simple_event btnRaiseLandingGear_{
		GetControlId(),
		bm::vc::GearLeverUpTarget_location,
		0.01,
		bm::pnl::pnlLandingGearUp_RC
	};

	bco::simple_event btnLowerLandingGear_{
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


	// ** COMPONENTS **
	NavLight		lightNav_;
	Beacon			beacon_;
	Strobe			strobe_;
};

