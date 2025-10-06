/*
SR71 Barrel - SR-71r Orbiter Addon
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

SR71 Barrel models a numerical barrel control in the SR71 cockpit.  The barrel
rotates up and down to display the target number.  The illusion is achieved by 
translating the UV coordinates of the barrel mesh.  The texture that draws the
numbers must arranged so that the numbers are evenly spaced vertically.

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
#include "SR71rVC_mesh.h"

#include "ShipMets.h"

namespace bco = bc_orbiter;

namespace SR71
{
    class Barrel : 
        public bco::IUIControl, 
        public bco::ITimeStepVC,
        public bco::ITimeStepPanel
    {
    public:
        /**
        * \brief Constructor for a Barrel control.
        * \param vcAnimGroupId The mesh group ID for the VC representation of the barrel.
        * \param vcVerts The vertices for the VC representation of the barrel.
        * \param pnlGroup The mesh group ID for the 2D panel representation of the barrel.
        * \param pnlVerts The vertices for the 2D panel representation of the barrel.
        * \param pnlId The panel ID for the 2D panel representation of the barrel.
        */
        Barrel(
            const UINT vcAnimGroupId,
            const NTVERTEX* vcVerts,
            const UINT pnlGroup,
            const NTVERTEX* pnlVerts,
            const int pnlId
        );

        void TimeStep(bco::Vessel& vessel, double simdt) override;

        void SetState(double state) { targetState_ = floor(state) / 10; }

        // IUpdateVC interface
        void TimeStepVC(bco::Vessel& vessel, double simdt, DEVMESHHANDLE hMesh) override;

        // IUpdatePanel interface
        void TimeStepPanel(bco::Vessel& vessel, double simdt, int panelId, MESHHANDLE hMesh) override;

    private:
        const double RollOffset = 0.1084;		// flat_roll offset.

        bco::Vessel* vessel_{ nullptr };

        const UINT      vcGroupId_;
        const NTVERTEX* vcVerts_;
        UINT            pnlGroup_;
        const NTVERTEX* pnlVerts_;
        int             pnlId_;
        double          targetState_{ 0 };
        VECTOR3         vecTrans_{ 0.0, 0.0, 0.0 };

        bco::AnimatedValue<bco::StateUpdateWrap>    anim_ { 1.5 };
    };

    inline Barrel::Barrel(
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
    { }

    inline void Barrel::TimeStep(bco::Vessel& vessel, double simdt)
    {
        anim_.Update(simdt, targetState_);
    }

    inline void Barrel::TimeStepVC(bco::Vessel& vessel, double simdt, DEVMESHHANDLE hMesh)
    {
        bco::TranslateUVQuad(hMesh, vcGroupId_, vcVerts_, 0.0, RollOffset * anim_.GetCurrent());
    }

    inline void Barrel::TimeStepPanel(bco::Vessel& vessel, double simdt, int panelId, MESHHANDLE hMesh)
    {
        if (panelId != pnlId_) return;
        bco::TranslateUVQuad(hMesh, pnlGroup_, pnlVerts_, 0.0, RollOffset * anim_.GetCurrent());
    }
}

