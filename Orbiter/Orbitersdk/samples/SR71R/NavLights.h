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

#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\signals.h"
#include "bc_orbiter\control.h"

#include "SR71r_mesh.h"

#include <map>

namespace bco = bc_orbiter;


class NavLight : 
	public bco::vessel_component,
	public bco::set_class_caps,
	public bco::power_consumer {
public:
	NavLight() 
		:
		enabledSlot_([&](bool v) { SetActive(); })
	{
	}

	// set_class_caps
	void handle_set_class_caps(bco::BaseVessel& vessel) {
		vessel.AddBeacon(&specNavLeft_);
		vessel.AddBeacon(&specNavRight_);
		vessel.AddBeacon(&specNavRear_);
	}

	// power_consumer
	double amp_load() override {
		return
			enabledSlot_.value() && (voltsInputSlot_.value() > MIN_VOLTS)
			? AMPS
			: 0.0;
	}

	bco::slot<bool>&	Slot()				{ return enabledSlot_; }			// Switch: is beacon on
	bco::slot<double>&	VoltsInputSlot()	{ return voltsInputSlot_; }			// Volts input from power

private:

	const double AMPS = 2.0;
	const double MIN_VOLTS = 24.0;

	bco::slot<bool>			enabledSlot_;
	bco::slot<double>		voltsInputSlot_;

	void SetActive() {
		auto state = ((voltsInputSlot_.value() > MIN_VOLTS) && enabledSlot_.value());
		specNavLeft_.active = state;
		specNavRear_.active = state;
		specNavRight_.active = state;
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
};
