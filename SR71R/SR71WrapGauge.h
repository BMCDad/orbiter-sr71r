/*
SR71 WrapGauge - SR-71r Orbiter Addon
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


SR71 WrapGauge represents a physical gauge in the SR71 cockpit.  This is
a gauge that sweeps out 360 degrees.  For example, a clock hand or altimeter
needle.

It uses an animation for the VC representation, and a mesh group which is
rotated for th 2D representation.  The component passes a reference to
a double value which represents the current value to display on the gauge.
in the range of 0.0 to 1.0.

Use:
Include this file in the component source.

Define an instance of the class in the component class:

    SR71::Gauge gaugeName_{
         <vc group id>,
         <vc vector defining the gauge pivot location>,
         <vc vector defining the axis of rotation>,
         <2D panel group id>,
         <2D panel NTVERTEX array>,
         <angle of gauge rotation>,
         <reference to component member that contains gauge value>,
         <2D panel ID (left or right)>
    }

In the component constructor call the Vessel.RegisterUIControl and pass the control name.
The Vessel class will take it from there.
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

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

namespace SR71
{
    class WrapGauge : public bco::IUIControl
    {
    public:
        WrapGauge(
            const UINT vcAnimGroupId,
            const VECTOR3& vcLocation,
            const VECTOR3& vcAxisLocation,
            const UINT pnlGroup,
            const NTVERTEX* pnlVerts,
            double& value,   // Reference to the value to display on the gauge.
            const int pnlId,
            const double speed = 0.4   // Speed of the gauge animation.
        );

        // IUIControl interface
        void Setup(bco::Vessel& vessel) override;
        void TimeStep(bco::Vessel& vessel, double simdt) override;
        void LoadVC(int vcId, bco::Vessel& vessel) override;
        void LoadPanel(int panelId, bco::Vessel& vessel, PANELHANDLE handle) override;

    private:

        UINT            animId_{ bco::ANIM_ID_MAX };
        UINT            vcGroupId_{ 0 };
        const VECTOR3&  vcLocation_{ _V(0,0,0) };
        const VECTOR3&  vcAxisLocation_{ _V(0,0,0) };
        UINT            pnlGroup_{ 0 };
        const NTVERTEX* pnlVerts_{ nullptr };
        int             pnlId_{ -1 };
        double&         value_;
        MESHHANDLE      panelMesh_{ nullptr };
        double          speed_{ 0.4 };

        std::unique_ptr<bco::AnimationGroup> vcAnimGroup_{ nullptr };
        bco::AnimatedValue<bco::StateUpdateWrap>  animGauge_{ speed_ };
    };

    inline WrapGauge::WrapGauge(
        const UINT vcAnimGroupId,
        const VECTOR3& vcLocation,
        const VECTOR3& vcAxisLocation,
        const UINT pnlGroup,
        const NTVERTEX* pnlVerts,
        double& value,   // Reference to the value to display on the gauge.
        const int pnlId,
        const double speed /*= 0.4*/
    ) :
        vcGroupId_(vcAnimGroupId),
        vcLocation_(vcLocation),
        vcAxisLocation_(vcAxisLocation),
        pnlGroup_(pnlGroup),
        pnlVerts_(pnlVerts),
        pnlId_(pnlId),
        value_(value),
        speed_(speed)
    {
    }

    inline void WrapGauge::Setup(bco::Vessel& vessel)
    {
        auto vcIndex = vessel.GetMeshIndex(bm::vc::MESH_NAME);
        animId_ = vessel.CreateVesselAnimation();

        // Ensure the constructor arguments for AnimationGroup match its definition  
        vcAnimGroup_ = std::make_unique<bco::AnimationGroup>(
            vcIndex,
            std::initializer_list<UINT>{ vcGroupId_ },  // Ensure initializer_list is used for the group IDs  
            vcLocation_, vcAxisLocation_,
            (360*RAD),
            0.0, 1.0
        );

        vessel.AddAnimationComponent(animId_, 0.0, 1.0, vcAnimGroup_->Transform(), nullptr);
    }

    inline void WrapGauge::TimeStep(bco::Vessel& vessel, double simdt)
    {
        animGauge_.Update(simdt, value_);
        assert(animId_ != bco::ANIM_ID_MAX);
        vessel.SetAnimation(animId_, animGauge_.GetCurrent());

        // Handle panel.
        if (panelMesh_ == nullptr) return; // No panel mesh to update.
        bco::RotateMesh(panelMesh_, pnlGroup_, pnlVerts_, (animGauge_.GetCurrent() * -PI2));
    }

    inline void WrapGauge::LoadVC(int vcId, bco::Vessel& vessel)
    {
        panelMesh_ = nullptr; // No panel mesh for VC
    }

    inline void WrapGauge::LoadPanel(int panelId, bco::Vessel& vessel, PANELHANDLE handle)
    {
        panelMesh_ = nullptr;
        if (panelId != pnlId_) return; // Not the panel we are interested in.
        panelMesh_ = vessel.GetPanelMeshHandle(panelId);
    }
}
