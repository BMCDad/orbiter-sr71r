//	AvionBase - SR-71r Orbiter Addon
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

#include "bc_orbiter\control.h"
#include "bc_orbiter\signals.h"
#include "bc_orbiter\BaseVessel.h"

namespace bco = bc_orbiter;

class AvionBase :
	public bco::vessel_component {

public:
	AvionBase() :
		enabledSlot_([&](bool v) { OnEnabledChanged(); }),
		avionicsModeSlot_([&](bool v) {OnAvionModeChanged(); })
	{}

	virtual void OnEnabledChanged() {}
	virtual void OnAvionModeChanged() {}

	// Slots:
	bco::slot<bool>& EnabledSlot() { return enabledSlot_; }
	// True = atmosphere mode, False = exo mode
	bco::slot<bool>& AvionicsModeSlot() { return avionicsModeSlot_; }

private:
	// Slots:
	bco::slot<bool>			enabledSlot_;				// Main avion power switch.
	bco::slot<bool>			avionicsModeSlot_;

};
