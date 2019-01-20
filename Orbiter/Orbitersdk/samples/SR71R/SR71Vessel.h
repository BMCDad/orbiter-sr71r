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

	// Setup
	void					SetupVesselComponents();
	void					SetupAerodynamics();

	MESHHANDLE				VCMeshHandle() { return vcMeshHandle_; }

	// Interfaces:
	PropulsionController*	GetPropulsionControl() { return &propulsionController_; }

private:
	MESHHANDLE				mainMeshHandle_;
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
};

