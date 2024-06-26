//    Lights - SR-71r Orbiter Addon
//    Copyright(C) 2023  Blake Christensen
//
//    This program is free software : you can redistribute it and / or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "..\bc_orbiter\control.h"
#include "..\bc_orbiter\handler_interfaces.h"
#include "..\bc_orbiter\vessel.h"

#include "SR71r_mesh.h"
#include "SR71r_common.h"

#include <map>

namespace bco = bc_orbiter;

class Lights :
    public bco::vessel_component,
    public bco::power_consumer,
    public bco::set_class_caps ,
    public bco::manage_state
{

public:
    Lights(bco::vessel& vessel, bco::power_provider& pwr) : power_(pwr) {
        switchStrobeLights_.attach_on_change([&]() { update(); });
        switchBeaconLights_.attach_on_change([&]() { update(); });
        switchNavigationLights_.attach_on_change([&]() { update(); });

        power_.attach_consumer(this);
        
        vessel.AddControl(&switchStrobeLights_);
        vessel.AddControl(&switchBeaconLights_);
        vessel.AddControl(&switchNavigationLights_);
    }

    // power_consumer
    void on_change(double v) override { update(); }
    double amp_draw() const override { 
        auto total = 0.0;
        total += switchStrobeLights_.is_on() ? 4.0 : 0.0;
        total += switchBeaconLights_.is_on() ? 4.0 : 0.0;
        total += switchNavigationLights_.is_on() ? 4.0 : 0.0;
        return total;
    }

    // set_class_caps
    void handle_set_class_caps(bco::vessel& vessel) {
        vessel.AddBeacon(&specStrobeLeft_);
        vessel.AddBeacon(&specStrobeRight_);
        
        vessel.AddBeacon(&specBeaconTop_);
        vessel.AddBeacon(&specBeaconBottom_);
        
        vessel.AddBeacon(&specNavLeft_);
        vessel.AddBeacon(&specNavRear_);
        vessel.AddBeacon(&specNavRight_);
    }

    // manage_state
    bool handle_load_state(bco::vessel& vessel, const std::string& line) override {
        // sscanf_s(configLine + 6, "%i%i%i%i", &nav, &beacon, &strobe, &dock);
        double dock; // not used.
        std::istringstream in(line);
        in >> switchNavigationLights_ >> switchBeaconLights_ >> switchStrobeLights_ >> dock;
        return true;
    }

    std::string handle_save_state(bco::vessel& vessel) override {
        std::ostringstream os;
        os << switchNavigationLights_ << " " << switchBeaconLights_ << " " << switchStrobeLights_ << 0;
        return os.str();
    }

private:

    bco::power_provider& power_;

    void update() {
        auto power = (power_.volts_available() > 25.0);

        auto strob = switchStrobeLights_.is_on() && power;
        auto beacon = switchBeaconLights_.is_on() && power;
        auto nav = switchNavigationLights_.is_on() && power;

        specStrobeLeft_.active = strob;
        specStrobeRight_.active = strob;

        specBeaconTop_.active = beacon;
        specBeaconBottom_.active = beacon;

        specNavLeft_.active = nav;
        specNavRear_.active = nav;
        specNavRight_.active = nav;
    }

    // Set light specs:
    VECTOR3 colRed{ 1.0, 0.5, 0.5 };
    VECTOR3 colGreen{ 0.5, 1.0, 0.5 };
    VECTOR3 colWhite{ 1.0, 1.0, 1.0 };

    BEACONLIGHTSPEC            specStrobeLeft_{
        BEACONSHAPE_DIFFUSE,
        const_cast<VECTOR3*>(&bm::main::StrobeLightP_loc),
        &colWhite,        // color
        0.3,            // size
        0.6,            // falloff
        1.13,            // period
        0.05,            // duration
        0.5,            // tofs
        false,            // active
    };

    BEACONLIGHTSPEC            specStrobeRight_{
        BEACONSHAPE_DIFFUSE,
        const_cast<VECTOR3*>(&bm::main::StrobeLightS_loc),
        &colWhite,  // color
        0.3,        // size
        0.6,        // falloff
        1.13,       // period
        0.05,       // duration
        0.5,        // tofs
        false,      // active
    };

    BEACONLIGHTSPEC            specBeaconTop_{
        BEACONSHAPE_STAR,
        const_cast<VECTOR3*>(&bm::main::BeaconTop_loc),
        &colRed,    // color
        0.55,       // size
        0.6,        // falloff
        2.0,        // period
        0.1,        // duration
        0.0,        // tofs
        false,      // active
    };

    BEACONLIGHTSPEC            specBeaconBottom_{
        BEACONSHAPE_STAR,
        const_cast<VECTOR3*>(&bm::main::BeaconBottom_loc),
        &colRed,    // color
        0.55,       // size
        0.6,        // falloff
        2.0,        // period
        0.1,        // duration
        0.0,        // tofs
        false,      // active
    };

    BEACONLIGHTSPEC            specNavLeft_{
        BEACONSHAPE_DIFFUSE,
        const_cast<VECTOR3*>(&bm::main::NavLightP_loc),
        &colRed,    // color
        0.3,        // size
        0.4,        // falloff
        0.0,        // period
        0.1,        // duration
        0.2,        // tofs
        false,      // active
    };

    BEACONLIGHTSPEC            specNavRight_{
        BEACONSHAPE_DIFFUSE,
        const_cast<VECTOR3*>(&bm::main::NavLightS_loc),
        &colGreen,   // color
        0.3,         // size
        0.4,         // falloff
        0.0,         // period
        0.1,         // duration
        0.2,         // tofs
        false,       // active
    };

    BEACONLIGHTSPEC            specNavRear_{
        BEACONSHAPE_DIFFUSE,
        const_cast<VECTOR3*>(&bm::main::NavLightTail_loc),
        &colWhite,        // color
        0.3,            // size
        0.4,            // falloff
        0.0,            // period
        0.1,            // duration
        0.2,            // tofs
        false,            // active
    };

    bco::on_off_input       switchStrobeLights_{        // On off switch for external strobe lights.
        { bm::vc::SwitchStrobeLights_id },
        bm::vc::SwitchStrobeLights_loc, bm::vc::LightsRightAxis_loc,
        toggleOnOff,
        bm::pnlright::pnlLightStrobe_id,
        bm::pnlright::pnlLightStrobe_vrt,
        bm::pnlright::pnlLightStrobe_RC,
        1
    };

    bco::on_off_input       switchBeaconLights_{        // On off switch for external beacon lights.
        { bm::vc::SwitchBeaconLights_id },
        bm::vc::SwitchBeaconLights_loc, 
        bm::vc::LightsRightAxis_loc,
        toggleOnOff,
        bm::pnlright::pnlLightBeacon_id,
        bm::pnlright::pnlLightBeacon_vrt,
        bm::pnlright::pnlLightBeacon_RC,
        1
    };

    bco::on_off_input       switchNavigationLights_ {   // On off switch for external navigation lights.
        { bm::vc::SwitchNavLights_id },
        bm::vc::SwitchNavLights_loc, bm::vc::LightsRightAxis_loc,
        toggleOnOff,
        bm::pnlright::pnlLightNav_id,
        bm::pnlright::pnlLightNav_vrt,
        bm::pnlright::pnlLightNav_RC,
        1
    };
};


