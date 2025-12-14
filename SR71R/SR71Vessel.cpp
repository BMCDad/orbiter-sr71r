/*
SR-71r Orbiter Addon
Copyright(C) 2025  Blake Christensen

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/


#include "StdAfx.h"

#include "SR71Vessel.h"
#include "LiftCoeff.h"
#include "ShipMets.h"

#include "SR71Vessel.h"
#include "SR71r_mesh.h"


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

SR71Vessel::SR71Vessel(OBJHANDLE hvessel, int flightmodel) : bco::Vessel(hvessel, flightmodel)
{
}

SR71Vessel::~SR71Vessel()
{
}

/**	clbkSetClassCaps
   Setup up vessel class caps.
*/
auto SR71Vessel::SetClassCaps() -> void
{
    // Load main meshes.
    LoadMesh(bm::main::MESH_NAME, MESHVIS_EXTERNAL);
    LoadMesh(bm::vc::MESH_NAME, MESHVIS_VC);

    // Load 2D Panel meshes.
    RegisterPanelMesh(bm::pnl::MESH_NAME, 0);
    RegisterPanelMesh(bm::pnlright::MESH_NAME, 1);

    // Setup ship metrics:
    SetSize(SR71R::SHIP_SIZE);
    SetEmptyMass(SR71R::EMPTY_MASS);
    SetPMI(SR71R::PMI);
    SetCrossSections(SR71R::CROSSSECTIONS);
    SetRotDrag(SR71R::ROTDRAG);
    SetDockParams(bm::main::DockingPort_loc, _V(0, 1, 0), _V(0, 0, 1));
    SetTouchdownPoints(tdvtx_geardown, ntdvtx_geardown);
    SetNosewheelSteering(true);
    SetNosewheelSteering(true);

    // Setups:
    SetupAerodynamics();
    SetCameraOffset(bm::vc::PilotPOV_loc);

    SetMaxWheelbrakeForce(4e5);

    // Setup components:
    propulsion_.Setup(*this);
    airBrake_.Setup(*this);
    canopy_.Setup(*this);
    cargoBay_.Setup(*this);
    hoverEngines_.Setup(*this, propulsion_); // Must come after propulsion setup.
    retroEngines_.Setup(*this, propulsion_); // Must come after propulsion setup.
    landingGear_.Setup(*this);
    mfds_.Setup(*this);
    surfaceCtrl_.Setup(*this);
}

bool SR71Vessel::LoadVC(int id)
{
    SetCameraMovement(
        _V(0.0, -0.13, 0.08), 0.0, -0.13,
        _V(-0.1, 0.0, 0.0), 0.0, 0.0,
        _V(0.1, 0.0, 0.0), 0.0, 0.0);

    airBrake_.LoadVC();
    landingGear_.LoadVC();
    mfds_.LoadVC(*this);
    hud_.LoadVC();

    UIUpdateHandler_ = { [&]() { UpdateVCUI(); } };
    return true;
}

bool SR71Vessel::LoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
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

    auto meshHandle = GetPanelMeshHandle(id);

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
            meshHandle,                   // Handle to the panel mesh.
            (DWORD)bm::pnl::MainPanel_Width,    // Panel mesh width (mesh units)
            (DWORD)bm::pnl::MainPanel_Height,   // Panel mesh height (mesh units)
            0,	                                    // Baseline (adjust to move panel)
            PANEL_ATTACH_BOTTOM | PANEL_MOVEOUT_BOTTOM);

        // Example: viewW = 3000 and panel width = 2000 (panel too small), defscale becomes (3000/2000) = 1.5 to fit the window.
        //			viewW = 1000 and panel width = 2000 (panel too big), defscale becomes (1000/2000) = .5 shrink panel to fit.
        double defscale = (double)viewW / bm::pnl::MainPanel_Width;
        double extscale = max(defscale, 1.0);
        SetPanelScaling(hPanel, defscale, extscale);

        oapiSetPanelNeighbours(-1, 1, -1, -1);

        UIUpdateHandler_ = { [&]() { UpdateMainPanelUI(); } };
        panelGlobalMesh_ = meshHandle; // Store the mesh handle for later use.

        // Call components in the panel that need loading.
        airBrake_.LoadPanel(*this, hPanel);
        landingGear_.LoadPanel(*this, hPanel);
        mfds_.LoadPanel(*this, hPanel, meshHandle);
        break;
    }
    case 1:
    {
        SetPanelBackground(
            hPanel,
            0,
            0,
            meshHandle,                      // Handle to the panel mesh.
            (DWORD)bm::pnlright::MainPanelRight_Width,  // Panel mesh width (mesh units)
            (DWORD)bm::pnlright::MainPanelRight_Height, // Panel mesh height (mesh units)
            0,	                                    // Baseline (adjust to move panel)
            PANEL_ATTACH_BOTTOM);

        double defscale = (double)viewW / bm::pnl::MainPanel_Width;     // Yes, this is correct so all panels have same aspect.
        double extscale = max(defscale, 1.0);
        SetPanelScaling(hPanel, defscale, extscale);
        oapiSetPanelNeighbours(0, -1, -1, -1);

        UIUpdateHandler_ = { [&]() { } };
        panelRightGlobalMesh_ = meshHandle; // Store the mesh handle for later use.
        break;
    }
    }

    return true;
}

