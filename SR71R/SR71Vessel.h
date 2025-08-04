#pragma once

#include "Orbitersdk.h"

#include "ShipMets.h"
#include "SR71r_mesh.h"
#include "SR71r_common.h"

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimationGroup.h"

#include "ClockTimer.h"

namespace bco = bc_orbiter;

class SR71Vessel : public bco::Vessel
{
public:
    SR71Vessel(OBJHANDLE hvessel, int flightmodel);
    ~SR71Vessel();

    auto SetClassCaps() -> void override;
    auto LoadVC(int id) -> bool override;
    auto LoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)  -> bool override;
    auto Step(bco::Vessel& vessel, double simt, double simdt, double mjd) -> void override;


    // Callbacks in 

    void    clbkLoadStateEx(FILEHANDLE scn, void* vs) override;
    void    clbkSaveState(FILEHANDLE scn) override;

protected:

    void    SetupAerodynamics();

private:
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
    ClockTimer          clock_;
};

