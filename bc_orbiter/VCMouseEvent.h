/*
VCMouseEvent - SR-71r Orbiter Addon
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
    \brief Class to model a mouse event for a virtual cockpit control.
    */
    class VCMouseEvent
    {
    public:
        /**
        \brief Constructor for a VCMouseEvent control.
        \param rect The RECT structure defining the mouse event area.
        \param panelId The panel ID (main or right) that this control is part of.
        \param onChanged The function to call when the mouse event occurs.
        */
        VCMouseEvent(
            const VECTOR3& vcLocation,
            double hitRadius,
            FuncOnChanged onChanged) :
            vcLocation_(vcLocation),
            hitRadius_(hitRadius),
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
        \brief Called when a VC is loading.
        \param vessel The vessel the UI control is part of.
        */
        void LoadVC(Vessel& vessel)
        {
            oapiVCRegisterArea(eventId_, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
            oapiVCSetAreaClickmode_Spherical(eventId_, vcLocation_, hitRadius_);
        }

    private:
        int             eventId_{ -1 };
        const VECTOR3&  vcLocation_; 
        FuncOnChanged   OnChanged_{ nullptr };
        double          hitRadius_{ 0.01 };

        bool OnMouseEvent(int eventId, int eventType)
        {
            if (nullptr != OnChanged_)
            {
                OnChanged_();
                return true;
            }
            return false;
        }
    };
}

