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
	public bco::vessel_component,
	public bco::set_class_caps,
	public bco::power_consumer {

public:
	Strobe()
		:
		enabledSlot_([&](bool v) { SetActive(); })
	{
	}

	// set_class_caps
	void handle_set_class_caps(bco::BaseVessel& vessel) {
		vessel.AddBeacon(&specStrobeLeft_);
		vessel.AddBeacon(&specStrobeRight_);
	}

	// power_consumer
	double amp_load() override {
		return
			enabledSlot_.value() && (voltsInputSlot_.value() > MIN_VOLTS)
			? AMPS
			: 0.0;
	}

	bco::slot<bool>& Slot() { return enabledSlot_; }			// Switch: is beacon on
	bco::slot<double>& VoltsInputSlot() { return voltsInputSlot_; }			// Volts input from power

private:

	const double AMPS = 2.0;
	const double MIN_VOLTS = 24.0;

	bco::slot<bool>			enabledSlot_;
	bco::slot<double>		voltsInputSlot_;

	void SetActive() {
		auto state = ((voltsInputSlot_.value() > MIN_VOLTS) && enabledSlot_.value());
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


