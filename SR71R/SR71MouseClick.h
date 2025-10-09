/*
SR71 Mouse Click - SR-71r Orbiter Addon
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

SR71 Toggle Switch represents a physical on / off toggle in the SR71
cockpit.  It uses an animation for the VC representation, and a static
texture image for the 2D cockpits.  It maintains a simple state of
on or off.

Use:
Include this file in the component source.

*/
#pragma once

#include <vector>
#include <limits>
#include <functional>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\MeshTools.h"
#include "..\bc_orbiter\Tools.h"
#include "..\bc_orbiter\IUIControl.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"

namespace bco = bc_orbiter;

namespace SR71
{
    using OnMouseCick = std::function<void()>;

    class MouseClick : public bco::IUIControl
    {
    public:
        MouseClick(
            const VECTOR3& vcLocation,
            const double vcHitRadius,
            const RECT& pnlRect,
            const int pnlId,
            OnMouseCick onClick = nullptr
        );

        // IUIControl interface
        void Setup(bco::Vessel& vessel) override;
        void LoadVC(int vcId, bco::Vessel& vessel) override;
        void LoadPanel(int panelId, bco::Vessel& vessel, PANELHANDLE handle);

    private:
        bco::Vessel* vessel_{ nullptr };

        const VECTOR3&  vcLocation_;
        RECT            pnlRect_;
        int             pnlId_;
        double          vcHitRadius_{ 0.01 };
        int             eventId_{ -1 };

        MESHHANDLE meshPanel_{ nullptr };
        DEVMESHHANDLE vcMesh_{ nullptr };

        OnMouseCick OnClick_{ nullptr };

        bool OnMouseEvent(int eventId, int eventType);
    };

    inline MouseClick::MouseClick(
        const VECTOR3& vcLocation,
        const double vcHitRadius,
        const RECT& pnlRect,
        const int pnlId,
        OnMouseCick onClick
    ) :
        vcLocation_(vcLocation),
        vcHitRadius_(vcHitRadius),
        pnlRect_(pnlRect),
        pnlId_(pnlId),
        OnClick_(onClick)
    {
    }

    inline void MouseClick::Setup(bco::Vessel& vessel)
    {
        vessel_ = &vessel;
        // Setup the event handler.
        eventId_ = vessel.RegisterEventHandler([this](int eventId, int eventType) { return OnMouseEvent(eventId, eventType); });
    }

    inline void MouseClick::LoadVC(int vcId, bco::Vessel& vessel)
    {
        if (vcId != 0) return; // Only one VC supported for now.    
        oapiVCRegisterArea(eventId_, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
        oapiVCSetAreaClickmode_Spherical(eventId_, vcLocation_, vcHitRadius_);
    }

    inline void MouseClick::LoadPanel(int panelId, bco::Vessel& vessel, PANELHANDLE handle)
    {
        if (panelId != pnlId_) return;
        vessel.RegisterPanelArea(
            handle,
            eventId_,
            pnlRect_,
            PANEL_REDRAW_NEVER,
            PANEL_MOUSE_LBDOWN | PANEL_MOUSE_ONREPLAY);
    }

    inline bool MouseClick::OnMouseEvent(int eventId, int eventType)
    {
        if (eventType != PANEL_MOUSE_LBDOWN) return false;

        if (OnClick_ != nullptr) {
            OnClick_();
        }

        return true;
    }
}
