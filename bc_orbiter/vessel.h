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
        \brief Called during vessel initialization to set up class capabilities.  This can be overloaded by the
        derived class to do one time initial setup of the vessel such as loading meshes, setting up events, and
        creating animations.
        */
        virtual void SetClassCaps() {};

        /**
        \brief Called when the user switches to a virtual cockpit view. id is the ID of the virtual cockpit being
        loaded.  If the derived class uses a virtual cockpit, it should override this method and return true.
        \param id - The ID of the virtual cockpit being loaded.
        \return true if the VC has a virtual cockpit, false otherwise.
        */
        virtual bool LoadVC(int id) { return false; }

        /**
        \brief Called when the user switches to a 2D panel view. id is the ID of the panel.
        The derived class should override this method to load the panel mesh.  It should return
        true if the vessel has a 2D panel, false otherwise.
        \param id - The ID of the panel being loaded.
        \param hPanel - The handle to the panel being loaded.
        \param viewW - The width of the Orbiter window (view port).
        \param viewH - The height of the Orbiter window (view port).
        \return true if the panel is loaded successfully, false otherwise.
        */
        virtual bool LoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) { return false; }

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
        \brief Gets a unique event ID for registering events.
        */
        int GetEventId() { return nextEventId_++; } 

        /**
        \brief Registers a custom event handler.
        */
        void RegisterEventHandler(int eventId, FuncEventHandler handler);

        /**
        \brief Registers a virtual cockpit mouse event handler.  The derived class should call this
        method during vessel initialization (e.g., in SetClassCaps).
        \param location - The location of the event in the virtual cockpit.
        \param handler - The event handler to call when the event is triggered.
        \param radius - The radius of the event in meters.
        \param vcId - The virtual cockpit ID the event is associated with.  Default is 0.
        \return The ID of the registered event.
        */
        int RegisterVCEvent(VECTOR3 location, double radius, FuncEventHandler handler, int vcId = 0);

        /**
        \brief Registers a 2D panel mouse event handler.  The derived class should call this
        method during vessel initialization (e.g., in SetClassCaps).
        \param rc - The rectangle of the event in screen coordinates.
        \param handler - The event handler to call when the event is triggered.
        \param panelId - The panel ID the event is associated with.  Default is 0.
        \return The ID of the registered event.
        */
        int RegisterPanelEvent(const RECT& rc, FuncEventHandler handler, int panelId = 0);

        void RegisterVCRedrawEvent(int id, FuncRedrawEvent handler);
        void RegisterPanelRedrawEvent(int id, FuncRedrawEvent handler);

        /**
        \brief Creates a vessel animation.  This method should be called during vessel initialization (e.g., in SetClassCaps).
         It creates a new animation ID that can be used to add animation groups.  (see AddAnimationGroup).
         \return The ID of the created animation.
        */
        UINT CreateVesselAnimation();

        /**
        \brief Adds a rotating animation group to the vessel (another overload adds transition animations).  
        This associates a set of mesh groups with an animation ID.  It also manages allocating the animation 
        group pointers and their lifetime.  An animation (animationId) can have multiple groups, as well and 
        parent-child relationships between groups.
         \param animationId - The ID of the animation to add the group to.
         \param meshIndex - The index of the mesh where the animation group is located.
         \param grp - A list of mesh group IDs that will be part of the animation.
         \param locA - The location of the 'root' of the animation axis.
         \param locB - The location that along with locA defines the rotation axis vector.
         \param angle - The angle of rotation in radians.
         \param start - The point in an animation sequence where this animation starts (0 to 1).
         \param stop - The point in an animation sequence where this animation stops (0 to 1).
         \param parent - The parent animation component handle, if any.  Default is nullptr.
        */
        ANIMATIONCOMPONENT_HANDLE AddAnimationGroup(
            UINT animationId,
            int meshIndex,
            std::initializer_list<UINT> const& grp,
            const VECTOR3& locA, const VECTOR3& locB,
            double angle,
            double start, double stop,
            ANIMATIONCOMPONENT_HANDLE parent = nullptr);

        /**
        \brief Adds a translation animation group to the vessel.  This associates a set of mesh groups with an animation ID.
         This also manages allocating the animation group pointers and their lifetime.  An animation (animationId) can have
         multiple groups, as well and parent-child relationships between groups.
         \param animationId - The ID of the animation to add the group to.
         \param meshIndex - The index of the mesh where the animation group is located.
         \param grp - A list of mesh group IDs that will be part of the animation.
         \param translate - The translation vector to apply to the mesh groups.
         \param start - The point in an animation sequence where this animation starts (0 to 1).
         \param stop - The point in an animation sequence where this animation stops (0 to 1).
         \param parent - The parent animation component handle, if any.  Default is nullptr.
        */
        ANIMATIONCOMPONENT_HANDLE AddAnimationGroup(
            UINT animationId,
            int meshIndex,
            std::initializer_list<UINT> const& grp,
            const VECTOR3& translate, 
            double start, double stop,
            ANIMATIONCOMPONENT_HANDLE parent = nullptr);

        // VESSEL4 overrides
        void    clbkSetClassCaps(FILEHANDLE cfg) override;
        bool    clbkLoadVC(int id) override;
        bool    clbkVCMouseEvent(int id, int event, VECTOR3& p) override;
        bool    clbkPanelMouseEvent(int id, int event, int mx, int my, void* context) override;
        bool    clbkLoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) override;
        void    clbkPostStep(double simt, double simdt, double mjd) override;
        bool    clbkVCRedrawEvent(int id, int event, SURFHANDLE surf) override;
        bool    clbkPanelRedrawEvent(int id, int event, SURFHANDLE surf, void* context) override;

        // Speeds
        double GetKeas() const;
        double GetKias() const;

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

        //struct VCEventEntry
        //{
        //    int                 id{ 0 };
        //    VECTOR3             location{ 0.0, 0.0, 0.0 };
        //    double              radius{ 0.0 };
        //    FuncEventHandler    eventHandler{ []() {} };
        //    int                 vcId{ 0 };
        //};

        //struct PanelEventEntry
        //{
        //    int                 id{ 0 };
        //    RECT                rc{ 0, 0, 0, 0 };
        //    FuncEventHandler    eventHandler{ []() {} };
        //    int                 panelId{ 0 };
        //};

