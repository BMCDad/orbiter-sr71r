//	Strobe - SR-71r Orbiter Addon
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

class Strobe :
	public bco::set_class_caps {

public:
	Strobe()
		:
		slot_([&](bool v) { SetActive(); })
	{
	}

	// set_class_caps
	void handle_set_class_caps(bco::BaseVessel& vessel) {
		vessel.AddBeacon(&specStrobeLeft_);
		vessel.AddBeacon(&specStrobeRight_);
	}


	bco::slot<bool>& Slot() { return slot_; }
private:

	bco::slot<bool> slot_;

	void SetActive() {
		auto hasPower = true;
		auto state = (hasPower && slot_.value());
		specStrobeLeft_.active = state;
		specStrobeRight_.active = state;
	}

	// Set light specs:
	VECTOR3 colRed{ 1.0, 0.5, 0.5 };
	VECTOR3 colGreen{ 0.5, 1.0, 0.5 };
	VECTOR3 colWhite{ 1.0, 1.0, 1.0 };

	BEACONLIGHTSPEC			specStrobeLeft_{
		BEACONSHAPE_DIFFUSE,
		const_cast<VECTOR3*>(&bm::main::StrobeLightP_location),
		&colWhite,		// color
		0.3,			// size
		0.6,			// falloff
		1.13,			// period
		0.05,			// duration
		0.5,			// tofs
		false,			// active
	};

	BEACONLIGHTSPEC			specStrobeRight_{
		BEACONSHAPE_DIFFUSE,
		const_cast<VECTOR3*>(&bm::main::StrobeLightS_location),
		&colWhite,		// color
		0.3,			// size
		0.6,			// falloff
		1.13,			// period
		0.05,			// duration
		0.5,			// tofs
		false,			// active
	};
};


