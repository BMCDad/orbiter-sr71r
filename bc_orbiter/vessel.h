//	Vessel - Orbiter Addon
//	Copyright(C) 2025  Blake Christensen
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
#pragma once

#include "OrbiterSDK.h"

#include "Types.h"
#include "AnimationGroup.h"

#include <map>

namespace bc_orbiter
{
    class Vessel : public VESSEL4
    {
    public:
        Vessel(OBJHANDLE hvessel, int flightmodel);
        ~Vessel();

        // Orbiter callback wrappers.  These methods simplify handling some of Orbiter's
        // more common callbake methods.

        /**
        \brief Called during vessel initialization to set up class capabilities.
        */
        virtual void SetClassCaps() {};

        /**
        \brief Called when the user switches to a virtual cockpit view. id is the ID of the virtual cockpit.
        Registration of events and redraw areas must be done here, and need to be done each time the VC is loaded.
        \param id - The ID of the virtual cockpit being loaded.
        \return true if the VC is loaded successfully, false otherwise.
        */
        virtual bool LoadVC(int id) { return false; }

        /**
        \brief Called when the user switches to a 2D panel view. id is the ID of the panel.
        Registration of events and redraw areas must be done here, and need to be done each time the panel is loaded.
        \param id - The ID of the panel being loaded.
        \param hPanel - The handle to the panel being loaded.
        \param viewW - The width of the Orbiter window (view port).
        \param viewH - The height of the Orbiter window (view port).
        \return true if the panel is loaded successfully, false otherwise.
        */
        virtual bool LoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) { return false; } // Calling class must override and at least return true if using a 2D panel.

        /**
        * \brief Called during the simulation step to update the vessel's state.
        * \param vessel - Reference to the vessel base class.
        * \param simt - The current simulation time [seconds].
        * \param simdt - The time step for the simulation [seconds].
        * \param mjd - The current Modified Julian Date.
        */
        virtual void Step(Vessel& vessel, double simt, double simdt, double mjd) {};

        /**
        \brief Registers a mesh with the vessel.
        \param meshName - The name of the mesh to register.  Orbiter will load the mesh from the global mesh directory.
        \param visibility - The visibility mode for the mesh (e.g., MESHVIS_EXTERNAL, MESHVIS_VC).
        \return The index of the registered mesh.
        */
        UINT RegisterMesh(const char* meshName, WORD visibility);

        /**
        \brief Returns the index of a mesh by its name.
        \param meshName - The name of the mesh to look up.
        \return The index of the mesh, or 0 if not found.
        */
        UINT GetMeshIndex(const char* meshName) const;

        /**
        \brief Registers a mesh for a 2D panel.
        \param meshName - The name of the mesh to register.  Orbiter will load the mesh from the global mesh directory.
        \param panelId - The ID of the panel the mesh is associated with.
        */
        void RegisterPanelMesh(const char* meshName, int panelId);

        /**
        \brief Gets the mesh handle for a 2D panel by its ID.
        \param panelId - The ID of the panel to get the mesh handle for.
        \return The mesh handle for the panel, or nullptr if not found.
        */
        MESHHANDLE GetPanelMeshHandle(int panelId) const;

        // Manage events

        /**
        \brief Registers a virtual cockpit mouse event handler.
        This method is called during vessel initialization.
        \param location - The location of the event in the virtual cockpit.
        \param handler - The event handler to call when the event is triggered.
        \param radius - The radius of the event in meters.
        \param vcId - The virtual cockpit ID the event is associated with.  Default is 0.
        \return The ID of the registered event.
        */
        int RegisterVCEvent(VECTOR3 location, double radius, FuncEventHandler handler, int vcId = 0);

        /**
        \brief Registers a 2D panel mouse event handler.
        This method is called during vessel initialization.
        \param rc - The rectangle of the event in screen coordinates.
        \param handler - The event handler to call when the event is triggered.
        \param panelId - The panel ID the event is associated with.  Default is 0.
        \return The ID of the registered event.
        */
        int RegisterPanelEvent(const RECT& rc, FuncEventHandler handler, int panelId = 0);

        UINT BuildAnimation(
            int meshIndex,
            std::initializer_list<UINT> const& grp,
            const VECTOR3& locA, const VECTOR3& locB,
            double angle,
            double start, double stop);

