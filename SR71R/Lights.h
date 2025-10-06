/*
Lights - SR-71r Orbiter Addon
Copyright(C) 2023  Blake Christensen

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
#include <string>
#include <sstream>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"
#include "..\bc_orbiter\Tools.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71r2DRight_mesh.h"

#include "ShipMets.h"
#include "IPowerProvider.h"
#include "SR71Toggle.h"

namespace bco = bc_orbiter;

class Lights
{
public:
    Lights(bco::Vessel& vessel);
    ~Lights() = default;

    void Setup(bco::Vessel& vessel);
    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power);

    void ToggleStrobe() { togStrobe_.ToggleState(); }
    void ToggleBeacon() { togBeacon_.ToggleState(); }
    void ToggleNavigation() { togNav_.ToggleState(); }

    void LoadState(const std::string& line);
    std::string GetState() const;

private:

    SR71::Toggle togStrobe_{
        bm::vc::SwitchStrobeLights_id,
        bm::vc::SwitchStrobeLights_loc, bm::vc::LightsRightAxis_loc,
        bm::pnlright::pnlLightStrobe_id,
        bm::pnlright::pnlLightStrobe_vrt,
        bm::pnlright::pnlLightStrobe_RC,
        SR71R::RightPanel_ID
    };

    SR71::Toggle togBeacon_{
        { bm::vc::SwitchBeaconLights_id },
        bm::vc::SwitchBeaconLights_loc, bm::vc::LightsRightAxis_loc,
        bm::pnlright::pnlLightBeacon_id,
        bm::pnlright::pnlLightBeacon_vrt,
        bm::pnlright::pnlLightBeacon_RC,
        SR71R::RightPanel_ID
    };

    SR71::Toggle togNav_{
        { bm::vc::SwitchNavLights_id },
        bm::vc::SwitchNavLights_loc, bm::vc::LightsRightAxis_loc,
        bm::pnlright::pnlLightNav_id,
        bm::pnlright::pnlLightNav_vrt,
        bm::pnlright::pnlLightNav_RC,
        SR71R::RightPanel_ID
    };

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
};

inline Lights::Lights(bco::Vessel& vessel)
{
    vessel.RegisterUIControl(togStrobe_);
    vessel.RegisterUIControl(togBeacon_);
    vessel.RegisterUIControl(togNav_);
}

inline void Lights::UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power)
{
    auto hasPower = (power.GetPowerLevel() > 25.0);

    bool strob = togStrobe_.IsOn() && hasPower;
    bool beacon = togBeacon_.IsOn() && hasPower;
    bool nav = togNav_.IsOn() && hasPower;

    specStrobeLeft_.active = strob;
    specStrobeRight_.active = strob;

    specBeaconTop_.active = beacon;
    specBeaconBottom_.active = beacon;

    specNavLeft_.active = nav;
    specNavRear_.active = nav;
    specNavRight_.active = nav;

}

inline void Lights::Setup(bco::Vessel& vessel)
{
    vessel.AddBeacon(&specStrobeLeft_);
    vessel.AddBeacon(&specStrobeRight_);

    vessel.AddBeacon(&specBeaconTop_);
    vessel.AddBeacon(&specBeaconBottom_);

    vessel.AddBeacon(&specNavLeft_);
    vessel.AddBeacon(&specNavRear_);
    vessel.AddBeacon(&specNavRight_);
}

inline void Lights::LoadState(const std::string& line)
{
    int nav, beacon, strobe, dock;
    std::istringstream in(line);
    in >> nav >> beacon >> strobe >> dock;

    togNav_.SetState(nav != 0);
    togBeacon_.SetState(beacon != 0);
    togStrobe_.SetState(strobe != 0);
}

inline std::string Lights::GetState() const
{
    std::ostringstream out;
    out << (togNav_.IsOn() ? 1 : 0) << " " << (togBeacon_.IsOn() ? 1 : 0) << " " << (togStrobe_.IsOn() ? 1 : 0) << " 0";
    return out.str();
}
