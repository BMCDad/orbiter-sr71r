//  HydrogenTank - SR-71r Orbiter Addon
//  Copyright(C) 2023  Blake Christensen
//  
//  This program is free software : you can redistribute it and / or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "../bc_orbiter/vessel.h"
#include "../bc_orbiter/control.h"
#include "../bc_orbiter/rotary_display.h"
#include "../bc_orbiter/on_off_input.h"
#include "../bc_orbiter/simple_event.h"
#include "../bc_orbiter/state_display.h"

#include "SR71r_mesh.h"
#include "SR71r_common.h"
#include "Common.h"
#include "ShipMets.h"

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

class HydrogenTank :
    public bco::generic_tank
{
public:
    HydrogenTank(bco::power_provider& pwr, bco::vessel& vessel) :
        bco::generic_tank(pwr, HYDRO_SUPPLY, HYDROGEN_FILL_RATE),
        vessel_(vessel)
    {
        vessel.AddControl(&gaugeLevel_);
        vessel.AddControl(&lightAvailable_);
        vessel.AddControl(&btnFill_);
        vessel.AddControl(&btnLightFill_);

        btnFill_.attach([&]() { ToggleFilling(); });
    }

    void UpdateLevel(double l) override { gaugeLevel_.set_state(l); }
    
    void UpdateIsFilling(bool b) override { 
        if (btnLightFill_.set_state(b)) vessel_.TriggerRedrawArea(cmn::panel::right, cmn::vc::main, btnLightFill_.get_id());
    }

    void UpdateIsAvailable(bool b) override { 
        if (lightAvailable_.set_state(b)) vessel_.TriggerRedrawArea(cmn::panel::right, cmn::vc::main, lightAvailable_.get_id());
    }

    double amp_draw() const override {
        return generic_tank::amp_draw() + (IsPowered() ? 5.0 : 0.0);	// Cryo cooling.
    }

private:

    bco::vessel& vessel_;

    // ***  HYDROGEN SUPPLY  *** //
    bco::rotary_display_target      gaugeLevel_{
        { bm::vc::gaHydrogenLevel_id },
        bm::vc::gaHydrogenLevel_loc, bm::vc::axisHydrogenLevel_loc,
        bm::pnlright::pnlLH2Press_id,
        bm::pnlright::pnlLH2Press_vrt,
        (300 * RAD),
        0.2,
        1
    };

    bco::state_display      lightAvailable_{
        bm::vc::LH2SupplyOnLight_id,
        bm::vc::LH2SupplyOnLight_vrt,
        cmn::vc::main,
        bm::pnlright::pnlLH2Avail_id,
        bm::pnlright::pnlLH2Avail_vrt,
        cmn::panel::right
    };

    bco::simple_event<>             btnFill_{
        bm::vc::LH2ValveOpenSwitch_loc,
        0.01,
        0,
        bm::pnlright::pnlLH2Switch_RC,
        1
    };

    bco::state_display      btnLightFill_{
        bm::vc::LH2ValveOpenSwitch_id,
        bm::vc::LH2ValveOpenSwitch_vrt,
        cmn::vc::main,
        bm::pnlright::pnlLH2Switch_id,
        bm::pnlright::pnlLH2Switch_vrt,
        cmn::panel::right
    };
};