        // VESSEL4 overrides
        void    clbkSetClassCaps(FILEHANDLE cfg) override;
        bool    clbkLoadVC(int id) override;
        bool    clbkVCMouseEvent(int id, int event, VECTOR3& p) override;
        bool    clbkPanelMouseEvent(int id, int event, int mx, int my, void* context) override;
        bool    clbkLoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) override;
        void    clbkPostStep(double simt, double simdt, double mjd) override;


        void WriterOrbiterLog(const std::string& message) const  
        {  
            // Create a temporary buffer to convert const char* to char*  
            char logMessage[256];  
            snprintf(logMessage, sizeof(logMessage), "%s", message.c_str());  
            oapiWriteLog(logMessage);  
        }

    private:
        // Mesh
        std::map<std::string, UINT> meshIndices_;
        std::map<int, MESHHANDLE> panelMeshes_;

        // Events
        int nextEventId_{ 1 };

        struct VCEventEntry
        {
            int                 id{ 0 };
            VECTOR3             location{ 0.0, 0.0, 0.0 };
            double              radius{ 0.0 };
            FuncEventHandler    eventHandler{ []() {} };
            int                 vcId{ 0 };
        };

        struct PanelEventEntry
        {
            int                 id{ 0 };
            RECT                rc{ 0, 0, 0, 0 };
            FuncEventHandler    eventHandler{ []() {} };
            int                 panelId{ 0 };
        };

        std::unordered_map<int, VCEventEntry> vcMouseEvent_;
        std::unordered_map<int, PanelEventEntry> panelMouseEvents_;

        // Animation management
        std::map<int, std::unique_ptr<AnimationGroup>> animations_;

    };

    inline Vessel::Vessel(OBJHANDLE hvessel, int flightmodel) : VESSEL4(hvessel, flightmodel)
    { }

    inline Vessel::~Vessel(){}

    inline void Vessel::clbkSetClassCaps(FILEHANDLE cfg) {
        SetClassCaps();
        VESSEL4::clbkSetClassCaps(cfg);
    }

    inline bool Vessel::clbkLoadVC(int vcId) {
         if (LoadVC(vcId)) {
               // Load VC events
               for (const auto& [eventId, entry] : vcMouseEvent_) {
                   if (entry.vcId != vcId) continue;  // Only register events for the current VC id

                   oapiVCRegisterArea(eventId, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
                   oapiVCSetAreaClickmode_Spherical(eventId, entry.location, entry.radius);
               }
               return true;
         }

         return false;
    }

    inline bool Vessel::clbkVCMouseEvent(int id, int event, VECTOR3& p) {
         auto it = vcMouseEvent_.find(id);
         if (it != vcMouseEvent_.end()) {
            const VCEventEntry& entry = it->second;
            entry.eventHandler();
            return true; // Event handled
         }
         return false; // Event not handled
    }

    inline bool Vessel::clbkPanelMouseEvent(int id, int event, int mx, int my, void* context) {
         auto it = panelMouseEvents_.find(id);
         if (it != panelMouseEvents_.end()) {
               const PanelEventEntry& entry = it->second;
               entry.eventHandler();
               return true; // Event handled
         }
         return false; // Event not handled
    }

    inline bool Vessel::clbkLoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) {
         if (LoadPanel2D(id, hPanel, viewW, viewH)) {
               // Load panel events
               for (const auto& [eventId, entry] : panelMouseEvents_) {
                   RegisterPanelArea(hPanel, eventId, entry.rc, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
               }
               return true;
         }
         return false;
    }

    inline void Vessel::clbkPostStep(double simt, double simdt, double mjd)
    {
        Step(*this, simdt, simdt, mjd); // Call the Step method to update the vessel's state

        VESSEL4::clbkPostStep(simt, simdt, mjd);
    }

    inline UINT Vessel::RegisterMesh(const char* meshName, WORD visibility)
    {
        UINT meshIndex = AddMesh(oapiLoadMeshGlobal(meshName));
        SetMeshVisibilityMode(meshIndex, visibility);
        meshIndices_[meshName] = meshIndex;
        return meshIndex;
    }

    inline UINT Vessel::GetMeshIndex(const char* meshName) const
    {
        auto it = meshIndices_.find(meshName);
        if (it != meshIndices_.end())
        {
            return it->second;
        }

        WriterOrbiterLog("GetMeshIndex: Mesh not found: " + std::string(meshName));
        return 0; // Mesh not found
    }

    inline void Vessel::RegisterPanelMesh(const char* meshName, int panelId)  
    {  
        MESHHANDLE meshHandle = oapiLoadMeshGlobal(meshName);  
        if (meshHandle)  
        {  
            panelMeshes_[panelId] = meshHandle;  
        }  
        else  
        {  
            WriterOrbiterLog("Failed to load panel mesh: " + std::string(meshName));
        }  
    }

    inline MESHHANDLE Vessel::GetPanelMeshHandle(int panelId) const
    {
         auto it = panelMeshes_.find(panelId);
         if (it != panelMeshes_.end())
         {
               return it->second;
         }
         WriterOrbiterLog("Failed to get MESHHANDLE for panel id: : " + panelId);
         return nullptr; // Panel mesh not found
    }

    inline int Vessel::RegisterVCEvent(VECTOR3 location, double radius, FuncEventHandler handler, int vcId)
    {
        int id = nextEventId_++;
        vcMouseEvent_.emplace(id, VCEventEntry{ id, location, radius, handler, vcId });
        return id;
    }

    inline int Vessel::RegisterPanelEvent(const RECT& rc, FuncEventHandler handler, int panelId)
    {
        int id = nextEventId_++;
        panelMouseEvents_.emplace(id, PanelEventEntry{ id, rc, handler, panelId });
        return id;
    }

    inline UINT Vessel::BuildAnimation(
        int meshIndex,
        std::initializer_list<UINT> const& grp,
        const VECTOR3& locA, const VECTOR3& locB,
        double angle,
        double start, double stop)
    {
        // Create the animation in Orbiter and get the ID.
        auto aid = CreateAnimation(0);

        animations_[aid] = std::make_unique<AnimationGroup>(meshIndex, grp, locA, locB, angle, start, stop);

        auto group = animations_[aid].get();
        // Add the animation to the vessel.
        AddAnimationComponent(aid, group->Start(), group->Stop(), group->Transform());

        return aid; // Return the animation ID
    }

}
