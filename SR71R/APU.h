//	APU - SR-71r Orbiter Addon
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

#include "../bc_orbiter/control.h"
#include "../bc_orbiter/OnOffInput.h"
#include "../bc_orbiter/RotaryDisplay.h"
#include "../bc_orbiter/Vessel.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"
#include "SR71rPanelRight_mesh.h"

#include "Common.h"

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

// TEMP SPECS:

const double APU_BURN_RATE = 0.05;   // kg per second - 180 kg per hour (180 / 60) / 60.
const double APU_MIN_VOLT = 20.0;
class VESSEL3;

/**	APU
	Auxiliary Power Unit.  The APU provide power to the hydraulic system of the aircraft
	which in turn powers the flight Control surfaces and landing gear.	The APU draws 
    from the same fuel source as the RCS system.
	
    When running, the 'APU' light will be illuminated on the status board.

	Hydraulic Level is currently all or nothing. All = 1.0, Nothing = 0.0;

	The APU requires electrical power to function.
	
	Configuration:
	APU = 0/1

	Short cuts:
	None.
*/
class APU : public bco::VesselComponent, public bco::PowerConsumer, public bco::HydraulicProvider
{
public:
    APU(bco::Vessel& vessel, bco::PowerProvider& pwr, PropulsionController& propulsion);

    double AmpDraw() const override { return IsPowered() ? 5.0 : 0.0; }

    // ManageState
    bool HandleLoadState(bco::Vessel& vessel, const std::string& line) override;
    std::string HandleSaveState(bco::Vessel& vessel) override;

    // haudralic_provider
    double Level() const override { return level_; }

    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override;
//    bco::slot<double>&FuelLevelSlot() { return slotFuelLevel_; }

private:
    bco::Vessel&            vessel_;
    bco::PowerProvider&     power_;
    PropulsionController&   propulsion_;

    bool IsPowered() const {
        return swPower_.IsOn() && (power_.VoltsAvailable() > 24.0);
    }

    double                  level_{ 0.0 };

    bco::OnOffInput       swPower_{
        {bm::vc::SwAPUPower_id },
        bm::vc::SwAPUPower_loc,
        bm::vc::LeftPanelTopRightAxis_loc,
        cmn::toggleOnOff,
        bm::pnlright::pnlAPUSwitch_id,
        bm::pnlright::pnlAPUSwitch_vrt,
        bm::pnlright::pnlAPUSwitch_RC,
        cmn::panel::right
    };

    bco::RotaryDisplayTarget  gaugeAPULevel_{
        { bm::vc::gaHydPress_id },
        bm::vc::gaHydPress_loc, bm::vc::axisHydPress_loc,
        bm::pnlright::pnlHydPress_id,
        bm::pnlright::pnlHydPress_vrt,
        (300 * RAD),	// Clockwise
        0.2,    // speed
        cmn::panel::right
    };

    bco::VesselTextureElement       status_{
        { bm::vc::MsgLightAPU_id},
        bm::vc::MsgLightAPU_vrt,
        cmn::vc::main,
        bm::pnl::pnlMsgLightAPU_id,
        bm::pnl::pnlMsgLightAPU_vrt,
        cmn::panel::main
    };
};

inline APU::APU(bco::Vessel& vessel, bco::PowerProvider& pwr, PropulsionController& propulsion) 
  : power_(pwr),
    vessel_(vessel),
    propulsion_(propulsion)
{
    power_.AttachConsumer(this);

    vessel.AddControl(&gaugeAPULevel_);
    vessel.AddControl(&status_);
    vessel.AddControl(&swPower_);
}

inline bool APU::HandleLoadState(bco::Vessel& vessel, const std::string& line)
{
    std::stringstream in(line);
    in >> swPower_;
    return true;
}

inline std::string APU::HandleSaveState(bco::Vessel& vessel)
{
    std::ostringstream os;
    os << swPower_;
    return os.str();
}

// PostStep
inline void APU::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) 
{
    bool hasFuel = false;

    if (!IsPowered()) {
        level_ = 0.0;
    }
    else {
        hasFuel = propulsion_.MainFuelLevel() > 0.0;

        if (hasFuel) {
            // Note:  We don't actually Draw fuel, but when its gone the APU will shutdown.
            level_ = 1.0;
        }
        else {
            level_ = 0.0;
        }
    }

    gaugeAPULevel_.SetState(level_);

    auto status = IsPowered() ?
        (hasFuel ? cmn::status::on : cmn::status::warn) : cmn::status::off;

    status_.SetState(vessel, status);
}
