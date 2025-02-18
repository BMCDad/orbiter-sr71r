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

#include "StdAfx.h"

#include "Orbitersdk.h"
#include "FuelCell.h"
#include "SR71r_mesh.h"


FuelCell::FuelCell(bco::PowerProvider& pwr, bco::Vessel& vessel, bco::Consumable& lox, bco::Consumable& hydro) :
    power_(pwr),
    lox_(lox),
    hydro_(hydro),
    isFuelCellAvailable_(false),
    vessel_(vessel)
{
    vessel.AddControl(&switchEnabled_);
    vessel.AddControl(&lightAvailable_);
}

void FuelCell::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd)
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

    sigAvailPower_.fire(isFuelCellAvailable_ ? MAX_VOLTS : 0.0);
}

bool FuelCell::HandleLoadState(bco::Vessel& vessel, const std::string& line)
{
	std::istringstream in(line);
	in >> switchEnabled_;
	return true;
}

std::string FuelCell::HandleSaveState(bco::Vessel& vessel)
{
    std::ostringstream os;
    os << switchEnabled_;
    return os.str();
}


void FuelCell::SetIsFuelCellPowerAvailable(bool newValue)
{
    if (newValue != isFuelCellAvailable_)
    {
        isFuelCellAvailable_ = newValue;
        lightAvailable_.set_state(vessel_, isFuelCellAvailable_);
    }
}