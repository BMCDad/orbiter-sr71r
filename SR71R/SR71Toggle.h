/*
SR71 Toggle Switch - SR-71r Orbiter Addon
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

Define an instance of the class in the component class:

    SR71::Toggle toggleSwitchName_ {
        { <vc group id> },
        <vc vector location of switch>,
        <vc vector location defining rotation axis>,
        <2D panel group id>,
        <2D panel NTVERTEX array>,
        <2D panel RECT location>,
        <2D panel ID (left or right)>
    };

In the component constructor call the Vessel.RegisterUIControl and pass the control name.
The Vessel class will take it from there.

The component can then manage the state of the switch through the
'ToggleState', 'SetState', and 'IsOn' methods.


*/
#pragma once

#include <vector>
#include <limits>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"
#include "..\bc_orbiter\Tools.h"
#include "..\bc_orbiter\AnimationGroup.h"
#include "..\bc_orbiter\IUIControl.h"
#include "..\bc_orbiter\PanelTextureTranslate.h"
#include "..\bc_orbiter\PanelMouseEvent.h"
#include "..\bc_orbiter\VCMouseEvent.h"


#include "SR71r_mesh.h"
#include "ShipMets.h"

namespace bco = bc_orbiter;

namespace SR71
{
    /**
    * Class to model a physical on/off toggle switch in the SR71 cockpit.
    */
    class Toggle : 
        public bco::IUIControl,
        public bco::PanelTextureTranslate,
        public bco::PanelMouseEvent,
        public bco::VCMouseEvent
    {
    public:
        /**
        * \brief Constructs a Toggle switch control.
        * \param vcAnimGroupId The animation group ID for the VC toggle switch.
        * \param vcLocation The location of the toggle switch in the VC.
        * \param vcAxisLocation The axis location.  Defines the axis of rotation for the toggle switch.
        * \param pnlGroup The panel group ID for the 2D panel toggle switch.
        * \param pnlVerts The NTVERTEX array for the 2D panel toggle switch.
        * \param pnlRect The RECT defining the location of the toggle switch on the 2D panel.
        * \param pnlId The panel ID (main or right) for the 2D panel toggle switch.
        */
        Toggle(
            const UINT vcAnimGroupId,
            const VECTOR3& vcLocation,
            const VECTOR3& vcAxisLocation,
            const UINT pnlGroup,
            const NTVERTEX* pnlVerts,
            const RECT& pnl,
            const int pnlId
        );

        // IUIControl interface
        void Setup(bco::Vessel& vessel) override;
        void TimeStep(bco::Vessel& vessel, double simdt);
        void LoadVC(int vcId, bco::Vessel& vessel) override;
        void LoadPanel(int panelId, bco::Vessel& vessel, PANELHANDLE handle);

        bool ToggleState();
        void SetState(bool state);
        bool IsOn() const { return isOn_; }
    private:

        bco::AnimatedValue<bco::StateUpdateTarget>  animVCSwitch_{ SR71R::ToggleAnimSpeed };

        bco::Vessel*        vessel_{ nullptr };
        const UINT          vcGroupId_;
        const VECTOR3&      vcLocation_;
        const VECTOR3&      vcAxisLocation_;

        int                 visualCtrlId_{ -1 };

        int                 eventId_{ -1 };
        UINT                animId_{ bco::ANIM_ID_MAX };
        bool                isOn_{ false };

        std::unique_ptr<bco::AnimationGroup> vcAnimGroup_{ nullptr };
        
        std::function<void(bco::Vessel& vessel)> UIUpdateHandler_{ [&](bco::Vessel& vessel) {} };
        MESHHANDLE meshPanel_{ nullptr };

        void OnRedrawPanelEvent();
        bool OnMouseEvent(int eventId, int eventType);
        void MouseEvent();
    };

    inline Toggle::Toggle(
        const UINT vcAnimGroupId,
        const VECTOR3& vcLocation,
        const VECTOR3& vcAxisLocation,
        const UINT pnlGroup,
        const NTVERTEX* pnlVerts,
        const RECT& pnlRect,
        const int pnlId
    ) :
        bco::PanelTextureTranslate(pnlGroup, pnlVerts, pnlId),
        bco::PanelMouseEvent(pnlRect, pnlId, [this]() { MouseEvent(); }),
        bco::VCMouseEvent(vcLocation, SR71R::ToggleHitRadius, [this]() { MouseEvent(); }),
        vcGroupId_(vcAnimGroupId),
        vcLocation_(vcLocation),
        vcAxisLocation_(vcAxisLocation)
    { }

    inline void Toggle::Setup(bco::Vessel& vessel)  
    {  
        vessel_ = &vessel;

        auto vcIndex = vessel.GetMeshIndex(bm::vc::MESH_NAME);  
        animId_ = vessel.CreateVesselAnimation();  

        // Ensure the constructor arguments for AnimationGroup match its definition  
        vcAnimGroup_ = std::make_unique<bco::AnimationGroup>(  
            vcIndex,  
            std::initializer_list<UINT>{ vcGroupId_ },  // Ensure initializer_list is used for the group IDs  
            vcLocation_, vcAxisLocation_,  
            SR71R::ToggleAnimAngle,  
            0.0, 1.0  
        );  

        vessel.AddAnimationComponent(animId_, 0.0, 1.0, vcAnimGroup_->Transform(), nullptr);

        visualCtrlId_ = vessel.GetControlId();

        bco::PanelTextureTranslate::Setup(vessel, visualCtrlId_);
        bco::PanelMouseEvent::Setup(vessel);
        bco::VCMouseEvent::Setup(vessel);
    }

    inline void Toggle::TimeStep(bco::Vessel& vessel, double simdt)
    {
        animVCSwitch_.Update(simdt, isOn_ ? 1.0 : 0.0);
        assert(animId_ != bco::ANIM_ID_MAX);
        vessel.SetAnimation(animId_, animVCSwitch_.GetCurrent());
    }

    inline void Toggle::LoadVC(int vcId, bco::Vessel& vessel)
    {
        if (vcId != 0) return; // Only one VC supported for now.    
        bco::VCMouseEvent::LoadVC(vessel);
    }

    inline void Toggle::LoadPanel(int panelId, bco::Vessel& vessel, PANELHANDLE handle)
    {
        bco::PanelMouseEvent::LoadPanel(vessel, panelId, handle);
        bco::PanelTextureTranslate::LoadPanel(vessel, panelId, handle);
    }

    inline void Toggle::MouseEvent()
    {
        ToggleState();
        SetState(isOn_);
    }
    
    inline bool Toggle::ToggleState()
    {
        SetState(!isOn_);
        return isOn_;
    }

    inline void Toggle::SetState(bool state)
    {
         isOn_ = state;
         bco::PanelTextureTranslate::SetState(*vessel_, isOn_ ? 1.0 : 0.0);
    }
}


