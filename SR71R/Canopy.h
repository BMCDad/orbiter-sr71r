//	Canopy - SR-71r Orbiter Addon
//	Copyright(C) 2017  Blake Christensen
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

#include "OrbiterSDK.h"

#include "../bc_orbiter/Animation.h"
#include "../bc_orbiter/vessel.h"
#include "../bc_orbiter/control.h"
#include "../bc_orbiter/on_off_input.h"
#include "../bc_orbiter/status_display.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "SR71r_common.h"

namespace bco = bc_orbiter;

/**	Canopy
Controls the canopy door.
The canopy runs on the main electrical circuit and can have a high draw
when in motion.

To operate the canopy:
- Power the main circuit (external or fuel cell).
- Turn 'Main' power on (up).
- Turn 'Canopy' (CNPY) power switch (right panel) on (up).
- To Open switch 'Canopy' switch (left panel) to OPEN.
- The message board [CNPY] light will show orange while the canopy is
in motion, and white when fully open.

Configuration:

CANOPY a b c
a - 0/1 Power switch off/on.
b - 0/1 Open close switch closed/open.
c - 0.0-1.0 current canopy position.

: rewrite :
related
on_off_input (canopy power):
: signal canopy slot: has_power
on_off_input (canopy open):
: signal to canopy slot: open/close

- inputs:
:slot - has power
*/
class Canopy : 
      public bco::vessel_component
    , public bco::set_class_caps
    , public bco::post_step
    , public bco::power_consumer
    , public bco::manage_state
{
public:
    Canopy(bco::power_provider& pwr, bco::vessel& vessel);

    // set_class_caps
    void handle_set_class_caps(bco::vessel& vessel) override;

    // power_consumer
    double amp_draw() const override { return CanopyIsMoving() ? 4.0 : 0.0; }

    // post_step
    void handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd) override;

    // manage_state
    bool handle_load_state(bco::vessel& vessel, const std::string& line) override;
    std::string handle_save_state(bco::vessel& vessel) override;

private:
    const double MIN_VOLTS = 20.0;
    
    bco::power_provider& power_;
    
    bool IsPowered() const { 
        return 
            (power_.volts_available() > MIN_VOLTS) &&
            switchPower_.is_on();
    }
    
    bool CanopyIsMoving() const { 
        return 
            IsPowered() && 
            switchOpen_.is_on() &&
            (animCanopy_.GetState() > 0.0) && 
            (animCanopy_.GetState() < 1.0); 
    }

    bco::animation_target		    animCanopy_     { 0.2 };

    bco::animation_group     gpCanopy_       { { bm::main::Canopy_id,
                                                bm::main::CanopyWindowInside_id,
                                                bm::main::CanopyWindowOutside_id},
                                                bm::main::CockpitAxisS_loc, bm::main::CockpitAxisP_loc,
                                                (55 * RAD),
                                                0, 1
                                            };

    bco::animation_group     gpCanopyVC_     { { bm::vc::CanopyFI_id,
                                                bm::vc::CanopyFO_id,
                                                bm::vc::CanopyWindowInside_id,
                                                bm::vc::CanopyWindowSI_id },
                                                bm::main::CockpitAxisS_loc, bm::main::CockpitAxisP_loc,
                                                (55 * RAD),
                                                0, 1
                                            };

    bco::on_off_input       switchPower_{
        { bm::vc::SwCanopyPower_id },
        bm::vc::SwCanopyPower_loc,
        bm::vc::PowerTopRightAxis_loc,
        toggleOnOff,
        bm::pnlright::pnlPwrCanopy_id,
        bm::pnlright::pnlPwrCanopy_vrt,
        bm::pnlright::pnlPwrCanopy_RC,
        1
    };

    bco::on_off_input       switchOpen_{
        { bm::vc::SwCanopyOpen_id },
        bm::vc::SwCanopyOpen_loc,
        bm::vc::DoorsRightAxis_loc,
        toggleOnOff,
        bm::pnlright::pnlDoorCanopy_id,
        bm::pnlright::pnlDoorCanopy_vrt,
        bm::pnlright::pnlDoorCanopy_RC,
        1
    };

    bco::status_display     status_ {           bm::vc::MsgLightCanopy_id,
                                                bm::vc::MsgLightCanopy_vrt,
                                                bm::pnl::pnlMsgLightCanopy_id,
                                                bm::pnl::pnlMsgLightCanopy_vrt,
                                                0.0361
                                            };
};