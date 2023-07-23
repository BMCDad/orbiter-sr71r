//	Shutters - SR-71r Orbiter Addon
//	Copyright(C) 2016  Blake Christensen
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

#include "bc_orbiter/Component.h"
#include "bc_orbiter/Animation.h"
#include "bc_orbiter/vessel.h"
#include "bc_orbiter/signals.h"
#include "bc_orbiter/on_off_input.h"

#include "SR71r_mesh.h"
#include "SR71r_common.h"

namespace bco = bc_orbiter;

class Shutters : 
	public bco::vessel_component,
	public bco::manage_state
{
public:
	Shutters(bco::vessel& vessel);

	// manage_state
	bool handle_load_state(bco::vessel& vessel, const std::string& line) override;
	std::string handle_save_state(bco::vessel& vessel) override;

	bco::slot<bool>& ShuttersSlot() { return shuttersSlot_; }

private:
	void Update();

	bco::slot<bool>		shuttersSlot_;

	const char*				ConfigKey = "SHUTTERS";

	// ***  Shutters  *** //
	bco::on_off_input		switchShutters_{		// Open close shutters
		{ bm::vc::swShutter_id },
			bm::vc::swShutter_location, bm::vc::DoorsRightAxis_location,
			toggleOnOff,
			bm::pnl::pnlScreenSwitch_id,
			bm::pnl::pnlScreenSwitch_verts,
			bm::pnl::pnlScreenSwitch_RC
	};
};