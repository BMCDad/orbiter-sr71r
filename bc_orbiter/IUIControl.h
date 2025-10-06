/*
IUIController - SR-71r Orbiter Addon
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
#pragma once

#include "Vessel.h"
#include "MeshTools.h"

#include <vector>

namespace bc_orbiter
{
    class Vessel;

    /**
    /brief Interface for updating the VC UI when the VC is active.
    */
    struct ITimeStepVC
    {
        /**
        /brief Called during the simulation step to update the VC UI when the VC is active.
        /param vessel The vessel the UI control is part of.
        /param simdt The time step of the simulation.
        /param VCID The ID of the VC that is active.
        /param hMesh The mesh handle for the VC mesh.
        */
        virtual void TimeStepVC(Vessel& vessel, double simdt, DEVMESHHANDLE hMesh) { return; }
    };

    struct ITimeStepPanel
    {
        /**
        /brief Called during the simulation step to update the 2D panel UI when the panel is active.
        /param vessel The vessel the UI control is part of.
        /param simdt The time step of the simulation.
        /param panelId The ID of the panel that is active.
        /param hMesh The mesh handle for the panel mesh.
        */
        virtual void TimeStepPanel(Vessel& vessel, double simdt, int panelId, MESHHANDLE hMesh) { return; }
    };

    class IUIControl
    {
    public:
        /**
        /brief Called during vessel initialization to setup the UI control.
        /param vessel The vessel the UI control is part of.
        */
        virtual void Setup(Vessel& vessel) { return; }

        /**
        /brief Called each simulation step to update the state of the UI control.
        /param vessel The vessel the UI control is part of.
        /param simdt The time step of the simulation.
         */
        virtual void TimeStep(Vessel& vessel, double simdt) { return; }

        /**
        /brief Called to update the VC UI when the VC is active.
        /param vcId The ID of the VC that is loading.
        */
        virtual void LoadVC(int vcId, Vessel& vessel) { return; }

        /**
        /brief Called when a 2D panel is loading.
        /param panelId The ID of the panel that is loading.
        /param vessel The vessel the UI control is part of.
        /param handle The panel handle.
        */
        virtual void LoadPanel(int panelId, Vessel& vessel, PANELHANDLE handle) { return; }
    };


}