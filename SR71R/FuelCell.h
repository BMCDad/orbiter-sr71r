//	FuelCell - SR-71r Orbiter Addon
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

#include "Orbitersdk.h"

#include "../bc_orbiter/Animation.h"
#include "../bc_orbiter/Vessel.h"
#include "../bc_orbiter/control.h"
#include "../bc_orbiter/OnOffInput.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "IConsumable.h"
#include "PowerSystem.h"
#include "Common.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"
#include "SR71rPanelRight_mesh.h"

class PowerSystem;

const double OXYGEN_BURN_RATE_PER_SEC_100A = (0.2 / 3600) / 100;		// 2 lbs per hour per at 100 amps.
const double HYDROGEN_BURN_RATE_PER_SEC_100A = (0.1 / 3600) / 100;			// 0.3 lbs per hour @ 100 amps.  

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

/**	Fuel cell.
	Models the plane's fuel cell.

	The fuel cell uses oxygen and hydrogen to produce electricity.  The fuel cell does require a 28v source to run (start up)
	so there must be battery or external connection to start the fuel cell.  When operating it provides 28 volts of power.  
	Resource burn rate will be dependent on the current ships amp usage, which comes from the main power system.

	To Start:
	- Connect external power to the main circuit.
	- Turn on 'Main' power switch (up).
	- Turn the fuel cell switch (right panel) on (up).
	- When the 'AVAIL' light next to the FUEL CELL connect switch lights, turn the FUEL CELL connect switch on (up).
	- The external power connect switch can now be turned OFF.

	The fuel cell will continue to provide power until it is turned off, or runs out of fuel (hydrogen and oxygen
	gauges on the left panel).

	Configuration:
	FUELCELL a
	a = 0/1 fuel cell power switch off/on.
		
*/
class FuelCell : public bco::VesselComponent, public bco::PowerConsumer
{
    const double MAX_VOLTS = 28.0;
    const double MIN_VOLTS = 20.0;
    const double AMP_DRAW =	  4.0;

public:
    FuelCell(bco::PowerProvider& pwr, bco::Vessel& vessel, bco::Consumable& lox, bco::Consumable& hydro);

    /**
        Draw down the oxygen and hydrogen levels based on the current amp load.
    */

    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override;

    // PowerConsumer
    double AmpDraw() const override { return IsPowered() ? AMP_DRAW : 0.0; }

    // ManageState
    bool HandleLoadState(bco::Vessel& vessel, const std::string& line) override;
    std::string HandleSaveState(bco::Vessel& vessel) override;

    // Outputs
    double AvailablePower() const { return isFuelCellAvailable_ ? MAX_VOLTS : 0.0; }

private:
    bco::Vessel&            vessel_;
    bco::PowerProvider&     power_;
    bco::Consumable&        lox_;
    bco::Consumable&        hydro_;

    bool IsPowered() const {
        return
            switchEnabled_.IsOn() &&
            (power_.VoltsAvailable() > MIN_VOLTS);
    }

    void SetIsFuelCellPowerAvailable(bool newValue);

    bool                isFuelCellAvailable_;
    double              ampDrawFactor_{ 0.0 };

    bco::OnOffInput	switchEnabled_{ 
        { bm::vc::swFuelCellPower_id },
        bm::vc::swFuelCellPower_loc, bm::vc::PowerTopRightAxis_loc,
        cmn::toggleOnOff,
        bm::pnlright::pnlPwrFC_id,
        bm::pnlright::pnlPwrFC_vrt,
        bm::pnlright::pnlPwrFC_RC,
        1
    };

    bco::VesselTextureElement       lightAvailable_ {
        bm::vc::FuelCellAvailableLight_id,
        bm::vc::FuelCellAvailableLight_vrt,
        cmn::vc::main,
        bm::pnlright::pnlLgtFCPwrAvail_id,
        bm::pnlright::pnlLgtFCPwrAvail_vrt,
        cmn::panel::right
    };
};

inline FuelCell::FuelCell(bco::PowerProvider& pwr, bco::Vessel& vessel, bco::Consumable& lox, bco::Consumable& hydro) :
    power_(pwr),
    lox_(lox),
    hydro_(hydro),
    isFuelCellAvailable_(false),
    vessel_(vessel)
{
    vessel.AddControl(&switchEnabled_);
    vessel.AddControl(&lightAvailable_);
}

inline void FuelCell::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd)
{
    if (!IsPowered())
    {
        SetIsFuelCellPowerAvailable(false);
    }
    else
    {
        auto ampFac = power_.AmpLoad();

        auto isLOX = lox_.Level() > 0.0;
        auto isHYD = hydro_.Level() > 0.0;

        if (isLOX) {
            lox_.Draw(OXYGEN_BURN_RATE_PER_SEC_100A * ampFac);
        }

        if (isHYD) {
            hydro_.Draw(HYDROGEN_BURN_RATE_PER_SEC_100A * ampFac);
        }

        SetIsFuelCellPowerAvailable(isLOX && isHYD);
    }
}

inline bool FuelCell::HandleLoadState(bco::Vessel& vessel, const std::string& line)
{
    std::istringstream in(line);
    in >> switchEnabled_;
    return true;
}

inline std::string FuelCell::HandleSaveState(bco::Vessel& vessel)
{
    std::ostringstream os;
    os << switchEnabled_;
    return os.str();
}


inline void FuelCell::SetIsFuelCellPowerAvailable(bool newValue)
{
    if (newValue != isFuelCellAvailable_) {
        isFuelCellAvailable_ = newValue;
        lightAvailable_.SetState(vessel_, isFuelCellAvailable_);
    }
}