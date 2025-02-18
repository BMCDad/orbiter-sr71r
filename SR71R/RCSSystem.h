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
#include "../bc_orbiter/SimpleEvent.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"

namespace bco = bc_orbiter;

class VESSEL3;

/** RCSMode
*/

class RCSSystem :
    public bco::VesselComponent
    , public bco::PowerConsumer
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

    bco::SimpleEvent<>     btnLinear_{
        bm::vc::vcRCSLin_loc,
        0.01,
        0,
        bm::pnl::pnlRCSLin_RC,
        0
    };

    bco::VesselTextureElement       lightLinear_{
        bm::vc::vcRCSLin_id,
        bm::vc::vcRCSLin_vrt,
        0,
        bm::pnl::pnlRCSLin_id,
        bm::pnl::pnlRCSLin_vrt,
        0
    };

    bco::SimpleEvent<>     btnRotate_{
        bm::vc::vcRCSRot_loc,
        0.01,
        0,
        bm::pnl::pnlRCSRot_RC,
        0
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