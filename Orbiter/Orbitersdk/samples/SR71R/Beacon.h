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

#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\signals.h"
#include "bc_orbiter\control.h"
#include "SR71r_mesh.h"

#include <map>

namespace bco = bc_orbiter;

class Beacon : 
	public bco::vessel_component,
	public bco::set_class_caps {

public:
	Beacon()
		:
		slot_([&](bool v) { SetActive(); })
	{
	}

	// set_class_caps
	void handle_set_class_caps(bco::BaseVessel& vessel) {
		vessel.AddBeacon(&specBeaconTop_);
		vessel.AddBeacon(&specBeaconBottom_);
	}


	bco::slot<bool>& Slot() { return slot_; }
private:

	bco::slot<bool> slot_;

	void SetActive() {
		auto hasPower = true;
		auto state = (hasPower && slot_.value());
		specBeaconTop_.active = state;
		specBeaconBottom_.active = state;
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
};
