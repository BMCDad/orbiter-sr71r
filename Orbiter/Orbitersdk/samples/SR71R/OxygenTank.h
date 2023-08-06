//	OxygenTank - SR-71r Orbiter Addon
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

#include "bc_orbiter/vessel.h"
#include "bc_orbiter/control.h"
#include "bc_orbiter/rotary_display.h"
#include "bc_orbiter/on_off_input.h"
#include "bc_orbiter/on_off_display.h"
#include "bc_orbiter/simple_event.h"

#include "SR71r_mesh.h"
#include "SR71r_common.h"
#include "ShipMets.h"

namespace bco = bc_orbiter;

class OxygenTank :
	public bco::generic_tank
{
public:
	OxygenTank(bco::power_provider& pwr, bco::vessel& vessel) :
		bco::generic_tank(pwr, O2_SUPPLY, OXYGEN_FILL_RATE)
	{
		vessel.AddControl(&gaugeLevel_);
		vessel.AddControl(&lightAvailable_);
		vessel.AddControl(&btnFill_);
		vessel.AddControl(&btnLightFill_);

		btnFill_.attach([&]() { ToggleFilling(); });

		//LevelSignal().attach(		gaugeLevel_.Slot());
		//IsFillingSignal().attach(	btnLightFill_.Slot());
		//IsAvailableSignal().attach(	lightAvailable_.Slot());
	}

	void UpdateLevel(double l) override { gaugeLevel_.set_state(l); }
	void UpdateIsFilling(bool b) override { btnLightFill_.set_state(b); }
	void UpdateIsAvailable(bool b) override { lightAvailable_.set_state(b); }

	double amp_draw() const override {
		return generic_tank::amp_draw() + (IsPowered() ? 5.0 : 0.0);	// Cryo cooling.
	}

private:

	// ***  HYDROGEN SUPPLY  *** //
	bco::rotary_display<bco::animation_target>	gaugeLevel_{ { bm::vc::gaugeOxygenLevel_id },
														bm::vc::gaugeOxygenLevel_location, bm::vc::axisOxygenLevel_location,
														bm::pnl::pnlLOXPress_id,
														bm::pnl::pnlLOXPress_verts,
														(300 * RAD),	// Clockwise
														0.2
	};

	bco::on_off_display				lightAvailable_ {	bm::vc::LOXSupplyOnLight_id,
														bm::vc::LOXSupplyOnLight_verts,
														bm::pnl::pnlO2Avail_id,
														bm::pnl::pnlO2Avail_verts,
														0.0244
													};

	bco::simple_event<>				btnFill_		{	bm::vc::LOXValveOpenSwitch_location,
														0.01,
														bm::pnl::pnlO2Switch_RC
													};

	bco::on_off_display				btnLightFill_	{	bm::vc::LOXValveOpenSwitch_id,
														bm::vc::LOXValveOpenSwitch_verts,
														bm::pnl::pnlO2Switch_id,
														bm::pnl::pnlO2Switch_verts,
														0.0352
													};
};

