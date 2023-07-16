#pragma once

#include "Orbitersdk.h"

#include "bc_orbiter\BaseVessel.h"
#include "bc_orbiter\on_off_display.h"
#include "bc_orbiter\on_off_input.h"
#include "bc_orbiter\rotary_display.h"
#include "bc_orbiter\simple_event.h"
#include "bc_orbiter\transform_display.h"
#include "bc_orbiter\flat_roll.h"
#include "bc_orbiter\cryogenic_tank.h"
#include "bc_orbiter\generic_tank.h"

#include "ShipMets.h"
#include "SR71r_mesh.h"
#include "SR71r_common.h"
#include "PropulsionController.h"
#include "HUD.h"
#include "RCSSystem.h"
#include "NavModes.h"
#include "PowerSystem.h"
#include "LeftMFD.h"
//#include "RightMfd.h"
#include "CargoBayController.h"
#include "Canopy.h"
#include "APU.h"
#include "LandingGear.h"
#include "FuelCell.h"
#include "StatusBoard.h"
#include "AirBrake.h"
#include "Clock.h"
#include "Shutters.h"
#include "VesselControl.h"
#include "HoverEngines.h"
#include "RetroEngines.h"
#include "NavLights.h"
#include "Beacon.h"
#include "Strobe.h"
#include "AeroData.h"
#include "Altimeter.h"
#include "HSI.h"
#include "Airspeed.h"
#include "HydrogenTank.h"
#include "OxygenTank.h"

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
	void					clbkLoadStateEx(FILEHANDLE scn, void* vs) override;
	void					clbkSaveState(FILEHANDLE scn) override;

	// Setup
	void					SetupVesselComponents();
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
	StatusBoard				statusBoard_;

	// DRAG
	double					bDrag{ 0.0 };
	// Collections:

	// ** COMPONENTS **


	// Map components that handle config state with a key for that component.
	std::map <std::string, bco::manage_state*>		mapStateManagement_{
		  {"CLOCK",		&clock_			}
		, {"POWER",		&powerSystem_	}
		, {"AERODATA",	&aeroData_		}
		, {"CANOPY",	&canopy_		}
		, {"CARGO",		&cargobay_		}
	};

	// ***  Airspeed  *** //

	PowerSystem				powerSystem_	{ *this };

	AirBrake				airBrake_		{ *this };
	Airspeed				airspeed_		{ *this };
	Altimeter				altimeter_		{ *this };
	Clock					clock_			{ *this };
	HSI						hsi_			{ *this };
	LandingGear				landingGear_	{ *this };
	NavModes				navModes_		{ *this };
	RCSSystem				rcs_			{ *this };
	Shutters				shutters_		{ *this };
	Beacon					beacon_			{ powerSystem_ };
	Strobe					strobe_			{ powerSystem_ };
	NavLight				navLight_		{ powerSystem_ };
	Canopy					canopy_			{ powerSystem_ };
	CargoBayController		cargobay_		{ powerSystem_ };
	AeroData				aeroData_		{ powerSystem_, *this };
	HoverEngines			hoverEngines_	{ powerSystem_,	*this };
	RetroEngines			retroEngines_	{ powerSystem_,	*this };
	HydrogenTank			hydrogenTank_	{ powerSystem_, *this };
	OxygenTank				oxygenTank_		{ powerSystem_, *this };
	FuelCell				fuelCell_		{ powerSystem_, *this, oxygenTank_,	hydrogenTank_ };
	bco::generic_tank		mainFuelTank_	{ powerSystem_, MAX_FUEL,		FUEL_TRANFER_RATE };
	bco::generic_tank		rcsFuelTank_	{ powerSystem_, MAX_RCS_FUEL,	FUEL_TRANFER_RATE };
	APU						apu_			{ powerSystem_, mainFuelTank_ };
	HUD						headsUpDisplay_	{ powerSystem_, *this };
	PropulsionController	propulsion_		{ powerSystem_, *this };

	LeftMFD					mfdLeft_		{ powerSystem_, this };

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

	// Surface stuff:
	bco::slot<double>		slotHydraulicLevel_;

	CTRLSURFHANDLE	ctrlSurfLeftAileron_;
	CTRLSURFHANDLE	ctrlSurfRightAileron_;
	CTRLSURFHANDLE	ctrlSurfLeftElevator_;
	CTRLSURFHANDLE	ctrlSurfRightElevator_;
	CTRLSURFHANDLE	ctrlSurfLeftRudder_;
	CTRLSURFHANDLE	ctrlSurfRightRudder_;
	CTRLSURFHANDLE	ctrlSurfLeftTrim_;
	CTRLSURFHANDLE	ctrlSurfRightTrim_;

	int			anim_left_aileron_;
	int			anim_left_elevator_;
	int			anim_right_aileron_;
	int			anim_right_elevator_;
	int			anim_left_rudder_;
	int			anim_right_rudder_;

	void SetupSurfaces();

	void UpdateHydraulicLevel(double v) {
		if (v == 1.0) {
			EnableSurfaceControls();
		}
		else {
			DisableControls();
		}
	}

	void EnableSurfaceControls() {
		
		// Aileron control : bank left/right.  For this we will just use the outer elevon area.
		ctrlSurfLeftAileron_ = this->CreateControlSurface3(AIRCTRL_AILERON, OutboardElevonArea / 2, dClOutboard, _V(-5.0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_left_aileron_);
		ctrlSurfRightAileron_ = this->CreateControlSurface3(AIRCTRL_AILERON, OutboardElevonArea / 2, dClOutboard, _V(5.0, 0, -8.0), AIRCTRL_AXIS_AUTO, 2.0, anim_right_aileron_);

		// Elevator control : pitch up/down.  For this we use the combined inner and outer area.
		auto fullArea = OutboardElevonArea + InboardElevonArea;
		auto fulldc = dClInboard + dClOutboard;
		ctrlSurfLeftElevator_ = this->CreateControlSurface3(AIRCTRL_ELEVATOR, fullArea, fulldc, _V(-5.0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_left_elevator_);
		ctrlSurfRightElevator_ = this->CreateControlSurface3(AIRCTRL_ELEVATOR, fullArea, fulldc, _V(5.0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_right_elevator_);

		// Rudder contro : yaw.
		ctrlSurfLeftRudder_ = this->CreateControlSurface3(AIRCTRL_RUDDER, RudderArea, dClRudder, _V(0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_left_rudder_);
		ctrlSurfRightRudder_ = this->CreateControlSurface3(AIRCTRL_RUDDER, RudderArea, dClRudder, _V(0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_right_rudder_);

		// Trim : SR-71 does not have trim tabs, or flaps.  We will use the inBoard area and generall as smaller dCl to provide finer control.
		ctrlSurfLeftTrim_ = this->CreateControlSurface3(AIRCTRL_ELEVATORTRIM, InboardElevonArea, dClInboard, _V(-5.0, 0, -8.0), AIRCTRL_AXIS_XPOS, 1.0, anim_left_elevator_);
		ctrlSurfRightTrim_ = this->CreateControlSurface3(AIRCTRL_ELEVATORTRIM, InboardElevonArea, dClInboard, _V(5.0, 0, -8.0), AIRCTRL_AXIS_XPOS, 1.0, anim_right_elevator_);
	}

	void DisableControls()
	{
		this->DelControlSurface(ctrlSurfLeftAileron_);
		this->DelControlSurface(ctrlSurfRightAileron_);
		this->DelControlSurface(ctrlSurfLeftElevator_);
		this->DelControlSurface(ctrlSurfRightElevator_);
		this->DelControlSurface(ctrlSurfLeftRudder_);
		this->DelControlSurface(ctrlSurfRightRudder_);
		this->DelControlSurface(ctrlSurfLeftTrim_);
		this->DelControlSurface(ctrlSurfRightTrim_);
	}

};

