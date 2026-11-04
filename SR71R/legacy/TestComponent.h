//	TEST - SR-71r Orbiter Addon
//	Copyright(C) 2015  Blake Christensen
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

#include "SR71r_mesh.h"
#include "bc_orbiter\control.h"
#include "bc_orbiter\signals.h"

namespace bco = bc_orbiter;

class VESSEL3;

/** RCSMode
*/

class TestComponent : public bco::vessel_component
{
public:
	TestComponent() :
		slotIncrement_([&](bool v) {Increment(); slotIncrement_.set(); }),
		slotDecrement_([&](bool v) {Decrement(); slotDecrement_.set(); })
	{}

	bco::slot<bool>& IncrementSlot() { return slotIncrement_; }
	bco::slot<bool>& DecrementSlot() { return slotDecrement_; }

	bco::signal<int>& ValueSignal() { return sigValue_; }

private:

	void Increment() {
		value_++;
		if (value_ > 9) value_ = 0;
		sigValue_.fire(value_);
	}

	void Decrement() {
		value_--;
		if (value_ < 0) value_ = 9;
		sigValue_.fire(value_);
	}

	int					value_{ 0 };
	bco::slot<bool>		slotIncrement_;
	bco::slot<bool>		slotDecrement_;

	bco::signal<int>	sigValue_;
};
