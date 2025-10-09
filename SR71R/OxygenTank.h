/*
OxygenTank - SR-71r Orbiter Addon
Copyright(C) 2025  Blake Christensen

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.

Oxygen a b
a: level as ratio, 0.0-empty, 1.0-full
b: fill switch 0/1
*/

#pragma once

#include <cmath>
#include <string>
#include <sstream>

#include "../bc_orbiter/Vessel.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71r2DRight_mesh.h"

#include "ShipMets.h"
#include "ILiquidOxygenProvider.h"
#include "IPowerProvider.h"
#include "SR71Button.h"
#include "SR71Gauge.h"
#include "SR71Light.h"

namespace bco = bc_orbiter;

class OxygenTank : public ILiquidOxygenProvider
{
public:
    OxygenTank(bco::Vessel& vessel);
    ~OxygenTank() = default;

    // IOxygenProvider
    double Draw(double amount) override;

    // UpdateState should be called from the vessel Step() method after all other systems have had a chance to draw power.
    void UpdateState(bco::Vessel& vessel, double simt, double simdt, IPowerProvider& power);

    void LoadState(const std::string& line);
    std::string GetState() const;

private:

    double  gaugeLevelRange_{ 0.0 };
    double  prevTime_{ 0.0 };
    double  currentLevel_{ 0.0 };
    double  prevLevel_{ 0.0 };

    void FillTank(double amount);

    SR71::Gauge gaugeLevel_{
        bm::vc::gaugeOxygenLevel_id,
        bm::vc::gaugeOxygenLevel_loc, bm::vc::axisOxygenLevel_loc,
        bm::pnlright::pnlLOXPress_id,
        bm::pnlright::pnlLOXPress_vrt,
        (300 * RAD),
        gaugeLevelRange_,
        SR71R::RightPanel_ID
    };

    SR71::Light lightAvailable_{
        bm::vc::LOXSupplyOnLight_id,
        bm::vc::LOXSupplyOnLight_vrt,
        bm::pnlright::pnlO2Avail_id,
        bm::pnlright::pnlO2Avail_vrt,
        SR71R::RightPanel_ID
    };

    SR71::Button btnFill_{
        bm::vc::LOXValveOpenSwitch_id,
        bm::vc::LOXValveOpenSwitch_loc,
        bm::vc::LOXValveOpenSwitch_vrt,
        bm::pnlright::pnlO2Switch_id,
        bm::pnlright::pnlO2Switch_vrt,
        bm::pnlright::pnlO2Switch_RC,
        SR71R::RightPanel_ID
    };
};

inline OxygenTank::OxygenTank(bco::Vessel& vessel)
{
    vessel.RegisterUIControl(gaugeLevel_);
    vessel.RegisterUIControl(lightAvailable_);
    vessel.RegisterUIControl(btnFill_);
}

inline void OxygenTank::UpdateState(bco::Vessel& vessel, double simt, double simdt, IPowerProvider& power)
{
    auto tD = simt - prevTime_;

    if (vessel.IsStoppedOrDocked() && power.GetPowerLevel() > 20.0) {
        lightAvailable_.SetState(true);

        if (btnFill_.IsOn()) {
            FillTank(SR71R::OXYGEN_FILL_RATE * simdt);
        }
    }
    else {
        lightAvailable_.SetState(false);
        btnFill_.SetState(false);
    }

    gaugeLevelRange_ = currentLevel_ / SR71R::O2_SUPPLY;
    //    prevTime_ = simt;
}

inline double OxygenTank::Draw(double amount)
{
    auto draw_amount = max(0.0, min(currentLevel_, amount));
    currentLevel_ -= draw_amount;
    return draw_amount;
}

inline void OxygenTank::FillTank(double amount) {
    currentLevel_ += amount;

    if (currentLevel_ >= SR71R::O2_SUPPLY) {
        currentLevel_ = SR71R::O2_SUPPLY;
        btnFill_.SetState(false);
    }
}


inline void OxygenTank::LoadState(const std::string& line)
{
    std::istringstream iss(line);
    int power;
    double amount;
    iss >> amount >> power;

    currentLevel_ = amount * SR71R::O2_SUPPLY;
    btnFill_.SetState(power != 0);
}

inline std::string OxygenTank::GetState() const
{
    std::ostringstream out;
    out << bco::FormatDouble(currentLevel_ / SR71R::O2_SUPPLY) << " " << (btnFill_.IsOn() ? 1 : 0);
    return out.str();
}
