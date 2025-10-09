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
    class Transform : 
        public bco::IUIControl,
        public bco::ITimeStepVC,
        public bco::ITimeStepPanel
    {
    public:
        Transform(
            const UINT vcAnimGroupId,
            const NTVERTEX* vcVerts,
            const UINT pnlGroup,
            const NTVERTEX* pnlVerts,
            const int pnlId
        );

        void TimeStepVC(bco::Vessel& vessel, double simdt, DEVMESHHANDLE hMesh) override;
        void TimeStepPanel(bco::Vessel& vessel, double simdt, int panelId, MESHHANDLE hMesh) override;

        // IUIControl interface

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

    inline void Transform::TimeStepVC(bco::Vessel& vessel, double simdt, DEVMESHHANDLE hMesh)
    {
        bco::TransformUV<DEVMESHHANDLE>(hMesh, vcGroupId_, vcVerts_, currentAngle_, translation_);
    }

    inline void Transform::TimeStepPanel(bco::Vessel& vessel, double simdt, int panelId, MESHHANDLE hMesh)
    {
        bco::TransformUV<MESHHANDLE>(hMesh, pnlGroup_, pnlVerts_, currentAngle_, translation_);
    }
}

