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

#include "../bc_orbiter/Animation.h"
#include "../bc_orbiter/control.h"
#include "../bc_orbiter/OnOffInput.h"
#include "../bc_orbiter/Vessel.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"
#include "SR71rPanelRight_mesh.h"

#include "ShipMets.h"
#include "common.h"

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

/**	CargoBayController
	Controls the cargo bay doors.
	Draws from the main power circuit.  The power Draw only
	happens when the doors are in motion.  That Draw can be fairly high so
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
class CargoBayController : public bco::VesselComponent, public bco::PowerConsumer
{
public:
	CargoBayController(bco::PowerProvider& pwr, bco::Vessel& vessel);

    // SetClassCaps
    void HandleSetClassCaps(bco::Vessel& vessel) override;

    // PowerConsumer
    double AmpDraw() const override { return IsMoving() ? 4.0 : 0.0; }

    // PostStep
    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override;

    // ManageState
    bool HandleLoadState(bco::Vessel& vessel, const std::string& line) override;
    std::string HandleSaveState(bco::Vessel& vessel) override;


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

    bco::AnimationTarget    animCargoBayDoors_{ 0.01 };

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

    bco::OnOffInput       switchPower_{
        { bm::vc::SwCargoPower_id },
        bm::vc::SwCargoPower_loc, bm::vc::PowerTopRightAxis_loc,
        cmn::toggleOnOff,
        bm::pnlright::pnlPwrCargo_id,
        bm::pnlright::pnlPwrCargo_vrt,
        bm::pnlright::pnlPwrCargo_RC,
        1
    };

    bco::OnOffInput       switchOpen_{ 
        { bm::vc::SwCargoOpen_id },
        bm::vc::SwCargoOpen_loc, bm::vc::DoorsRightAxis_loc,
        cmn::toggleOnOff,
        bm::pnlright::pnlDoorCargo_id,
        bm::pnlright::pnlDoorCargo_vrt,
        bm::pnlright::pnlDoorCargo_RC,
        1
    };

    bco::VesselTextureElement       status_ {
        bm::vc::MsgLightBay_id,
        bm::vc::MsgLightBay_vrt,
        cmn::vc::main,
        bm::pnl::pnlMsgLightBay_id,
        bm::pnl::pnlMsgLightBay_vrt,
        cmn::panel::main
    };
};

inline CargoBayController::CargoBayController(bco::PowerProvider& pwr, bco::Vessel& vessel) :
    power_(pwr)
{
    power_.AttachConsumer(this);
    vessel.AddControl(&switchOpen_);
    vessel.AddControl(&switchPower_);
    vessel.AddControl(&status_);
}

inline void CargoBayController::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd)
{
    if (IsPowered()) {
        animCargoBayDoors_.Update(vessel, switchOpen_.IsOn() ? 1.0 : 0.0, simdt);
    }

    auto status = cmn::status::off;
    if (power_.VoltsAvailable() > MIN_VOLTS) {
        if ((animCargoBayDoors_.GetState() > 0.0) && (animCargoBayDoors_.GetState() < 1.0)) {
            status = cmn::status::warn;
        }
        else {
            if (animCargoBayDoors_.GetState() == 1.0) {
                status = cmn::status::on;
            }
        }
    }
    status_.SetState(vessel, status);
}

inline bool CargoBayController::HandleLoadState(bco::Vessel& vessel, const std::string& line)
{
    std::istringstream in(line);
    in >> switchPower_ >> switchOpen_ >> animCargoBayDoors_;
    vessel.SetAnimationState(animCargoBayDoors_);
    return true;
}

inline std::string CargoBayController::HandleSaveState(bco::Vessel& vessel)
{
    std::ostringstream os;
    os << switchPower_ << " " << switchOpen_ << " " << animCargoBayDoors_;
    return os.str();
}

inline void CargoBayController::HandleSetClassCaps(bco::Vessel& vessel)
{
    auto mIdx = vessel.GetMainMeshIndex();

    auto id = vessel.CreateVesselAnimation(animCargoBayDoors_);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoLeftFront_);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoRightFront_);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoLeftMain_);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoRightMain_);
}