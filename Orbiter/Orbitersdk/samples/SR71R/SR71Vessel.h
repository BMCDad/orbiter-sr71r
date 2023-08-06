#pragma once

#include "Orbitersdk.h"

#include "bc_orbiter/vessel.h"
#include "bc_orbiter/on_off_display.h"
#include "bc_orbiter/on_off_input.h"
#include "bc_orbiter/rotary_display.h"
#include "bc_orbiter/simple_event.h"
#include "bc_orbiter/transform_display.h"
#include "bc_orbiter/flat_roll.h"
#include "bc_orbiter/generic_tank.h"
#include "bc_orbiter/status_display.h"

#include "ShipMets.h"
#include "SR71r_mesh.h"
#include "SR71r_common.h"
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
#include "AirBrake.h"
#include "Clock.h"
#include "Shutters.h"
#include "VesselControl.h"
#include "HoverEngines.h"
#include "RetroEngines.h"
#include "NavLights.h"
#include "Beacon.h"
#include "Strobe.h"
#include "Avionics.h"
#include "Altimeter.h"
#include "HSI.h"
#include "Airspeed.h"
#include "HydrogenTank.h"
#include "OxygenTank.h"
#include "FlightComputer.h"
#include "SurfaceController.h"

#include <vector>
#include <map>


class SR71Vessel : public bco::vessel
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
	void					clbkLoadStateEx(FILEHANDLE scn, void* vs) override;
	void					clbkSaveState(FILEHANDLE scn) override;

	// Setup
	void					SetupAerodynamics();

	MESHHANDLE				VCMeshHandle() { return vcMeshHandle_; }

	// Interfaces:
	PropulsionController*	GetPropulsionControl() { return &propulsion_; }

private:
	UINT					mainMeshIndex_{ 0 };

	MESHHANDLE				vcMeshHandle_{ nullptr };
	DEVMESHHANDLE			meshVirtualCockpit_{ nullptr };


	// OLD Components:
//	LeftMFD					mfdLeft_;
//	RightMFD				mfdRight_;
//	StatusBoard				statusBoard_;

	// DRAG
	double					bDrag{ 0.0 };
	// Collections:

	// ** COMPONENTS **


	// ***  Airspeed  *** //

	PowerSystem				powerSystem_	{ *this };
	APU						apu_			{ *this, powerSystem_ };

	AirBrake				airBrake_		{ *this, apu_ };
	LandingGear				landingGear_	{ *this, apu_ };
	SurfaceController		surfaceCtrl_	{ *this, apu_ };
	FC::FlightComputer		computer_		{ *this, powerSystem_ };

	Airspeed				airspeed_		{ *this };
	Altimeter				altimeter_		{ *this };
	Clock					clock_			{ *this };
	HSI						hsi_			{ *this };
	NavModes				navModes_		{ *this };
	RCSSystem				rcs_			{ *this };
	Shutters				shutters_		{ *this };
	Avionics				avionics_		{ powerSystem_, *this };
	PropulsionController	propulsion_		{ powerSystem_, *this };
	Beacon					beacon_			{ powerSystem_, *this };
	Strobe					strobe_			{ powerSystem_, *this };
	NavLight				navLight_		{ powerSystem_, *this };
	Canopy					canopy_			{ powerSystem_, *this };
	CargoBayController		cargobay_		{ powerSystem_, *this };
	HoverEngines			hoverEngines_	{ powerSystem_,	*this };
	RetroEngines			retroEngines_	{ powerSystem_,	*this };
	HydrogenTank			hydrogenTank_	{ powerSystem_, *this };
	OxygenTank				oxygenTank_		{ powerSystem_, *this };
	FuelCell				fuelCell_		{ powerSystem_, *this, oxygenTank_,	hydrogenTank_ };
	//bco::generic_tank		mainFuelTank_	{ powerSystem_, MAX_FUEL,		FUEL_TRANFER_RATE };
	//bco::generic_tank		rcsFuelTank_	{ powerSystem_, MAX_RCS_FUEL,	FUEL_TRANFER_RATE };
	HUD						headsUpDisplay_	{ powerSystem_, *this };

	LeftMFD					mfdLeft_		{ powerSystem_, this };
	RightMFD				mfdRight_		{ powerSystem_, this };


	// Map components that handle config state with a key for that component.
	std::map <std::string, bco::manage_state*>		mapStateManagement_{
		  { "AIRBRAKE",		&airBrake_		}		// [a b]		: (a)Switch position,  (b)Brake position
		, { "APU",			&apu_			}		// [a]			: (a)Enabled switch
		, { "AVIONICS",		&avionics_		}		// [a b c d e]	: (a)Set course, (b)Set heading, (c)power, (d)Mode switch[1=atmo], (c)Nav select
		, { "BEACON",		&beacon_		}		// [a]			: (a)Power
		, { "COMPUTER",		&computer_		}		// [a b c d e]	: (a)Atmo on, (b)Hold heading, (c)Hold altitude, (d)Hold KEAS, (e)Hold MACH
		, { "CANOPY",		&canopy_		}		// [a b c]		: (a)Power, (b)Switch, (c)canopy position
		, { "CARGO",		&cargobay_		}		// [a b c]		: (a)Power, (b)Switch, (c)Cargo doors position
		, { "CLOCK",		&clock_			}		// [a b c]		: (a)Elapsed mission, (b)Is timer running, (c)Elapsed timer.
		, { "FUELCELL",		&fuelCell_		}		// [a]			: (a)Power
		, { "GEAR",			&landingGear_	}		// [a b]		: (a)Switch position, (b)Landing gear position.
		, { "HOVER",		&hoverEngines_	}		// [a b]		: (a)Switch position, (b)Door position
		, { "LH2",			&hydrogenTank_	}		// [a b]		: (a)Quantity(liters 10max), (b)Is filling
		, { "LOX",			&oxygenTank_	}		// [a b]		: (a)Quantity(liters 20max), (b)Is filling
		, { "NAVLIGHTS",	&navLight_		}		// [a]			: (a)Power
		, { "POWER",		&powerSystem_	}		// [a b c]		: (a)Main power switch, (b)External connected, (c)Fuelcell connected.
		, { "PROPULSION",	&propulsion_	}		// [a]			: (a)Thrust limit switch
		, { "RETRO",		&retroEngines_	}		// [a b]		: (a)Switch position, (b)Door position
		, { "STROBE",		&strobe_		}		// [a]			: (a)Power
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

	// Put status here that does not go anywhere else.
	bco::status_display     statusDock_	{	bm::vc::MsgLightDock_id,
											bm::vc::MsgLightDock_verts,
											bm::pnl::pnlMsgLightDock_id,
											bm::pnl::pnlMsgLightDock_verts,
											0.0361
										};

};

