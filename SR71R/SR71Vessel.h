#pragma once

#include "Orbitersdk.h"

#include "ShipMets.h"
#include "SR71r_mesh.h"
#include "SR71r_common.h"

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimationGroup.h"

#include "IPowerProvider.h"
#include "IHydraulicProvider.h"
#include "IAvionics.h"
#include "IHydrogenProvider.h"
#include "ILiquidOxygenProvider.h"

#include "AirBrake.h"
#include "Airspeed.h"
#include "Altimeter.h"
#include "APU.h"
#include "Canopy.h"
#include "CargoBay.h"
#include "ClockTimer.h"
#include "FuelCell.h"
#include "HSI.h"
#include "HoverEngines.h"
#include "LandingGear.h"
#include "MFDs.h"

namespace bco = bc_orbiter;

class SR71Vessel : public bco::Vessel, 
    /*TEMP*/ public IPowerProvider, 
    /*TEMP*/ public IHydraulicProvider, 
    /*TEMP*/ public IAvionics,
    /*TEMP*/ public IFuelSystem,
    /*TEMP*/ public IHydrogenProvider,
    /*TEMP*/ public ILiquidOxygenProvider
{
public:
    SR71Vessel(OBJHANDLE hvessel, int flightmodel);
    ~SR71Vessel();

    auto SetClassCaps() -> void override;
    auto LoadVC(int id) -> bool override;
    auto LoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)  -> bool override;
    auto Step(bco::Vessel& vessel, double simt, double simdt, double mjd) -> void override;

    /* TEMP POWER */
    double GetPowerLevel() const override { return 20.0; }
    void DrawPower(double amps) override {}
    double CurrentAmps() const override { return 20.0; }

    /* TEMP HYDRAULIC */
    double Level() const override { return 1.0; } // Example pressure in psi

    /* TEMP AVIONICS */
    bool IsAeroActive() const override { return false; }
    bool IsAeroAtmoMode() const override { return false; }
    double GetSetHeading() const override { return 0.0; }
    double GetSetCourse() const override { return 0.0; }
    DWORD GetNavSource() const override { return 0; }

    /* FUEL SYSTEM*/
    double GetMainFuelLevel() const override { return 1.0; }

    /* HYDROGEN */
    double IHydrogenProvider::Draw(double amount) override { return amount; }

    /* OXYGEN */
    double ILiquidOxygenProvider::Draw(double amount) override { return amount; }

    // Callbacks in 

    void    clbkLoadStateEx(FILEHANDLE scn, void* vs) override;
    void    clbkSaveState(FILEHANDLE scn) override;
    void    clbkMFDMode(int mfd, int mode) override;

protected:

    void    SetupAerodynamics();

private:
    void UpdateVCUI();
    void UpdateMainPanelUI();
    void UpdateRightPanelUI();

    std::function<void()> UIUpdateHandler_{ [&]() {} };

    // Mesh handles:
    MESHHANDLE          mainGlobalMesh_{ nullptr };
    MESHHANDLE          vcGlobalMesh_{ nullptr };
    MESHHANDLE          panelGlobalMesh_{ nullptr };
    MESHHANDLE          panelRightGlobalMesh_{ nullptr };
    
    // Mesh indices:
    UINT                vcMeshIndex_{ 0 };
    UINT                mainMeshIndex_{ 0 };

    // Propellent
    PROPELLANT_HANDLE   mainPropellant_{ nullptr };
    PROPELLANT_HANDLE   rcsPropellant_{ nullptr };
  
    // DRAG
    double              bDrag{ 0.0 };

    //
    AirBrake            airBrake_;
    Airspeed            airspeed_;
    Altimeter           altimeter_;
    APU                 apu_;
    Canopy              canopy_;
    CargoBay            cargoBay_;
    ClockTimer          clock_;
    FuelCell            fuelCell_;
    HoverEngines        hoverEngines_;
    LandingGear         landingGear_;
    MFDs                mfds_;
};

