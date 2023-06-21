//	VSI - SR-71r Orbiter Addon
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

class VSI :
	public AvionBase,
	public bco::post_step {

public:

	VSI() {}
	~VSI() {}

	// post_step
	void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override {
		// VSI displays speeds between -6000 and 6000 fpm.  On the
		// VSI gauge animation this is 0 (-6000) to 1 (6000).
		// Furthermore we scale the speed around 0 so that we get
		// a greater resolution around 0.  This means 0 fpm is
		// 0.5 on the animation.
		auto vertSpeed = 0.0;

		if (EnabledSlot().value()) {
			vertSpeed = bco::GetVerticalSpeedFPM(&vessel);
		}

		double isPos = (vertSpeed >= 0) ? 1 : -1;
		auto absSpd = abs(vertSpeed);
		if (absSpd > 6000) absSpd = 6000;


		// This scales our speed so we end up with 0 to 1.
		// 6000 -> 1.0      0 ->    0.0
		// 5000 -> 0.69     1000 -> 0.17
		// 4000 -> 0.44     2000 -> 0.11
		// 3000 -> 0.25     3000 -> 0.25
		// 2000 -> 0.11     4000 -> 0.44
		// 1000 -> 0.03     5000 -> 0.69
		double spRot = (1 - pow((6000 - absSpd) / 6000, 2)) / 2;

		//gaVSINeedle_.SetState(0.5 + (isPos * spRot));
		vsiNeedleSignal_.fire(0.5 + (isPos * spRot));
	}

	bco::signal<double>& VSINeedleSignal() { return vsiNeedleSignal_; }

private:

	bco::signal<double >	vsiNeedleSignal_;
};

