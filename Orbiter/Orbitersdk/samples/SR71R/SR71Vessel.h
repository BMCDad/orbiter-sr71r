#pragma once

#include "Orbitersdk.h"

#include "bc_orbiter\BaseVessel.h"

#include "ShipMets.h"
#include "SR71r_mesh.h"
#include "O2Supply.h"
#include "Avionics.h"
#include "SurfaceController.h"
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
#include "HydrogenSupply.h"
#include "FuelCell.h"
#include "StatusBoard.h"
#include "AirBrake.h"
#include "Lights.h"
#include "Clock.h"
#include "Shutters.h"
#include "FlightComputer.h"
#include "VesselControl.h"
#include "HoverEngines.h"
#include "RetroEngines.h"

#include "NavLights.h"

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
	UINT					mainMeshIndex_;

	MESHHANDLE				vcMeshHandle_;
	DEVMESHHANDLE			meshVirtualCockpit_;


	// Components:
	APU						apu_;
//	AutoPilot				autoPilot_;
	Avionics				avionics_;
	CargoBayController		cargoBayController_;
    Canopy                  canopy_;
	FuelCell				fuelCell_;
	HUD						headsUpDisplay_;
	HydrogenSupply			hydrogenTank_;
	LandingGear				landingGear_;
	LeftMFD					mfdLeft_;
	RightMFD				mfdRight_;
	NavModes				navModes_;
	O2Supply				oxygenTank_;
	PowerSystem				powerSystem_;
	PropulsionController	propulsionController_;
	RCSSystem				rcsSystem_;
	SurfaceController		surfaceControl_;
	StatusBoard				statusBoard_;
	AirBrake				airBrake_;
	Lights					lights_;
	Clock					clock_;
	Shutters				shutters_;
	FC::FlightComputer		computer_;
    HoverEngines            hoverEngines_;
    RetroEngines            retroEngines_;

	// DRAG
	double					bDrag;
	// Collections:

	// TRY ADDING A NEW CONTROL


	// ** TOGGLE SWITCHES **

	/* Control data for on/off up/down physical toggle switches */
	bco::ControlData toggleOnOff {
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

	bco::OnOffToggle		toggleMainPower {		// On off switch for the main power supply
		GetControlId(),
		{ bm::vc::swMainPower_id },
		bm::vc::swMainPower_location, bm::vc::PowerTopRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlPwrMain_id,
		bm::pnl::pnlPwrMain_verts,
		bm::pnl::pnlPwrMain_RC
	};

	bco::OnOffToggle		togglePowerConnectionExternal {	// On off switch for connect to external power
		GetControlId(),
		{ bm::vc::swConnectExternalPower_id },
		bm::vc::swConnectExternalPower_location, bm::vc::PowerBottomRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlPwrExtBus_id,
		bm::pnl::pnlPwrExtBus_verts,
		bm::pnl::pnlPwrExtBus_RC
	};

	bco::OnOffToggle		togglePowerConnectionFuelCell {	// On off switch for connect to fuel cell
		GetControlId(),
		{ bm::vc::swConnectFuelCell_id },
		bm::vc::swConnectFuelCell_location, bm::vc::PowerBottomRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlPwrFCBus_id,
		bm::pnl::pnlPwrFCBus_verts,
		bm::pnl::pnlPwrFCBus_RC
	};

	bco::OnOffToggle		toggleNavigationLights {		// On off switch for external navigation lights.
		GetControlId(),
		{ bm::vc::SwitchNavLights_id },
		bm::vc::SwitchNavLights_location, bm::vc::LightsRightAxis_location,
		toggleOnOff,
		bm::pnl::pnlLightNav_id,
		bm::pnl::pnlLightNav_verts,
		bm::pnl::pnlLightNav_RC
	};

	// *** GAUGES ***
	bco::Gauge				gaugePowerVolts_{
		powerSystem_.VoltProvider(),
		{ bm::vc::gaugeVoltMeter_id },
		bm::vc::gaugeVoltMeter_location, bm::vc::VoltMeterFrontAxis_location,
		bm::pnl::pnlVoltMeter_id,
		bm::pnl::pnlVoltMeter_verts,
		-(120 * RAD),
		0.2
	};

	bco::Gauge				gaugePowerAmps_{
		powerSystem_.AmpProvider(),
		{ bm::vc::gaugeAmpMeter_id },
		bm::vc::gaugeAmpMeter_location, bm::vc::VoltMeterFrontAxis_location,
		bm::pnl::pnlAmpMeter_id,
		bm::pnl::pnlAmpMeter_verts,
		(120 * RAD),	// Clockwise
		0.2
	};


	// *** POWER STATUS LIGHTS:
	bco::StatusLight		lightFuelCellAvail_{
		powerSystem_.FuelCellAvailableProvider(),
		GetControlId(),
		bm::vc::FuelCellAvailableLight_id,
		bm::vc::FuelCellAvailableLight_verts,
		bm::pnl::pnlLgtFCPwrAvail_id,
		bm::pnl::pnlLgtFCPwrAvail_verts,
		0.0244 
	};

	// ** COMPONENTS **
	NavLight		lightNav_{ toggleNavigationLights };
};

