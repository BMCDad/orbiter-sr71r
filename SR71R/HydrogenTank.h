/*
HydrogenTank - SR-71r Orbiter Addon
Copyright(C) 2023  Blake Christensen

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

HYDROGEN a b
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
#include "IHydrogenProvider.h"
#include "IPowerProvider.h"
#include "SR71Button.h"
#include "SR71Gauge.h"
#include "SR71Light.h"

namespace bco = bc_orbiter;

class HydrogenTank : public IHydrogenProvider
{
public:
    HydrogenTank(bco::Vessel& vessel);
    ~HydrogenTank() = default;

    // IHydrogenProvider
    double Draw(double amount) override;

    // UpdateState should be called from the vessel Step() method after all other systems have had a chance to draw power.
    void UpdateState(bco::Vessel& vessel, double simt, double simdt, IPowerProvider& power);

    void LoadState(const std::string& line);
    std::string GetState() const;

private:

    double  gaugeLevelRange_    { 0.0 };
    double  prevTime_           { 0.0 };
    double  currentLevel_       { 0.0 };
    double  prevLevel_          { 0.0 };

    void FillTank(double amount);

    SR71::Gauge gaugeLevel_{
        bm::vc::gaHydrogenLevel_id,
        bm::vc::gaHydrogenLevel_loc, bm::vc::axisHydrogenLevel_loc,
        bm::pnlright::pnlLH2Press_id,
        bm::pnlright::pnlLH2Press_vrt,
        (300 * RAD),
        gaugeLevelRange_,
        SR71R::RightPanel_ID
    };

    SR71::Light lightAvailable_{
        bm::vc::LH2SupplyOnLight_id,
        bm::vc::LH2SupplyOnLight_vrt,
        bm::pnlright::pnlLH2Avail_id,
        bm::pnlright::pnlLH2Avail_vrt,
        SR71R::RightPanel_ID
    };

    SR71::Button btnFill_{
        bm::vc::LH2ValveOpenSwitch_id,
        bm::vc::LH2ValveOpenSwitch_loc,
        bm::vc::LH2ValveOpenSwitch_vrt,
        bm::pnlright::pnlLH2Switch_id,
        bm::pnlright::pnlLH2Switch_vrt,
        bm::pnlright::pnlLH2Switch_RC,
        SR71R::RightPanel_ID
    };
};

inline HydrogenTank::HydrogenTank(bco::Vessel& vessel)
{
    vessel.RegisterUIControl(gaugeLevel_);
    vessel.RegisterUIControl(lightAvailable_);
    vessel.RegisterUIControl(btnFill_);
}

inline void HydrogenTank::UpdateState(bco::Vessel& vessel, double simt, double simdt, IPowerProvider& power)
{
    auto tD = simt - prevTime_;

    if (vessel.IsStoppedOrDocked() && power.GetPowerLevel() > 20.0) {
        lightAvailable_.SetState(true);

        if (btnFill_.IsOn()) {
            FillTank(SR71R::HYDROGEN_FILL_RATE * simdt);
        }
    }
    else {
        lightAvailable_.SetState(false);
        btnFill_.SetState(false);
    }

    gaugeLevelRange_ = currentLevel_ / SR71R::HYDRO_SUPPLY;
//    prevTime_ = simt;
}

inline double HydrogenTank::Draw(double amount)
{
    auto draw_amount = max(0.0, min(currentLevel_, amount));
    currentLevel_ -= draw_amount;
    return draw_amount;
}

inline void HydrogenTank::FillTank(double amount) {
    currentLevel_ += amount;

    if (currentLevel_ >= SR71R::HYDRO_SUPPLY) {
        currentLevel_ = SR71R::HYDRO_SUPPLY;
        btnFill_.SetState(false);
    }
}


inline void HydrogenTank::LoadState(const std::string& line)
{
    std::istringstream iss(line);
    int power;
    double amount;
    iss >> amount >> power;

    currentLevel_ = amount * SR71R::HYDRO_SUPPLY;
    btnFill_.SetState(power != 0);
}

inline std::string HydrogenTank::GetState() const
{
    std::ostringstream out;
    out << bco::FormatDouble(currentLevel_ / SR71R::HYDRO_SUPPLY) << " " << (btnFill_.IsOn() ? 1 : 0);
    return out.str();
}
