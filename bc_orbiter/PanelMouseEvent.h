/*
PanelMouseEvent - SR-71r Orbiter Addon
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

#include "OrbiterAPI.h"
#include "Vessel.h"

#include "Types.h"

namespace bc_orbiter
{
    class Vessel;

    /**
    \brief Class to model a mouse event for a 2D panel control.
    */
    class PanelMouseEvent
    {
    public:
        /**
        \brief Constructor for a PanelMouseEvent control.
        \param rect The RECT structure defining the mouse event area.
        \param panelId The panel ID (main or right) that this control is part of.
        \param onChanged The function to call when the mouse event occurs.
        */
        PanelMouseEvent(
            const RECT& rect,
            int panelId,
            FuncOnChanged onChanged) :
            rect_(rect),
            panelId_(panelId),
            OnChanged_(onChanged)
        {}

        /**
        \brief Called during the setup of the implementing control.
        \param vessel The vessel the UI control is part of.
        */
        virtual void Setup(Vessel& vessel)
        {
            eventId_ = vessel.RegisterEventHandler([this](int eventId, int eventType) { return OnMouseEvent(eventId, eventType); });
        }

        /**
        \brief Called when a 2D panel is loading.  Does nothing if the panel ID does not match.
        \param vessel The vessel the UI control is part of.
        \param panelId The ID of the panel that is loading.
        \param handle The panel handle.
        */
        void LoadPanel(Vessel& vessel, int panelId, PANELHANDLE handle)
        {
            if (panelId != panelId_) return;

            vessel.RegisterPanelArea(
                handle,
                eventId_,
                rect_,
                PANEL_REDRAW_NEVER,
                PANEL_MOUSE_LBDOWN);
        }

    private:
        int             eventId_{ -1 };
        RECT            rect_;
        int             panelId_{ -1 };
        FuncOnChanged   OnChanged_{ nullptr };

        bool OnMouseEvent(int eventId, int eventType)
        {
            if (eventType != PANEL_MOUSE_LBDOWN) return false;

            if (nullptr != OnChanged_)
            {
                OnChanged_();
                return true;
            }
            return false;
        }
    };
}
