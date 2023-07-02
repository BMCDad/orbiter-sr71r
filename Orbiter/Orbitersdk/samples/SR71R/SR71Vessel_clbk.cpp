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

static const DWORD ntdvtx_geardown = 9;
static TOUCHDOWNVTX tdvtx_geardown[ntdvtx_geardown] = {
    { bm::main::LandingTouchF_location,    1e6, 1e5, 1.6, 0.1 },      // front
    { bm::main::LandingTouchS_location,    1e6, 1e5, 3.0, 0.2 },      // right back
    { bm::main::LandingTouchP_location,    1e6, 1e5, 3.0, 0.2 },      // left back
    { bm::main::TPNose_location,           1e7, 1e5, 3.0 },
    { bm::main::TPHood_location,           1e7, 1e5, 3.0 },
    { bm::main::TPRudder_location,         1e7, 1e5, 3.0 },
    { bm::main::TPTail_location,           1e7, 1e5, 3.0 },
    { bm::main::TPWingS_location,          1e7, 1e5, 3.0 },
    { bm::main::TPWingP_location,          1e7, 1e5, 3.0 }
};


/**	clbkSetClassCaps
	Setup up vessel class caps.
*/
void SR71Vessel::clbkSetClassCaps(FILEHANDLE cfg)
{
	// Load global mesh files.  These are the 'template' meshes Orbiter will use.
	// You use these mesh handles to the AddMesh the meshes and set how they will be viewed.
	// If you ever need this handle again, call oapiLoadMeshGlobal with the same name and it
	// we be returned, the mesh will not be reloaded.
	auto mainGlobalMesh = oapiLoadMeshGlobal(bm::main::MESH_NAME);
	mainMeshIndex_ = AddMesh(mainGlobalMesh);
	SetMeshVisibilityMode(mainMeshIndex_, MESHVIS_EXTERNAL);
    SetMainMeshIndex(mainMeshIndex_);  // The index will be needed when adding animations to the mesh.

	// In case of the VC mesh we hold on to the mesh handle as it will be needed to get the texture
	// that will be modified for things like MFD font painting.
	vcMeshHandle_ = oapiLoadMeshGlobal(bm::vc::MESH_NAME);
	auto idx = AddMesh(vcMeshHandle_);
	SetMeshVisibilityMode(idx, MESHVIS_VC);
	SetVCMeshIndex0(idx);
	SetVCMeshHandle0(vcMeshHandle_);

	// Load 2D Panel:
	auto panelMeshHandle = oapiLoadMeshGlobal(bm::pnl::MESH_NAME);
	SetPanelMeshHandle0(panelMeshHandle);
	
		// Setup ship metrics:
	SetSize(SHIP_SIZE);
	SetEmptyMass(EMPTY_MASS);
	SetPMI(PMI);
	SetCrossSections(CROSSSECTIONS);
	SetRotDrag(ROTDRAG);

	SetDockParams(bm::main::DockingPort_location, _V(0, 1, 0), _V(0, 0, 1));
	
    SetTouchdownPoints(tdvtx_geardown, ntdvtx_geardown);
    SetNosewheelSteering(true);


	SetNosewheelSteering(true);

	// Setups:
	SetupVesselComponents();

	SetupAerodynamics();

	SetCameraOffset(bm::main::PilotPOV_location);

    // Propellent, move to setup method:
    CreateMainPropellant(MAX_FUEL);
    CreateRcsPropellant(MAX_RCS_FUEL);

	bco::BaseVessel::clbkSetClassCaps(cfg);

	SetMaxWheelbrakeForce(4e5);
}

int SR71Vessel::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate)
{
	if (!down) return 0; // only process keydown events
	if (Playback()) return 0; // don't allow manual user input during a playback

	if (KEYMOD_SHIFT(kstate)) {
	}
	else if (KEYMOD_ALT(kstate)) {
		switch (key)
		{
		case OAPI_KEY_B:
			btnDecreaseAirbrake_.Signal().fire(true);
			return 1;
		}
	}
	else if (KEYMOD_CONTROL(kstate)) {
		switch (key) {
		case OAPI_KEY_SPACE: // open control dialog
			return 1;
		
		case OAPI_KEY_B:
			btnIncreaseAirbrake_.Signal().fire(true);
			return 1;

		case OAPI_KEY_1:
			//propulsionController_.ThrustLimitSwitch().Toggle();
			switchThrustLimit_.Signal().fire(!switchThrustLimit_.GetState());
			return 1;
		}
	}
	else {
		switch (key) {
		case OAPI_KEY_G:  // "operate landing gear"
			//landingGear_.LandingGearSwitch().Toggle();
			return 1;

		case OAPI_KEY_1:	// Main auto pilot toggle.
			computer_.APMainButton().Push();
			return 1;

		case OAPI_KEY_2:	// Toggle hold heading
			computer_.APHeadingButton().Push();
			return 1;

		case OAPI_KEY_3:
			computer_.APAltitudeButton().Push();
			return 1;

		case OAPI_KEY_4:
			computer_.APKEASButton().Push();
			return 1;

		case OAPI_KEY_5:
			computer_.APMACHButton().Push();
			return 1;

		//case OAPI_KEY_6:
		//	autoPilot_.HoldAttitudeSwitch().Toggle();
		//	return 1;
		}
	}
	return 0;
}


