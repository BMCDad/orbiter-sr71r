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
    { bm::main::LandingTouchF_loc,    1e6, 1e5, 1.6, 0.1 },      // front
    { bm::main::LandingTouchS_loc,    1e6, 1e5, 3.0, 0.2 },      // right back
    { bm::main::LandingTouchP_loc,    1e6, 1e5, 3.0, 0.2 },      // left back
    { bm::main::TPNose_loc,           1e7, 1e5, 3.0 },
    { bm::main::TPHood_loc,           1e7, 1e5, 3.0 },
    { bm::main::TPRudder_loc,         1e7, 1e5, 3.0 },
    { bm::main::TPTail_loc,           1e7, 1e5, 3.0 },
    { bm::main::TPWingS_loc,          1e7, 1e5, 3.0 },
    { bm::main::TPWingP_loc,          1e7, 1e5, 3.0 }
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
	SetPanelMeshHandle(0, panelMeshHandle);
    panelMeshHandle = oapiLoadMeshGlobal(bm::pnlright::MESH_NAME);
    SetPanelMeshHandle(1, panelMeshHandle);

		// Setup ship metrics:
	SetSize(SHIP_SIZE);
	SetEmptyMass(EMPTY_MASS);
	SetPMI(PMI);
	SetCrossSections(CROSSSECTIONS);
	SetRotDrag(ROTDRAG);
	SetDockParams(bm::main::DockingPort_loc, _V(0, 1, 0), _V(0, 0, 1));
    SetTouchdownPoints(tdvtx_geardown, ntdvtx_geardown);
    SetNosewheelSteering(true);
	SetNosewheelSteering(true);

	// Setups:
	SetupAerodynamics();
	SetCameraOffset(bm::main::PilotPOV_loc);

    // Propellent, move to setup method:
    CreateMainPropellant(MAX_FUEL);
    CreateRcsPropellant(MAX_RCS_FUEL);

	// Controls that live in vessel : must come before the baseVessel call.
	AddControl(&statusDock_);

	bco::vessel::clbkSetClassCaps(cfg);

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
			airBrake_.DecreaseDrag();
			return 1;
		}
	}
	else if (KEYMOD_CONTROL(kstate)) {
		switch (key) {
		case OAPI_KEY_SPACE: // open control dialog
			return 1;
		
		case OAPI_KEY_B:
			airBrake_.IncreaseDrag();
			return 1;

		case OAPI_KEY_1:
			propulsion_.ToggleThrustLimit();
			return 1;
		}
	}
	else {
		switch (key) {
		case OAPI_KEY_G:  // "operate landing gear"
			landingGear_.Toggle();
			return 1;

		case OAPI_KEY_1:	// Main auto pilot toggle.
//			computer_.ToggleProgram(FCProgFlags::AtmoActive);
			return 1;

		case OAPI_KEY_2:	// Toggle hold heading
//			computer_.ToggleProgram(FCProgFlags::HoldHeading);
			return 1;

		case OAPI_KEY_3:
//			computer_.ToggleProgram(FCProgFlags::HoldAttitude);
			return 1;

		case OAPI_KEY_4:
//			computer_.ToggleProgram(FCProgFlags::HoldKEAS);
			return 1;

		case OAPI_KEY_5:
//			computer_.ToggleProgram(FCProgFlags::HoldMACH);
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

	return vessel::clbkLoadVC(id);
}

void SR71Vessel::clbkHUDMode(int mode)
{
	headsUpDisplay_.OnHudMode(mode);
}

void SR71Vessel::clbkRCSMode(int mode)
{
	rcs_.OnRCSMode(mode);
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
    vessel::clbkPostStep(simt, simdt, mjd);

	statusDock_.set_state( DockingStatus(0) == 1 ? bco::status_display::status::on : bco::status_display::status::off);
}

void SR71Vessel::clbkPostCreation()
{
	vessel::clbkPostCreation();
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

    switch (id) {
    case 0:
    {
        /*
        * Orbiter expects the mesh size to be
        */
        SetPanelBackground(
            hPanel,
            0,
            0,
            GetpanelMeshHandle(0),					// Handle to the panel mesh.
            (DWORD)bm::pnl::MainPanel_Width,		// Panel mesh width (mesh units)
            (DWORD)bm::pnl::MainPanel_Height,		// Panel mesh height (mesh units)
            0,	                                    // Baseline (adjust to move panel)
            PANEL_ATTACH_BOTTOM | PANEL_MOVEOUT_BOTTOM);

        // Example: viewW = 3000 and panel width = 2000 (panel too small), defscale becomes (3000/2000) = 1.5 to fit the window.
        //			viewW = 1000 and panel width = 2000 (panel too big), defscale becomes (1000/2000) = .5 shrink panel to fit.
        double defscale = (double)viewW / bm::pnl::MainPanel_Width;
        double extscale = max(defscale, 1.0);
        SetPanelScaling(hPanel, defscale, extscale);

        oapiSetPanelNeighbours(-1, 1, -1, -1);
        break;
    }
    case 1:
    {
        SetPanelBackground(
            hPanel,
            0,
            0,
            GetpanelMeshHandle(1),					// Handle to the panel mesh.
            (DWORD)bm::pnlright::MainPanelRight_Width,		// Panel mesh width (mesh units)
            (DWORD)bm::pnlright::MainPanelRight_Height,		// Panel mesh height (mesh units)
            0,	                                    // Baseline (adjust to move panel)
            PANEL_ATTACH_BOTTOM);

        double defscale = (double)viewW / bm::pnl::MainPanel_Width;     // Yes, this is correct so all panels have same aspect.
        double extscale = max(defscale, 1.0);
        SetPanelScaling(hPanel, defscale, extscale);
        oapiSetPanelNeighbours(0, -1, -1, -1);
        break;
    }
    }
	return vessel::clbkLoadPanel2D(id, hPanel, viewW, viewH);
}

void SR71Vessel::clbkLoadStateEx(FILEHANDLE scn, void* vs)
{
	char* line;

	while (oapiReadScenario_nextline(scn, line))
	{
		bool handled = false;
		std::istringstream ps(line);
		std::string key;
		ps >> key;
		std::string configLine;
		std::getline(ps >> std::ws, configLine);

		auto eh = mapStateManagement_.find(key);
		if (eh != mapStateManagement_.end()) {
			eh->second->handle_load_state(*this, configLine);
			handled = true;
		}

		if (!handled) {
			ParseScenarioLineEx(line, vs);
		}
	}
}

void SR71Vessel::clbkSaveState(FILEHANDLE scn)
{
	VESSEL3::clbkSaveState(scn);	// Save default state.

	for (auto& p : mapStateManagement_) {
		oapiWriteScenario_string(
			scn, 
			(char*)p.first.c_str(), 
			(char*)p.second->handle_save_state(*this).c_str());
	}
}
