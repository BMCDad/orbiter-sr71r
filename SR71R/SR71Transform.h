/*
SR71 Transform - SR-71r Orbiter Addon
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

#include <vector>
#include <limits>
#include <functional>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"
#include "..\bc_orbiter\Tools.h"
#include "..\bc_orbiter\AnimationGroup.h"
#include "..\bc_orbiter\IUIControl.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"

namespace bco = bc_orbiter;

namespace SR71
{
    class Transform : public bco::IUIControl
    {
    public:
        Transform(
            const UINT vcAnimGroupId,
            const NTVERTEX* vcVerts,
            const UINT pnlGroup,
            const NTVERTEX* pnlVerts,
            const int pnlId
        );

        // IUIControl interface
        void UpdateState(Vessel& vessel, double simdt);
        void LoadVC(int vcId, bco::Vessel& vessel) override;
        void LoadPanel(int panelId, bco::Vessel& vessel, PANELHANDLE handle);

        void SetAngle(double angle) { currentAngle_ = angle; }
        void SetTranslate(double x, double y) { translation_.x = x; translation_.y = y; }
    private:
        bco::Vessel* vessel_{ nullptr };

        double          texOffset_{ 0.0 };
        const UINT      vcGroupId_;
        const NTVERTEX* vcVerts_;
        UINT            pnlGroup_;
        const NTVERTEX* pnlVerts_;
        int             pnlId_;
        int             ctrlId_{ -1 };
        bool            isOn_{ false };

        double          currentAngle_{ 0.0 };
        VECTOR3         translation_{ 0.0, 0.0, 0.0 };

        MESHHANDLE meshPanel_{ nullptr };
        DEVMESHHANDLE vcMesh_{ nullptr };

        void OnRedrawPanelEvent();
        void OnRedrawVCAreaEvent();
    };

    inline Transform::Transform(
        const UINT vcAnimGroupId,
        const NTVERTEX* vcVerts,
        const UINT pnlGroup,
        const NTVERTEX* pnlVerts,
        const int pnlId
    ) :
        vcGroupId_(vcAnimGroupId),
        vcVerts_(vcVerts),
        pnlGroup_(pnlGroup),
        pnlVerts_(pnlVerts),
        pnlId_(pnlId)
    {
        texOffset_ = bco::UVOffset(pnlVerts);
    }

    inline void Transform::UpdateState(Vessel& vessel, double simdt)
    {
        if (vcMesh_ != nullptr) {
            bco::TransformUV<DEVMESHHANDLE>(vcMesh_, vcGroupId_, vcVerts_, currentAngle_, translation_);
        }

        if (meshPanel_ != nullptr) {
            bco::TransformUV<MESHHANDLE>(meshPanel_, pnlGroup_, pnlVerts_, currentAngle_, translation_);
        }
    }

    inline void Transform::LoadVC(int vcId, bco::Vessel& vessel)
    {
        vcMesh_ = vessel.GetDeviceMesh(vessel.GetMeshIndex(bm::vc::MESH_NAME));
        meshPanel_ = nullptr;
    }

    inline void Transform::LoadPanel(int panelId, bco::Vessel& vessel, PANELHANDLE handle)
    {
        meshPanel_ = vessel.GetPanelMeshHandle(panelId);
        vcMesh_ = nullptr;
    }
}

