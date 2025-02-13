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
#include "../bc_orbiter/simple_event.h"
#include "../bc_orbiter/on_off_display.h"
#include "../bc_orbiter/state_display.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"

namespace bco = bc_orbiter;

class VESSEL3;

/** RCSMode
*/

class RCSSystem :
    public bco::vessel_component
    , public bco::power_consumer
{
public:
    RCSSystem(bco::vessel& vessel, bco::power_provider& pwr);

    // Callback:
    void OnRCSMode(int mode);

    double amp_draw() const override { return IsPowered() ? 2.0 : 0.0; }
    void on_change(double v) override { }

private:
    bco::vessel& vessel_;
    bco::power_provider& power_;

    bool IsPowered() const { return power_.volts_available() > 24; }

    void OnChanged(int mode);
    void ActiveChanged(bool isActive);

    bco::simple_event<>     btnLinear_{
        bm::vc::vcRCSLin_loc,
        0.01,
        0,
        bm::pnl::pnlRCSLin_RC,
        0
    };

    bco::state_display     lightLinear_{
        bm::vc::vcRCSLin_id,
        bm::vc::vcRCSLin_vrt,
        0,
        bm::pnl::pnlRCSLin_id,
        bm::pnl::pnlRCSLin_vrt,
        0
    };

    bco::simple_event<>     btnRotate_{
        bm::vc::vcRCSRot_loc,
        0.01,
        0,
        bm::pnl::pnlRCSRot_RC,
        0
    };

    bco::state_display      lightRotate_{
        bm::vc::vcRCSRot_id,
        bm::vc::vcRCSRot_vrt,
        0,
        bm::pnl::pnlRCSRot_id,
        bm::pnl::pnlRCSRot_vrt,
        0
    };
};