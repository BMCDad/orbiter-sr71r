//	VCEventManager - SR-71r Orbiter Addon
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

#include <functional>
#include <unordered_map>

#include "OrbiterSDK.h"
#include "Types.h"


namespace bc_orbiter
{
    /* EventManager
    * Manages virtual cockpit mouse click events.
    */
    class EventManager
    {
    public:
        EventManager() = default;
        ~EventManager() = default;

        /* RegisterVCEvent
        * Registers a virtual cockpit mouse event handler.
        * This method is called during vessel initialization.
        * param location - The location of the event in the virtual cockpit.
        * param radius - The radius of the event in meters.
        * param handler - The event handler to call when the event is triggered.
        * param vcId - The virtual cockpit ID the event is associated with.  Default is 0.
        */
        int RegisterVCEvent(VECTOR3 location, double radius, std::function<void()> handler, int vcId = 0);
        
        /* RegisterPanelEvent
        * Registers a 2D panel mouse event handler.
        * This method is called during vessel initialization.
        * param rc - The rectangle of the event in screen coordinates.
        * param handler - The event handler to call when the event is triggered.
        * param panelId - The panel ID the event is associated with.  Default is 0.
        */
        int RegisterPanelEvent(const RECT& rc, std::function<void()> handler, int panelId = 0);

        /* OnLoadVC
        * This method is called from the vessel clbkLoadVC method.  It registers all events, and must be called
        * every time the virtual cockpit is loaded.
        * param panelId - The virtual cockpit ID being loaded.
        */
        void OnLoadVC(int panelId);

        /* OnLoadPanel
        * This method is called from the vessel clbkLoadPanel2D method.  It registers all panel events, and must be called
        * every time a 2D panel is loaded.
        * param vessel - The vessel the panel is being loaded for.
        * param panelId - The panel ID being loaded.
        * param hPanel - The panel handle being loaded.
        */
        void OnLoadPanel(VESSEL3& vessel, int panelId, PANELHANDLE hPanel);

        /* OnMouseEvent
        * Handler for mouse events.  This method is called from the vessel clbkVCMouseEvent and clbkPanelMouseEvent methods.
        */
        bool OnMouseEvent(int eventId);
    private:

        int nextId_{ 1 };

        struct VCEventEntry  
        {  
            int                     id              { 0 };
            VECTOR3                 location        { 0.0, 0.0, 0.0 };
            double                  radius          { 0.0 };
            std::function<void()>   eventHandler    { [](){} };  
            int                     vcId            { 0 };  
        };

        std::unordered_map<int, VCEventEntry> events_;

        struct PanelEventEntry
        {
            int                     id              { 0 };
            RECT                    rc              { 0, 0, 0, 0 };
            std::function<void()>   eventHandler    { [](){} };
            int                     panelId         { 0 };
        };

        std::unordered_map<int, PanelEventEntry> panelEvents_;
    };

    inline int EventManager::RegisterVCEvent(VECTOR3 location, double radius, std::function<void()> handler, int vcId)
    {
        int id = nextId_++;
        events_.emplace(id, VCEventEntry{ id, location, radius, handler, vcId});
        return id;
    }

    inline int EventManager::RegisterPanelEvent(const RECT& rc, std::function<void()> handler, int panelId)
    {
        int id = nextId_++;
        panelEvents_.emplace(id, PanelEventEntry{ id, rc, handler, panelId });
        return id;
    }

    inline void EventManager::OnLoadVC(int vcId)
    {
         for (const auto& [eventId, eventEntry] : events_)
         {
             if (eventEntry.vcId != vcId) continue;

             oapiVCRegisterArea(eventId, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
             oapiVCSetAreaClickmode_Spherical(eventId, eventEntry.location, eventEntry.radius);
         }
    }

    inline void EventManager::OnLoadPanel(VESSEL3& vessel, int panelId, PANELHANDLE hPanel)
    {
        for (const auto& [eventId, eventEntry] : panelEvents_)
        {
            if (eventEntry.panelId != panelId) continue;
            vessel.RegisterPanelArea(hPanel, eventId, eventEntry.rc, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
        }
    }

    inline bool EventManager::OnMouseEvent(int eventId)  
    {  
        auto it = events_.find(eventId);  
        if (it != events_.end())  
        {  
            it->second.eventHandler();  
            return true;
        }  

        return false;
    }
}