void SR71Vessel::Step(bco::Vessel& vessel, double simt, double simdt, double mjd)
{
    // Call the components that need access to the simulation time.
    powerSystem_.UpdateStatePreStep(vessel, simdt);

    airBrake_.UpdateState(vessel, simdt, *this);
    airspeed_.UpdateState(vessel, simdt, avionics_);
    altimeter_.UpdateState(vessel, simdt, avionics_);
    apu_.UpdateState(vessel, simdt, powerSystem_, propulsion_);
    avionics_.UpdateState(vessel, simdt, powerSystem_);
    canopy_.UpdateState(vessel, simdt, powerSystem_);
    cargoBay_.UpdateState(vessel, simdt, powerSystem_);
    clock_.UpdateState(simdt);
    hoverEngines_.UpdateState(vessel, simdt, powerSystem_);
    hud_.UpdateState(vessel, simdt, powerSystem_);
    hydrogenTank_.UpdateState(vessel, simt, simdt, powerSystem_);
    landingGear_.UpdateState(vessel, simdt, *this);
    lights_.UpdateState(vessel, simdt, powerSystem_);
    navModes_.UpdateState(vessel, simdt, avionics_);
    oxygenTank_.UpdateState(vessel, simt, simdt, powerSystem_);
    propulsion_.UpdateState(vessel, simdt, powerSystem_);
    retroEngines_.UpdateState(vessel, simdt, powerSystem_);
    surfaceCtrl_.UpdateState(vessel, simdt, *this);

    fuelCell_.UpdateState(vessel, simdt, powerSystem_, hydrogenTank_, oxygenTank_);
    // This should be last, after all power draws are done.
    powerSystem_.UpdateStatePostStep(vessel, simdt, fuelCell_);

    UIUpdateHandler_();
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

        //auto eh = mapStateManagement_.find(key);
        //if (eh != mapStateManagement_.end()) {
        //    eh->second->handle_load_state(*this, configLine);
        //    handled = true;
        //}

        if (key == "APU") {
            apu_.LoadState(configLine);
            handled = true;
        }
        else if (key == "HOVER") {
            hoverEngines_.LoadState(configLine);
            handled = true;
        }
        else if (key == "RETRO") {
            retroEngines_.LoadState(configLine);
            handled = true;
        }
        else if (key == "CARGOBAY") {
            cargoBay_.LoadState(configLine);
            handled = true;
        }
        else if (key == "CANOPY") {
            canopy_.LoadState(configLine);
            handled = true;
        }
        else if (key == "AIRBRAKE") {
            airBrake_.LoadState(configLine);
            handled = true;
        }
        else if (key == "GEAR") {
            landingGear_.LoadState(configLine);
            handled = true;
        }
        else if (key == "FUELCELL") {
            fuelCell_.LoadState(configLine);
            handled = true;
        }
        else if (key == "AVIONICS") {
            avionics_.LoadState(configLine);
            handled = true;
        }
        else if (key == "POWER") {
            powerSystem_.LoadState(configLine);
            handled = true;
        }
        else if (key == "HYDROGEN") {
            hydrogenTank_.LoadState(configLine);
            handled = true;
        }
        else if (key == "OXYGEN") {
            oxygenTank_.LoadState(configLine);
            handled = true;
        }
        else if (key == "PROPULSION") {
            propulsion_.LoadState(configLine);
            handled = true;
        }
        else if (key == "LIGHTS") {
            lights_.LoadState(configLine);
            handled = true;
        }
        else if (key == "CLOCK") {
            clock_.LoadState(configLine);
            handled = true;
        }
        else if (key == "HOVER") {
            hoverEngines_.LoadState(configLine);
            handled = true;
        }
        else if (key == "RETRO") {
            retroEngines_.LoadState(configLine);
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

    oapiWriteScenario_string(scn, "APU",        (char*)apu_.GetState().c_str());
    oapiWriteScenario_string(scn, "HOVER",      (char*)hoverEngines_.GetState().c_str());
    oapiWriteScenario_string(scn, "RETRO",      (char*)retroEngines_.GetState().c_str());
    oapiWriteScenario_string(scn, "CARGOBAY",   (char*)cargoBay_.GetState().c_str());
    oapiWriteScenario_string(scn, "CANOPY",     (char*)canopy_.GetState().c_str());
    oapiWriteScenario_string(scn, "AIRBRAKE",   (char*)airBrake_.GetState().c_str());
    oapiWriteScenario_string(scn, "FUELCELL",   (char*)fuelCell_.GetState().c_str());
    oapiWriteScenario_string(scn, "AVIONICS",   (char*)avionics_.GetState().c_str());
    oapiWriteScenario_string(scn, "POWER",      (char*)powerSystem_.GetState().c_str());
    oapiWriteScenario_string(scn, "HYDROGEN",   (char*)hydrogenTank_.GetState().c_str());
    oapiWriteScenario_string(scn, "GEAR",       (char*)landingGear_.GetState().c_str());
    oapiWriteScenario_string(scn, "OXYGEN",     (char*)oxygenTank_.GetState().c_str());
    oapiWriteScenario_string(scn, "LIGHTS",     (char*)lights_.GetState().c_str());
    oapiWriteScenario_string(scn, "CLOCK",      (char*)clock_.GetState().c_str());

    //for (auto& p : mapStateManagement_) {
    //	oapiWriteScenario_string(
    //		scn, 
    //		(char*)p.first.c_str(), 
    //		(char*)p.second->handle_save_state(*this).c_str());
    //}
}

void SR71Vessel::clbkMFDMode(int mfd, int mode)
{
    mfds_.ResetMFD(*this, mfd);
}

void SR71Vessel::clbkNavMode(int mode, bool active)
{
    navModes_.OnNavMode(mode, active);
}

void SR71Vessel::clbkRCSMode(int mode)
{
    rcsSystem_.OnRCSMode(mode);
}

void SR71Vessel::clbkHUDMode(int mode)
{
    hud_.OnHudMode(mode);
}

void SR71Vessel::SetupAerodynamics()
{
    // Aerodynamics - see notes in ShipMets file.
    CreateAirfoil3(
        LIFT_VERTICAL,
        _V(0, 0, -0.3),
        SR71R::VLiftCoeff,
        0,
        SR71R::VERT_WING_CHORD,
        SR71R::VERT_WING_AREA,
        SR71R::VERT_WING_AR);

    CreateAirfoil3(
        LIFT_HORIZONTAL,
        _V(0, 0, -4),
        SR71R::HLiftCoeff,
        0,
        SR71R::HORZ_WING_CHORD,
        SR71R::HORZ_WING_AREA,
        SR71R::HORZ_WING_AR);
}

void SR71Vessel::UpdateVCUI() 
{
    airBrake_.UpdateVCUI(*this);
    canopy_.UpdateVCUI(*this);
    landingGear_.UpdateVCUI(*this);
}

void SR71Vessel::UpdateMainPanelUI()
{
    airBrake_.UpdateMainPanelUI(panelGlobalMesh_);
    landingGear_.UpdateMainPanelUI(panelGlobalMesh_);
}