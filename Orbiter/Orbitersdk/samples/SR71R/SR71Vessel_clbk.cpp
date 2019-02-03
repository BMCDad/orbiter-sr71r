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
#include "XRSound.h"

static const DWORD ntdvtx_geardown = 9;
static TOUCHDOWNVTX tdvtx_geardown[ntdvtx_geardown] = {
    { bt_mesh::SR71r::LandingTouchF_location,    1e6, 1e5, 1.6, 0.1 },      // front
    { bt_mesh::SR71r::LandingTouchS_location,    1e6, 1e5, 3.0, 0.2 },      // right back
    { bt_mesh::SR71r::LandingTouchP_location,    1e6, 1e5, 3.0, 0.2 },      // left back
    { bt_mesh::SR71r::TPNose_location,           1e7, 1e5, 3.0 },
    { bt_mesh::SR71r::TPHood_location,           1e7, 1e5, 3.0 },
    { bt_mesh::SR71r::TPRudder_location,         1e7, 1e5, 3.0 },
    { bt_mesh::SR71r::TPTail_location,           1e7, 1e5, 3.0 },
    { bt_mesh::SR71r::TPWingS_location,          1e7, 1e5, 3.0 },
    { bt_mesh::SR71r::TPWingP_location,          1e7, 1e5, 3.0 }
};


/**	clbkSetClassCaps
	Setup up vessel class caps.
*/
void SR71Vessel::clbkSetClassCaps(FILEHANDLE cfg)
{
	// Load mesh files:
	mainMeshHandle_ = oapiLoadMeshGlobal(SR71r_MESH_NAME);
	mainMeshIndex_ = AddMesh(mainMeshHandle_);
	SetMeshVisibilityMode(mainMeshIndex_, MESHVIS_EXTERNAL);
    SetMainMeshIndex(mainMeshIndex_);

	vcMeshHandle_ = oapiLoadMeshGlobal(SR71rVC_MESH_NAME);
	auto idx = AddMesh(vcMeshHandle_);
	SetMeshVisibilityMode(idx, MESHVIS_VC);

	// Set base vcmesh handle so components can access it.
	SetVCMeshIndex0(idx);
	SetVCMeshHandle0(vcMeshHandle_);

	// Setup ship metrics:
	SetSize(SHIP_SIZE);
	SetEmptyMass(EMPTY_MASS);
	SetPMI(PMI);
	SetCrossSections(CROSSSECTIONS);
	SetRotDrag(ROTDRAG);

	SetDockParams(bt_mesh::SR71r::DockingPort_location, _V(0, 1, 0), _V(0, 0, 1));
	
    SetTouchdownPoints(tdvtx_geardown, ntdvtx_geardown);

    SetNosewheelSteering(true);

	// Setups:
	SetupVesselComponents();

	SetupAerodynamics();

	SetCameraOffset(bt_mesh::SR71r::PilotPOV_location);

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
			airBrake_.AirBrakeSwitch().Decrement();
			return 1;
		}
	}
	else if (KEYMOD_CONTROL(kstate)) {
		switch (key) {
		case OAPI_KEY_SPACE: // open control dialog
			return 1;
		
		case OAPI_KEY_B:
			airBrake_.AirBrakeSwitch().Increment();
			return 1;

		case OAPI_KEY_1:
			propulsionController_.ThrustLimitSwitch().Toggle();
			return 1;
		}
	}
	else {
		switch (key) {
		case OAPI_KEY_G:  // "operate landing gear"
			landingGear_.LandingGearSwitch().Toggle();
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
	avionics_.Step(simt, simdt, mjd);
	cargoBayController_.Step(simt, simdt, mjd);
    canopy_.Step(simt, simdt, mjd);
	fuelCell_.Step(simt, simdt, mjd);
	hydrogenTank_.Step(simt, simdt, mjd);
	landingGear_.Step(simt, simdt, mjd);
	oxygenTank_.Step(simt, simdt, mjd);
	powerSystem_.Step(simt, simdt, mjd);
	propulsionController_.Step(simt, simdt, mjd);
	surfaceControl_.Step(simt, simdt, mjd);
	statusBoard_.Step(simt, simdt, mjd);
	airBrake_.Step(simt, simdt, mjd);
	lights_.Step(simt, simdt, mjd);
	clock_.Step(simt, simdt, mjd);
	computer_.Step(simt, simdt, mjd);
    hoverEngines_.Step(simt, simdt, mjd);
    retroEngines_.Step(simt, simdt, mjd);

    BaseVessel::clbkPostStep(simt, simdt, mjd);
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

    powerSystem_.PostCreation();
}