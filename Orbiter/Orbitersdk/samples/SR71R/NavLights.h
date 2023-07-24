//	Lights - SR-71r Orbiter Addon
//	Copyright(C) 2023  Blake Christensen
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

#include "bc_orbiter\signals.h"
#include "bc_orbiter\control.h"

#include "SR71r_mesh.h"
#include "SR71r_common.h"

#include <map>

namespace bco = bc_orbiter;


class NavLight : 
	  public bco::vessel_component
	, public bco::power_consumer
	, public bco::set_class_caps
	, public bco::manage_state
{

public:
	NavLight (bco::power_provider& pwr, bco::vessel& vessel)
		:
		power_(pwr)
	{
		switchNavigationLights_.attach_on_change([&]() { update(); });
		power_.attach_consumer(this);

		vessel.AddControl(&switchNavigationLights_);
	}

	// power_consumer
	void on_change(double v) override { update(); }
	double amp_draw() const override { return switchNavigationLights_.is_on() ? 4.0 : 0.0; }

	// set_class_caps
	void handle_set_class_caps(bco::vessel& vessel) {
		vessel.AddBeacon(&specNavLeft_);
		vessel.AddBeacon(&specNavRight_);
		vessel.AddBeacon(&specNavRear_);
	}

	// manage_state
	bool handle_load_state(bco::vessel& vessel, const std::string& line) override {
		std::istringstream in(line);
		in >> switchNavigationLights_;
		return true;
	}

	std::string handle_save_state(bco::vessel& vessel) override {
		std::ostringstream os;
		os << switchNavigationLights_;
		return os.str();
	}

private:

	bco::power_provider& power_;

	void update() {
		auto isActive =
			switchNavigationLights_.is_on() &&
			(power_.volts_available() > 25.0);

		specNavLeft_.active = isActive;
		specNavRear_.active = isActive;
		specNavRight_.active = isActive;
	}

	// Set light specs:
	VECTOR3 colRed{ 1.0, 0.5, 0.5 };
	VECTOR3 colGreen{ 0.5, 1.0, 0.5 };
	VECTOR3 colWhite{ 1.0, 1.0, 1.0 };

	BEACONLIGHTSPEC			specNavLeft_{
		BEACONSHAPE_DIFFUSE,
		const_cast<VECTOR3*>(&bm::main::NavLightP_location),
		&colRed,		// color
		0.3,			// size
		0.4,			// falloff
		0.0,			// period
		0.1,			// duration
		0.2,			// tofs
		false,			// active
	};

	BEACONLIGHTSPEC			specNavRight_{
		BEACONSHAPE_DIFFUSE,
		const_cast<VECTOR3*>(&bm::main::NavLightS_location),
		&colGreen,		// color
		0.3,			// size
		0.4,			// falloff
		0.0,			// period
		0.1,			// duration
		0.2,			// tofs
		false,			// active
	};

	BEACONLIGHTSPEC			specNavRear_{
		BEACONSHAPE_DIFFUSE,
		const_cast<VECTOR3*>(&bm::main::NavLightTail_location),
		&colWhite,		// color
		0.3,			// size
		0.4,			// falloff
		0.0,			// period
		0.1,			// duration
		0.2,			// tofs
		false,			// active
	};

	bco::on_off_input		switchNavigationLights_ {		// On off switch for external navigation lights.
		{ bm::vc::SwitchNavLights_id },
			bm::vc::SwitchNavLights_location, bm::vc::LightsRightAxis_location,
			toggleOnOff,
			bm::pnl::pnlLightNav_id,
			bm::pnl::pnlLightNav_verts,
			bm::pnl::pnlLightNav_RC
	};
};
