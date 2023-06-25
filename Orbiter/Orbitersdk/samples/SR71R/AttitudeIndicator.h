//	AttitudeIndicator - SR-71r Orbiter Addon
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

#include "AvionBase.h"

namespace bco = bc_orbiter;

class AttitudeIndicator :
	public AvionBase,
	public bco::post_step {

public:

	AttitudeIndicator() {}
	~AttitudeIndicator() {}

	// post_step
	void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override {
		double bank = 0.0;
		double pitch = 0.0;

		if (EnabledSlot().value()) {
			pitch = vessel.GetPitch();
			bank = vessel.GetBank();
		}

		// 90deg = 1.57079 rad.
		// So, pixel distance between 0 and 90 is 500 - 178 = 322
		// Texture size of 2048, the ratio for 322 = .15722.
		// Divided by 1.57079 = .100093

		auto pTran = 0.100093 * pitch;

		signalBank_.fire(bank);
		signalPitch_.fire(pTran);
	}


	bco::signal<double>& BankSignal()	{ return signalBank_; }
	bco::signal<double>& PitchSignal()	{ return signalPitch_; }

private:

	bco::signal<double>		signalBank_;
	bco::signal<double>		signalPitch_;

	double prevPitch_{ 0.0 };
	double prevBank_{ 0.0 };
};


