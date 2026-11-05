#pragma once

#include "Orbitersdk.h"

#include "ShipMets.h"
#include "SR71r_mesh.h"
#include "SR71r_common.h"

#include "bc_orbiter/vessel.h"
#include "bc_orbiter/AnimationGroup.h"
#include "bc_orbiter/IUIControl.h"

#include "interfaces/IHydraulicProvider.h"

#include "systems/airframe/AirBrake.h"
#include "systems/avionics/Airspeed.h"
#include "systems/avionics/Altimeter.h"
#include "systems/power/APU.h"
#include "systems/avionics/Avionics.h"
#include "systems/airframe/Canopy.h"
#include "systems/airframe/CargoBay.h"
#include "systems/avionics/ClockTimer.h"
#include "systems/power/FuelCell.h"
#include "systems/avionics/HSI.h"
#include "systems/avionics/HUD.h"
#include "systems/propulsion/HoverEngines.h"
#include "systems/power/HydrogenTank.h"
#include "systems/airframe/LandingGear.h"
#include "systems/airframe/Lights.h"
#include "systems/avionics/MFDs.h"
#include "systems/avionics/NavModes.h"
#include "systems/power/OxygenTank.h"
#include "systems/power/PowerSystem.h"
#include "systems/propulsion/PropulsionController.h"
#include "systems/propulsion/RCSSystem.h"
#include "systems/propulsion/RetroEngines.h"
#include "systems/airframe/SurfaceController.h"

namespace bco = bc_orbiter;

class SR71Vessel : public bco::Vessel, 
    /*TEMP*/ public IHydraulicProvider
{
public:
    SR71Vessel(OBJHANDLE hvessel, int flightmodel);
    ~SR71Vessel();

    auto SetClassCaps() -> void override;
    auto LoadVC(int id) -> bool override;
    auto LoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)  -> bool override;
    auto Step(bco::Vessel& vessel, double simt, double simdt, double mjd) -> void override;

    /* TEMP HYDRAULIC */
    double Level() const override { return 1.0; } // Example pressure in psi

    // Callbacks in 

    void clbkLoadStateEx(FILEHANDLE scn, void* vs) override;
    void clbkSaveState(FILEHANDLE scn) override;
    void clbkMFDMode(int mfd, int mode) override;
    void clbkNavMode(int mode, bool active) override;
    void clbkRCSMode(int mode) override;
    void clbkHUDMode(int mode) override;
    int clbkConsumeBufferedKey(DWORD key, bool down, char* kstate) override;

private:
    void UpdateVCUI();
    void UpdateMainPanelUI();
    void SetupAerodynamics();

    std::function<void()> UIUpdateHandler_{ [&]() {} };

    // Mesh handles:
    MESHHANDLE          mainGlobalMesh_{ nullptr };
    MESHHANDLE          vcGlobalMesh_{ nullptr };
    MESHHANDLE          panelGlobalMesh_{ nullptr };
    MESHHANDLE          panelRightGlobalMesh_{ nullptr };
    
    // Mesh indices:
    UINT                vcMeshIndex_{ 0 };
    UINT                mainMeshIndex_{ 0 };

    // DRAG
    double              bDrag{ 0.0 };

    //
    AirBrake                airBrake_{ *this };
    Airspeed                airspeed_{ *this };
    Altimeter               altimeter_{ *this };
    APU                     apu_{ *this };
    Avionics                avionics_{ *this };
    Canopy                  canopy_{ *this };
    CargoBay                cargoBay_{ *this };
    ClockTimer              clock_{ *this };
    FuelCell                fuelCell_{ *this };
    HoverEngines            hoverEngines_{ *this };
    HSI                     hsi_{ *this };
    HUD                     hud_{ *this };
    HydrogenTank            hydrogenTank_{ *this };
    LandingGear             landingGear_;
    Lights                  lights_{ *this };
    MFDs                    mfds_;
    NavModes                navModes_{ *this };
    OxygenTank              oxygenTank_{ *this };
    PowerSystem             powerSystem_{ *this };
    PropulsionController    propulsion_{ *this };
    RCSSystem               rcsSystem_{ *this };
    RetroEngines            retroEngines_{ *this };
    SurfaceController       surfaceCtrl_{ *this };
};