bool SR71Vessel::clbkLoadVC(int id)
{
	SetCameraMovement(
		_V(0.0, -0.13, 0.08), 0.0, -0.13,
		_V(-0.1, 0.0, 0.0), 0.0, 0.0,
		_V(0.1, 0.0, 0.0), 0.0, 0.0);

	return BaseVessel::clbkLoadVC(id);
}

void SR71Vessel::clbkHUDMode(int mode)
{
	headsUpDisplay_.OnHudMode(mode);
}

void SR71Vessel::clbkRCSMode(int mode)
{
	rcsSystem_.OnRCSMode(mode);
}

void SR71Vessel::clbkNavMode(int mode, bool active)
{
	navModes_.OnNavMode(mode, active);
}

void SR71Vessel::clbkMFDMode(int mfd, int mode)
{
	mfdLeft_.OnMfdMode(mfd, mode);
	mfdRight_.OnMfdMode(mfd, mode);
}

void SR71Vessel::clbkPostStep(double simt, double simdt, double mjd)
{
	apu_.Step(simt, simdt, mjd);
//	avionics_.Step(simt, simdt, mjd);
	cargoBayController_.Step(simt, simdt, mjd);
    canopy_.Step(simt, simdt, mjd);
	fuelCell_.Step(simt, simdt, mjd);
	hydrogenTank_.Step(simt, simdt, mjd);
	landingGear_.Step(simt, simdt, mjd);
	oxygenTank_.Step(simt, simdt, mjd);
//	powerSystem_.Step(simt, simdt, mjd);
	propulsionController_.Step(simt, simdt, mjd);
	surfaceControl_.Step(simt, simdt, mjd);
	statusBoard_.Step(simt, simdt, mjd);
	airBrake_.Step(simt, simdt, mjd);
//	lights_.Step(simt, simdt, mjd);
	clock_.Step(simt, simdt, mjd);
	computer_.Step(simt, simdt, mjd);
    hoverEngines_.Step(simt, simdt, mjd);
    retroEngines_.Step(simt, simdt, mjd);

    BaseVessel::clbkPostStep(simt, simdt, mjd);

	powerSystem_.PostAmpStep();		// Handles all amp draws for time step.
}

bool SR71Vessel::clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
	headsUpDisplay_.DrawHUD(mode, hps, skp);
    landingGear_.DrawHUD(mode, hps, skp);
//    autoPilot_.DrawHUD(mode, hps, skp);
	computer_.DrawHUD(mode, hps, skp);
    navModes_.DrawHUD(mode, hps, skp);
    propulsionController_.DrawHUD(mode, hps, skp);
    hoverEngines_.DrawHUD(mode, hps, skp);
    retroEngines_.DrawHUD(mode, hps, skp);

	return VESSEL3::clbkDrawHUD(mode, hps, skp);
}

void SR71Vessel::clbkPostCreation()
{
	BaseVessel::clbkPostCreation();
}

bool SR71Vessel::clbkLoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
{
	/*	Panel definition tasks (panel 0):
	*	viewW is the width of the Orbiter window (view port).
	*	viewH is the height of the Orbiter window (view port).  These values are used to scale the panel.
	*
		- SetPanelBackground
		- MFD setup
		- SetPanelScale
		- Set neighbours: oapiSetPanelNeighbours (-1,-1,1,-1);
		- SetCameraDefaultDirection (_V(0,0,1)); // forward
		- oapiCameraSetCockpitDir (0,0);

		Other panels referenced from oapiSetPanelNeighbours will come through this call those ids.
	*/

	/*
	* Orbiter expects the mesh size to be
	*/
	SetPanelBackground(
		hPanel,
		0,
		0,
		GetpanelMeshHandle0(),					// Handle to the panel mesh.
		(DWORD)bm::pnl::MainPanel_Width,		// Panel mesh width (mesh units)
		(DWORD)bm::pnl::MainPanel_Height,		// Panel mesh height (mesh units)
		(DWORD)bm::pnl::MainPanel_Height / 5,	// Baseline (adjust to move panel)
		PANEL_ATTACH_BOTTOM | PANEL_MOVEOUT_BOTTOM);

	// Example: viewW = 3000 and panel width = 2000 (panel too small), defscale becomes (3000/2000) = 1.5 to fit the window.
	//			viewW = 1000 and panel width = 2000 (panel too big), defscale becomes (1000/2000) = .5 shrink panel to fit.
	double defscale = (double)viewW / bm::pnl::MainPanel_Width;
	double extscale = max(defscale, 1.0);
	SetPanelScaling(hPanel, defscale, extscale);

	return BaseVessel::clbkLoadPanel2D(id, hPanel, viewW, viewH);
}
