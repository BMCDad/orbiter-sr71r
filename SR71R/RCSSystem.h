//	RCSSystem - SR-71r Orbiter Addon
//	Copyright(C) 2015  Blake Christensen
//
//	This program is free software : you can redistribute it and / or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "../bc_orbiter/control.h"
#include "../bc_orbiter/VesselEvent.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"

namespace bco = bc_orbiter;

class VESSEL3;

/** RCSMode
*/

class RCSSystem : public bco::VesselComponent, public bco::PowerConsumer
{
public:
    RCSSystem(bco::Vessel& vessel, bco::PowerProvider& pwr);

    // Callback:
    void OnRCSMode(int mode);

    double AmpDraw() const override { return IsPowered() ? 2.0 : 0.0; }
    void OnChange(double v) override { }

private:
    bco::Vessel& vessel_;
    bco::PowerProvider& power_;

    bool IsPowered() const { return power_.VoltsAvailable() > 24; }

    void OnChanged(int mode);
    void ActiveChanged(bool isActive);

    bco::VesselEvent     btnLinear_{
        bm::vc::vcRCSLin_loc,
        0.01,
        cmn::vc::main ,
        bm::pnl::pnlRCSLin_RC,
        cmn::panel::main
    };

    bco::VesselTextureElement       lightLinear_{
        bm::vc::vcRCSLin_id,
        bm::vc::vcRCSLin_vrt,
        0,
        bm::pnl::pnlRCSLin_id,
        bm::pnl::pnlRCSLin_vrt,
        0
    };

    bco::VesselEvent     btnRotate_{
        bm::vc::vcRCSRot_loc,
        0.01,
        cmn::vc::main,
        bm::pnl::pnlRCSRot_RC,
        cmn::panel::main
    };

    bco::VesselTextureElement       lightRotate_{
        bm::vc::vcRCSRot_id,
        bm::vc::vcRCSRot_vrt,
        0,
        bm::pnl::pnlRCSRot_id,
        bm::pnl::pnlRCSRot_vrt,
        0
    };
};

inline RCSSystem::RCSSystem(bco::Vessel& vessel, bco::PowerProvider& pwr) :
    vessel_(vessel),
    power_(pwr)
{
    vessel_.AddControl(&btnLinear_);
    vessel_.AddControl(&btnRotate_);
    vessel_.AddControl(&lightLinear_);
    vessel_.AddControl(&lightRotate_);

    btnLinear_.Attach([&](VESSEL4&) { OnChanged(RCS_LIN); });
    btnRotate_.Attach([&](VESSEL4&) { OnChanged(RCS_ROT); });
}

inline void RCSSystem::ActiveChanged(bool isActive)
{
    if (!isActive) {
        if (vessel_.IsCreated()) {
            vessel_.SetAttitudeMode(RCS_NONE);
        }
    }
}


// Callback:
inline void RCSSystem::OnRCSMode(int mode)
{
    auto linMode = false;
    auto rotMode = false;
    if ((RCS_NONE != mode) && (!IsPowered())) {
        vessel_.SetAttitudeMode(RCS_NONE);
    }
    else {
        linMode = (mode == RCS_LIN);
        rotMode = (mode == RCS_ROT);
    }

    lightLinear_.SetState(vessel_, linMode);
    lightRotate_.SetState(vessel_, rotMode);
}

inline void RCSSystem::OnChanged(int mode)
{
    if (!vessel_.IsCreated()) return;

    auto currentMode = vessel_.GetAttitudeMode();
    auto newMode = (mode == currentMode) ? RCS_NONE : mode;
    vessel_.SetAttitudeMode(newMode);
}
