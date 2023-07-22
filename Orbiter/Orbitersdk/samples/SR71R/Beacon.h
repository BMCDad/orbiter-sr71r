//	Beacon - SR-71r Orbiter Addon
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
#include "bc_orbiter\electrical_component.h"

#include "SR71r_mesh.h"
#include "SR71r_common.h"

#include <map>

namespace bco = bc_orbiter;

class Beacon : 
	  public bco::vessel_component
	, public bco::power_consumer
	, public bco::set_class_caps 
	, public bco::manage_state
{

public:
	Beacon(bco::power_provider& pwr) 
		:
		power_(pwr)
	{ 
		switchBeaconLights_.attach_on_change([&]() { update(); });
		power_.attach_consumer(this);
	}

	// power_consumer
	void on_change(double v) override { update(); }
	double amp_draw() const override { return switchBeaconLights_.is_on() ? 4.0 : 0.0; }

	// set_class_caps
	void handle_set_class_caps(bco::BaseVessel& vessel) override {
		vessel.AddBeacon(&specBeaconTop_);
		vessel.AddBeacon(&specBeaconBottom_);
	
		vessel.AddControl(&switchBeaconLights_);
	}

	// manage_state
	bool handle_load_state(bco::BaseVessel& vessel, const std::string& line) override {

		std::istringstream in(line);
		in >> switchBeaconLights_;
		return true;
	}

	std::string handle_save_state(bco::BaseVessel& vessel) override {
		std::ostringstream os;
		os << switchBeaconLights_;
		return os.str();
	}

private:

	bco::power_provider& power_;

	void update() {
		auto isActive = 
			switchBeaconLights_.is_on() &&
			(power_.volts_available() > 25.0);

		specBeaconTop_.active = isActive;
		specBeaconBottom_.active = isActive;
	}


	// Set light specs:
	VECTOR3 colRed{ 1.0, 0.5, 0.5 };
	VECTOR3 colGreen{ 0.5, 1.0, 0.5 };
	VECTOR3 colWhite{ 1.0, 1.0, 1.0 };

	BEACONLIGHTSPEC			specBeaconTop_{
		BEACONSHAPE_STAR,
		const_cast<VECTOR3*>(&bm::main::BeaconTop_location),
		&colRed,		// color
		0.55,			// size
		0.6,			// falloff
		2.0,			// period
		0.1,			// duration
		0.0,			// tofs
		false,			// active
	};
	
	BEACONLIGHTSPEC			specBeaconBottom_{
		BEACONSHAPE_STAR,
		const_cast<VECTOR3*>(&bm::main::BeaconBottom_location),
		&colRed,		// color
		0.55,			// size
		0.6,			// falloff
		2.0,			// period
		0.1,			// duration
		0.0,			// tofs
		false,			// active
	};

	bco::on_off_input		switchBeaconLights_{		// On off switch for external beacon lights.
		  { bm::vc::SwitchBeaconLights_id }
		, bm::vc::SwitchBeaconLights_location, bm::vc::LightsRightAxis_location
		, toggleOnOff
		, bm::pnl::pnlLightBeacon_id
		, bm::pnl::pnlLightBeacon_verts
		, bm::pnl::pnlLightBeacon_RC
	};
};

