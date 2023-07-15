//	CargoBayController - SR-71r Orbiter Addon
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

#include "OrbiterSDK.h"

#include "bc_orbiter/Animation.h"
#include "bc_orbiter/BaseVessel.h"
#include "bc_orbiter/control.h"
#include "bc_orbiter/on_off_input.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "SR71r_common.h"

namespace bco = bc_orbiter;

/**	CargoBayController
	Controls the cargo bay doors.
	Draws from the main power circuit.  The power draw only
	happens when the doors are in motion.  That draw can be fairly high so
	try to operate one or the other at a time.

	To operate the cargo bay:
	- Power the main circuit (external or fuel cell).
	- Turn 'Main' power on (up).
	- Turn 'Cargo bay' (CRGO) power switch (right panel) on (up).
	- To Open switch 'Cargo Bay' switch (left panel) to OPEN.
	- The message board [BAY] light will show orange while the cargo bay doors
	are in motion, and white when fully open.

	Configuration:
	CARGOBAY a b c
	a - 0/1 Power switch off/on.
	b - 0/1 Open close switch closed/open.
	c - 0.0-1.0 current door position.
*/
class CargoBayController :
      public bco::vessel_component
    , public bco::set_class_caps
    , public bco::post_step
    , public bco::power_consumer
    , public bco::manage_state
{
public:
	CargoBayController(bco::power_provider& pwr);

    // set_class_caps
    void handle_set_class_caps(bco::BaseVessel& vessel) override;

    // power_consumer
    double amp_draw() const override { return IsMoving() ? 4.0 : 0.0; }

    // post_step
    void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override;

    // manage_state
    bool handle_load_state(const std::string& line) override;
    std::string handle_save_state() override;


private:
    const double MIN_VOLTS = 20.0;

    bco::power_provider& power_;

	bool IsPowered() const {
        return
            power_.volts_available() > MIN_VOLTS &&
            switchPower_.is_on();
    }

    bool IsMoving() const {
        return 
            IsPowered() &&
            (animCargoBayDoors_.GetState() > 0.0) && 
            (animCargoBayDoors_.GetState() < 1.0); 
    }

    bco::Animation		    animCargoBayDoors_{ 0.01 };

    bco::AnimationGroup     gpCargoLeftFront_   {   { bm::main::BayDoorPF_id },
                                                    bm::main::Bay1AxisPA_location, bm::main::Bay1AxisPF_location,
                                                    (160 * RAD),
                                                    0.51, 0.74
                                                };

    bco::AnimationGroup     gpCargoRightFront_  {   { bm::main::BayDoorSF_id },
                                                    bm::main::Bay1AxisSF_location, bm::main::Bay1AxisSA_location,
                                                    (160 * RAD),
                                                    0.76, 1.0
                                                };

    bco::AnimationGroup     gpCargoLeftMain_    {   { bm::main::BayDoorPA_id },
                                                    bm::main::Bay2AxisPA_location, bm::main::Bay2AxisPF_location,
                                                    (160 * RAD),
                                                    0.0, 0.24
                                                };

    bco::AnimationGroup     gpCargoRightMain_   {   { bm::main::BayDoorSA_id },
                                                    bm::main::Bay2AxisSF_location, bm::main::Bay2AxisSA_location,
                                                    (160 * RAD),
                                                    0.26, 0.49
                                                };

    bco::on_off_input		switchPower_        { { bm::vc::SwCargoPower_id },
                                                    bm::vc::SwCargoPower_location, bm::vc::PowerTopRightAxis_location,
                                                    toggleOnOff,
                                                    bm::pnl::pnlPwrCargo_id,
                                                    bm::pnl::pnlPwrCargo_verts,
                                                    bm::pnl::pnlPwrCargo_RC
                                                };

    bco::on_off_input		switchOpen_         { { bm::vc::SwCargoOpen_id },
                                                    bm::vc::SwCargoOpen_location, bm::vc::DoorsRightAxis_location,
                                                    toggleOnOff,
                                                    bm::pnl::pnlDoorCargo_id,
                                                    bm::pnl::pnlDoorCargo_verts,
                                                    bm::pnl::pnlDoorCargo_RC
                                                };
};