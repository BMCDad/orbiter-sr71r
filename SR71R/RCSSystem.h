/*
RCSSystem - SR-71r Orbiter Addon
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

#pragma
#include <vector>
#include <map>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\MeshTools.h"
#include "..\bc_orbiter\IUIControl.h"

#include "SR71r_mesh.h"
#include "IPowerProvider.h"
#include "SR71Button.h"

namespace bco = bc_orbiter;

/** RCSMode
*/

class RCSSystem
{
public:
    RCSSystem(bco::Vessel& vessel);
    ~RCSSystem() = default;

    // Callback:
    void OnRCSMode(int mode);

private:

    bco::Vessel* vessel_{ nullptr };

    SR71::Button btnLinear_{
        bm::vc::vcRCSLin_id,
        bm::vc::vcRCSLin_loc,
        bm::vc::vcRCSLin_vrt,
        bm::pnl::pnlRCSLin_id,
        bm::pnl::pnlRCSLin_vrt,
        bm::pnl::pnlRCSLin_RC,
        SR71R::MainPanel_ID,
        [this](bool state) { vessel_->SetAttitudeMode(RCS_LIN); }
    };

    SR71::Button btnRotate_{
        bm::vc::vcRCSRot_id,
        bm::vc::vcRCSRot_loc,
        bm::vc::vcRCSRot_vrt,
        bm::pnl::pnlRCSRot_id,
        bm::pnl::pnlRCSRot_vrt,
        bm::pnl::pnlRCSRot_RC,
        SR71R::MainPanel_ID,
        [this](bool state) { vessel_->SetAttitudeMode(RCS_ROT); }
    };
};

inline RCSSystem::RCSSystem(bco::Vessel& vessel)
{
    vessel_ = &vessel;
    vessel.RegisterUIControl(btnLinear_);
    vessel.RegisterUIControl(btnRotate_);
}

inline void RCSSystem::OnRCSMode(int mode)
{
    switch (mode)
    {
    case RCS_LIN:
        btnLinear_.SetState(true);
        btnRotate_.SetState(false);
        break;
    case RCS_ROT:
        btnLinear_.SetState(false);
        btnRotate_.SetState(true);
        break;
    default:
        btnLinear_.SetState(false);
        btnRotate_.SetState(false);
        break;
    }
}