//        std::unordered_map<int, VCEventEntry> vcMouseEvent_;
//        std::unordered_map<int, PanelEventEntry> panelMouseEvents_;
        std::unordered_map<int, FuncEventHandler> eventHandlers_;

        std::unordered_map<int, FuncRedrawEvent> vcRedrawEvents_;
        std::unordered_map<int, FuncRedrawEvent> panelRedrawEvents_;

        // Animation management
        std::vector<std::unique_ptr<AnimationGroup>> animations_;
    };

    inline Vessel::Vessel(OBJHANDLE hvessel, int flightmodel) : VESSEL4(hvessel, flightmodel)
    { }

    inline Vessel::~Vessel(){}

    inline void Vessel::clbkSetClassCaps(FILEHANDLE cfg) {
        SetClassCaps();
        VESSEL4::clbkSetClassCaps(cfg);
    }

    inline bool Vessel::clbkLoadVC(int vcId) {
        return LoadVC(vcId);
    }

    inline bool Vessel::clbkVCMouseEvent(int id, int event, VECTOR3& p) {
        auto it = eventHandlers_.find(id);
         if (it != eventHandlers_.end()) {
               const auto& handler = it->second;
               return handler(id, event);
         }
         return false; // Event not handled
    }

    inline bool Vessel::clbkPanelMouseEvent(int id, int event, int mx, int my, void* context) {
        auto it = eventHandlers_.find(id);
         if (it != eventHandlers_.end()) {
               const auto& handler = it->second;
               return handler(id, event);
         }
         return false; // Event not handled
    }

    inline bool Vessel::clbkLoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) {
         return LoadPanel2D(id, hPanel, viewW, viewH);
    }

    inline void Vessel::clbkPostStep(double simt, double simdt, double mjd)
    {
        Step(*this, simdt, simdt, mjd); // Call the Step method to update the vessel's state

        VESSEL4::clbkPostStep(simt, simdt, mjd);
    }

    inline bool Vessel::clbkVCRedrawEvent(int id, int event, SURFHANDLE surf)
    {
        auto it = vcRedrawEvents_.find(id);
         if (it != vcRedrawEvents_.end())
         {
               const auto& handler = it->second;
               handler(id, event, surf);
               return true;
         }
         return false;
    }

    inline bool Vessel::clbkPanelRedrawEvent(int id, int event, SURFHANDLE surf, void* context)
    {
        auto it = panelRedrawEvents_.find(id);
         if (it != panelRedrawEvents_.end())
         {
               const auto& handler = it->second;
               handler(id, event, surf);
               return true;
         }
         return false;
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

    inline void Vessel::RegisterEventHandler(int eventId, FuncEventHandler handler)
    {
         eventHandlers_[eventId] = handler;
    }

    inline void Vessel::RegisterVCRedrawEvent(int id, FuncRedrawEvent handler)
    {
        vcRedrawEvents_[id] = handler;
    }

    inline void Vessel::RegisterPanelRedrawEvent(int id, FuncRedrawEvent handler)
    {
        panelRedrawEvents_[id] = handler;
    }

    //inline int Vessel::RegisterVCEvent(VECTOR3 location, double radius, FuncEventHandler handler, int vcId)
    //{
    //    int id = nextEventId_++;
    //    vcMouseEvent_.emplace(id, VCEventEntry{ id, location, radius, handler, vcId });
    //    return id;
    //}

    //inline int Vessel::RegisterPanelEvent(const RECT& rc, FuncEventHandler handler, int panelId)
    //{
    //    int id = nextEventId_++;
    //    panelMouseEvents_.emplace(id, PanelEventEntry{ id, rc, handler, panelId });
    //    return id;
    //}

    inline UINT Vessel::CreateVesselAnimation()
    {
        return CreateAnimation(0);
    }

    inline ANIMATIONCOMPONENT_HANDLE Vessel::AddAnimationGroup(
        UINT animationId,
        int meshIndex,
        std::initializer_list<UINT> const& grp,
        const VECTOR3& locA, const VECTOR3& locB,
        double angle,
        double start, double stop,
        ANIMATIONCOMPONENT_HANDLE parent)
    {
        animations_.emplace_back(std::make_unique<AnimationGroup>(meshIndex, grp, locA, locB, angle, start, stop));
        auto anim = animations_.back().get();
        return AddAnimationComponent(animationId, anim->Start(), anim->Stop(), anim->Transform(), parent);
    }

    inline ANIMATIONCOMPONENT_HANDLE Vessel::AddAnimationGroup(
        UINT animationId,
        int meshIndex,
        std::initializer_list<UINT> const& grp,
        const VECTOR3& translate,
        double start, double stop,
        ANIMATIONCOMPONENT_HANDLE parent)
    {
        animations_.emplace_back(std::make_unique<AnimationGroup>(meshIndex, grp, translate, start, stop));
        auto anim = animations_.back().get();
        return AddAnimationComponent(animationId, anim->Start(), anim->Stop(), anim->Transform(), parent);
    }

    inline double Vessel::GetKeas() const
    {
        const double speedOfSound = 340.29;     // Speed of sound in meters per second.
        const double msKnots = 1.94384;         // Convert m/s to knots

        return	(GetMachNumber() *               // Start with mach
            speedOfSound *                      // Multiply by speed of sound
            sqrt(GetAtmPressure() / ATMP)       // Multiply by atmo pressure ration squared.  As you get higher this goes to 0.
            ) * msKnots;                        // Convert to knots.
    }

    inline double Vessel::GetKias() const
    {
        auto result = 0.0;

        // From Heilor, Orbiter forum
        // speed of sound at sea level
        double speedOfSound = 340.29;   // m/s

        double ias = 0;
        double dynPres, mach;
        const ATMCONST* atmConst;
        OBJHANDLE atmRef = GetAtmRef();		// Get ref for current body, null if none.
        if (atmRef != NULL) {
            double staticAtmoPres = GetAtmPressure();	// static atmospheric pressure.

            // Retrieve the ratio of specific heats
            atmConst = oapiGetPlanetAtmConstants(atmRef);
            double gamma = atmConst->gamma;

            // Mach number
            mach = GetMachNumber();

            // Determine the dynamic pressure using the
            // thermal definition for stagnation pressure
            dynPres = (gamma - 1) * pow(mach, 2.0) / 2 + 1;
            dynPres = pow(dynPres, gamma / (gamma - 1));
            // Convert stagnation pressure to dynamic pressure
            dynPres = dynPres * staticAtmoPres - staticAtmoPres;

            // Following is the equation from the Orbiter manual, page 62
            ias = dynPres / ATMP + 1;
            ias = pow(ias, ((gamma - 1) / gamma)) - 1.0;
            ias = ias * 2 / (gamma - 1);
            ias = sqrt(ias) * speedOfSound;
        }

        result = ias * 1.94384; // To knots.
        return result;
    }
}
