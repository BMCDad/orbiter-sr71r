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
#include "bc_orbiter/vessel.h"
#include "bc_orbiter/control.h"
#include "bc_orbiter/on_off_input.h"
#include "bc_orbiter/status_display.h"

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
      public bco::VesselComponent
    , public bco::HandlesSetClassCaps
    , public bco::HandlesPostStep
    , public bco::PowerConsumer
    , public bco::HandlesState
{
public:
	CargoBayController(bco::PowerProvider& pwr, bco::vessel& vessel);

    // set_class_caps
    void HandleSetClassCaps(bco::vessel& vessel) override;

    // power_consumer
    double AmpDraw() const override { return IsMoving() ? 4.0 : 0.0; }

    // post_step
    void HandlePostStep(bco::vessel& vessel, double simt, double simdt, double mjd) override;

    // manage_state
    bool HandleLoadState(bco::vessel& vessel, const std::string& line) override;
    std::string HandleSaveState(bco::vessel& vessel) override;


private:
    const double MIN_VOLTS = 20.0;

    bco::PowerProvider& power_;

	bool IsPowered() const {
        return
            (power_.VoltsAvailable() > MIN_VOLTS) &&
            switchPower_.IsOn();
    }

    bool IsMoving() const {
        return 
            IsPowered() &&
            (animCargoBayDoors_.GetState() > 0.0) && 
            (animCargoBayDoors_.GetState() < 1.0); 
    }

    bco::animation_target		    animCargoBayDoors_{ 0.01 };

    bco::AnimationGroup     gpCargoLeftFront_   {   { bm::main::BayDoorPF_id },
                                                    bm::main::Bay1AxisPA_loc, bm::main::Bay1AxisPF_loc,
                                                    (160 * RAD),
                                                    0.51, 0.74
                                                };

    bco::AnimationGroup     gpCargoRightFront_  {   { bm::main::BayDoorSF_id },
                                                    bm::main::Bay1AxisSF_loc, bm::main::Bay1AxisSA_loc,
                                                    (160 * RAD),
                                                    0.76, 1.0
                                                };

    bco::AnimationGroup     gpCargoLeftMain_    {   { bm::main::BayDoorPA_id },
                                                    bm::main::Bay2AxisPA_loc, bm::main::Bay2AxisPF_loc,
                                                    (160 * RAD),
                                                    0.0, 0.24
                                                };

    bco::AnimationGroup     gpCargoRightMain_   {   { bm::main::BayDoorSA_id },
                                                    bm::main::Bay2AxisSF_loc, bm::main::Bay2AxisSA_loc,
                                                    (160 * RAD),
                                                    0.26, 0.49
                                                };

    bco::on_off_input		switchPower_        { { bm::vc::SwCargoPower_id },
                                                    bm::vc::SwCargoPower_loc, bm::vc::PowerTopRightAxis_loc,
                                                    toggleOnOff,
                                                    bm::pnl::pnlPwrCargo_id,
                                                    bm::pnl::pnlPwrCargo_vrt,
                                                    bm::pnl::pnlPwrCargo_RC
                                                };

    bco::on_off_input		switchOpen_         { { bm::vc::SwCargoOpen_id },
                                                    bm::vc::SwCargoOpen_loc, bm::vc::DoorsRightAxis_loc,
                                                    toggleOnOff,
                                                    bm::pnl::pnlDoorCargo_id,
                                                    bm::pnl::pnlDoorCargo_vrt,
                                                    bm::pnl::pnlDoorCargo_RC
                                                };

    bco::status_display     status_     {           bm::vc::MsgLightBay_id,
                                                    bm::vc::MsgLightBay_vrt,
                                                    bm::pnl::pnlMsgLightBay_id,
                                                    bm::pnl::pnlMsgLightBay_vrt,
                                                    0.0361
                                                };
};