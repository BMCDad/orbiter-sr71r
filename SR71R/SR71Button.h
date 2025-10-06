/*
SR71 Button - SR-71r Orbiter Addon
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

SR71 Button represents a physical on / off button in the SR71.  The button
state is represented in both the VC and the 2D cockpit using a texture swap.

*/
#pragma once

#include <vector>
#include <limits>
#include <functional>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"
#include "..\bc_orbiter\Tools.h"
#include "..\bc_orbiter\AnimationGroup.h"
#include "..\bc_orbiter\IUIControl.h"
#include "..\bc_orbiter\VCTextureTranslate.h"
#include "..\bc_orbiter\VCMouseEvent.h"
#include "..\bc_orbiter\PanelTextureTranslate.h"
#include "..\bc_orbiter\PanelMouseEvent.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"

namespace bco = bc_orbiter;

namespace SR71
{
    using OnChanged = std::function<void(bool state)>;

    class Button : 
        public bco::IUIControl,
        public bco::VCTextureTranslate,
        public bco::VCMouseEvent,
        public bco::PanelTextureTranslate,
        public bco::PanelMouseEvent
    {
    public:
        Button(
            const UINT vcAnimGroupId,
            const VECTOR3& vcLocation,
            const NTVERTEX* vcVerts,
            const UINT pnlGroup,
            const NTVERTEX* pnlVerts,
            const RECT& pnl,
            const int pnlId,
            OnChanged onChanged = nullptr
        );

        // IUIControl interface
        void Setup(bco::Vessel& vessel) override;
        void LoadVC(int vcId, bco::Vessel& vessel) override;
        void LoadPanel(int panelId, bco::Vessel& vessel, PANELHANDLE handle);

        bool ToggleState() { SetState(!isOn_); return isOn_; }
        void SetState(bool state);
        bool IsOn() const { return isOn_; }
    private:
        bco::Vessel*    vessel_{ nullptr };

        int             pnlId_;

        int             eventId_{ -1 };
        int             visualCtrlId_{ -1 };

        bool            isOn_{ false };

        OnChanged OnChanged_{ nullptr };

        void MouseEvent();
    };

    inline Button::Button(
        const UINT vcAnimGroupId,
        const VECTOR3& vcLocation,
        const NTVERTEX* vcVerts,
        const UINT pnlGroup,
        const NTVERTEX* pnlVerts,
        const RECT& pnlRect,
        const int pnlId,
        OnChanged onChanged
    ) :
        bco::VCTextureTranslate(vcAnimGroupId, vcVerts),
        bco::VCMouseEvent(vcLocation, 0.01, [this]() { MouseEvent(); }),
        bco::PanelTextureTranslate(pnlGroup, pnlVerts, pnlId),
        bco::PanelMouseEvent(pnlRect, pnlId, [this]() { MouseEvent(); }),
        pnlId_(pnlId),
        OnChanged_(onChanged)
    {}

    inline void Button::Setup(bco::Vessel& vessel)
    {
        vessel_ = &vessel;
        
        visualCtrlId_ = vessel.GetControlId();
        bco::VCTextureTranslate::Setup(vessel, visualCtrlId_);
        bco::PanelTextureTranslate::Setup(vessel, visualCtrlId_);
        
        bco::VCMouseEvent::Setup(vessel);
        bco::PanelMouseEvent::Setup(vessel);
    }

    inline void Button::LoadVC(int vcId, bco::Vessel& vessel)
    {
        if (vcId != 0) return; // Only one VC supported for now.    

        bco::VCTextureTranslate::LoadVC(vcId, vessel);
        bco::VCMouseEvent::LoadVC(vessel);
    }

    inline void Button::LoadPanel(int panelId, bco::Vessel& vessel, PANELHANDLE handle)
    {
        if (panelId != pnlId_) return;

        bco::PanelTextureTranslate::LoadPanel(vessel, panelId, handle);
        bco::PanelMouseEvent::LoadPanel(vessel, panelId, handle);
    }

    inline void Button::MouseEvent()
    {
         ToggleState();
   
         if (OnChanged_ != nullptr) {
               OnChanged_(isOn_);
         }
    }

    inline void Button::SetState(bool state)
    {
        isOn_ = state;
        if (vessel_) {
            bco::VCTextureTranslate::SetState(*vessel_, isOn_ ? 1.0 : 0.0);
            bco::PanelTextureTranslate::SetState(*vessel_, isOn_ ? 1.0 : 0.0);
        }
    }
}