/*
Airspeed - SR-71r Orbiter Addon
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

Airspeed

KEAS - Knots Equivalent Air Speed
        KEAS is shown on the TDI and is more accurate at higher speeds and altitudes.

KIAS - Knots Indicated Air Speed
        KIAS is shown on the dial and is more accurate at lower speeds and altitudes.

MACH - Mach number, ratio of the speed of the aircraft to the speed of sound.
        MACH is shown on the TDI and the dial.

MAX MACH - Maximum safe MACH for the given altitude.  Shown as a red error bar on the dial.
        The MAX MACH is calculated based on altitude and temperature.

OFF FLAG - Shows when there is no power to the avionics system, or avionics is turned off.

GND FLAG - When avionics is in EXO mode, the mach gauge shows raw speed (velocity).

TODO:  Review the airspeed calculations and see if they can be improved.
*/

#pragma once

#include <cmath>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "ShipMets.h"
#include "SR71Gauge.h"
#include "SR71Light.h"
#include "SR71Barrel.h"

#include "IAvionics.h"

namespace bco = bc_orbiter;

class Airspeed
{
public:
    Airspeed(bco::Vessel& vessel);
    ~Airspeed() = default;
    
    void UpdateState(bco::Vessel& vessel, double simdt, IAvionics& avionics);
    
    void LoadState(const std::string& line);
    std::string GetState() const;

private:

    const double MIN_PIN = 0.0;
    const double MAX_PIN_RAD = 5.236; // 300.0 deg
    double l22 = log(23);
    const double RollOffset = 0.1084;		// flat_roll offset.

    const double ANGLE = -(300.0 * RAD); // 300 degrees in radians.
    bco::AnimatedValue<bco::StateUpdateTarget>  animMaxMachHand_    { 2.0 };

    SR71::Barrel tdiKeasOne_{   bm::vc::vcTDIKeasOnes_id,   bm::vc::vcTDIKeasOnes_vrt,  bm::pnl::pnlTDIKEASOnes_id,     bm::pnl::pnlTDIKEASOnes_vrt,    SR71R::MainPanel_ID };
    SR71::Barrel tdiKeasTen_{   bm::vc::vcTDIKeasTens_id,   bm::vc::vcTDIKeasTens_vrt,  bm::pnl::pnlTDIKEASTens_id,     bm::pnl::pnlTDIKEASTens_vrt,    SR71R::MainPanel_ID };
    SR71::Barrel tdiKeasHund_{  bm::vc::vcTDIKeasHunds_id,  bm::vc::vcTDIKeasHunds_vrt, bm::pnl::pnlTDIKEASHunds_id,    bm::pnl::pnlTDIKEASHunds_vrt,   SR71R::MainPanel_ID };

    SR71::Barrel tdiMachOne_{  bm::vc::vcTDIMachOne_id,    bm::vc::vcTDIMachOne_vrt,   bm::pnl::pnlTDIMACHOne_id,      bm::pnl::pnlTDIMACHOne_vrt,     SR71R::MainPanel_ID };
    SR71::Barrel tdiMachTen_{  bm::vc::vcTDIMachTens_id,   bm::vc::vcTDIMachTens_vrt,  bm::pnl::pnlTDIMACHTens_id,     bm::pnl::pnlTDIMACHTens_vrt,    SR71R::MainPanel_ID };
    SR71::Barrel tdiMachHund_{ bm::vc::vcTDIMachHunds_id,  bm::vc::vcTDIMachHunds_vrt, bm::pnl::pnlTDIMACHHunds_id,    bm::pnl::pnlTDIMACHHunds_vrt,   SR71R::MainPanel_ID };

    UINT aidVCMaxMachHand_  { 0 };

    double machLevel_ = 0.0;
    double kiesLevel_ = 0.0;
    double machMaxlevel_ = 0.0;

    SR71::Gauge gaugeMACHHand_{
        bm::vc::vcMachHand_id,
        bm::vc::vcMachHand_loc, bm::vc::SpeedAxis_loc,
        bm::pnl::pnlMachHand_id,
        bm::pnl::pnlMachHand_vrt,
        (300 * RAD),
        machLevel_,
        SR71R::MainPanel_ID
    };

    SR71::Gauge gaugeKEISHand_{
        bm::vc::vcKiesHand_id,
        bm::vc::vcKiesHand_loc, bm::vc::SpeedAxis_loc,
        bm::pnl::pnlKiesHand_id,
        bm::pnl::pnlKiesHand_vrt,
        (300 * RAD),
        kiesLevel_,
        SR71R::MainPanel_ID
    };

    SR71::Gauge gaugeMaxMAcHHand_{
        bm::vc::vcMachMaxHand_id,
        bm::vc::vcMachMaxHand_loc, bm::vc::SpeedAxis_loc,
        bm::pnl::pnlMachMaxHand_id,
        bm::pnl::pnlMachMaxHand_vrt,
        (300 * RAD),
        machMaxlevel_,
        SR71R::MainPanel_ID
    };

    SR71::Light flagEnabled_{
        bm::vc::SpeedFlagOff_id,
        bm::vc::SpeedFlagOff_vrt,
        bm::pnl::pnlSpeedFlagOff_id,
        bm::pnl::pnlSpeedFlagOff_vrt,
        SR71R::MainPanel_ID
    };

    SR71::Light flagVelocity_ {
        bm::vc::SpeedVelocityFlag_id,
        bm::vc::SpeedVelocityFlag_vrt,
        bm::pnl::pnlSpeedVelocityFlag_id,
        bm::pnl::pnlSpeedVelocityFlag_vrt,
        SR71R::MainPanel_ID
    };

    SR71::Light status_{
        bm::vc::MsgLightKeasWarn_id,
        bm::vc::MsgLightKeasWarn_vrt,
        bm::pnl::pnlMsgLightKeasWarn_id,
        bm::pnl::pnlMsgLightKeasWarn_vrt,
        SR71R::MainPanel_ID
    };

};

inline Airspeed::Airspeed(bco::Vessel& vessel)
{
    vessel.RegisterUIControl(gaugeMACHHand_);
    vessel.RegisterUIControl(gaugeKEISHand_);
    vessel.RegisterUIControl(gaugeMaxMAcHHand_);
    vessel.RegisterUIControl(flagEnabled_);
    vessel.RegisterUIControl(flagVelocity_);

    vessel.RegisterUIControl(tdiKeasOne_);
    vessel.RegisterUIControl(tdiKeasTen_);
    vessel.RegisterUIControl(tdiKeasHund_);
    vessel.RegisterUIControl(tdiMachOne_);
    vessel.RegisterUIControl(tdiMachTen_);
    vessel.RegisterUIControl(tdiMachHund_);

    vessel.RegisterUIControl(status_);
}

inline void Airspeed::UpdateState(bco::Vessel& vessel, double simdt, IAvionics& avionics)
{
    double  keas            = 0.0;  // equivalent airspeed, shows in TDI
    double  kias            = 0.0;  // indicated, shows as dial.
    double  mach            = 0.0;  // shows in TDI and as a dial
    double  maxMach         = 0.0;  // Error bar on dial
    double  kiasSpeed       = 0.0;
    bool	isOverSpeed        = false;

    machLevel_ = 0.0; // Reset mach level for gauge.
    kiesLevel_ = 0.0; // Reset kies level for gauge.
    machMaxlevel_ = 0.0; // Reset max mach level for gauge.

    if (avionics.IsAeroActive()) {
        flagEnabled_.SetState(true);
        flagVelocity_.SetState(avionics.IsAeroAtmoMode());

        keas = vessel.GetKeas();
        kias = vessel.GetKias();
        mach = vessel.GetMachNumber();
        auto atmDens = vessel.GetAtmDensity();

        if (atmDens > 0.0)
        {
            maxMach = sqrt(SR71R::MaxPress / atmDens) / 331.34;
            isOverSpeed = mach > maxMach;
        }

        auto machGauge = (mach > SR71R::MAX_MACH) ? SR71R::MAX_MACH : mach;		// machGauge will be pinned, mach itself will be used to set the dials, so it not pinned.

        if (!avionics.IsAeroAtmoMode())  // if exo mode, use velocity for machGauge
        {
            machGauge = vessel.GetAirspeed() / 100;
            maxMach = 22.0;
        }

        // Set WARNING MAX MACH gauge : MACH scale is log base 22 (max mach):
        // LOG range 1 to 23 (22 positions) to avoid <1 values.

        // Calculate the max mach needle.
        if (maxMach > 22.0) maxMach = 22.0;		// Pin MAX to 22 and 1
        if (maxMach < 0.0) maxMach = 0.0;
        machMaxlevel_ = (maxMach == 0.0)
            ? 0.0
            : ((log(maxMach + 1) / l22) * MAX_PIN_RAD) / MAX_PIN_RAD;	// Determine LOG based on speed then convert to 0-1 ration for the gauge.

        // Kies dial
        machLevel_ = ((log(machGauge + 1.0) / l22) * MAX_PIN_RAD) / MAX_PIN_RAD; // FIX << RATIO does not need MAX_PIN_RAD
        auto kRatio = kias / 600; // 600 max kias speed
        kiasSpeed = (avionics.IsAeroAtmoMode()) ? (machLevel_ - kRatio) : machLevel_;	// If atmo, subtract the speedRatio to get correct rotation.

        kiesLevel_ = machLevel_ - (kias / 600); // Kies needle is mach - (kias/600)
    }
    else {
        flagEnabled_.SetState(false);
        flagVelocity_.SetState(true);
    }

    bco::TensParts parts;
    bco::GetDigits(keas, parts);
    tdiKeasOne_.SetState(parts.Tens);
    tdiKeasTen_.SetState(parts.Hundreds);
    tdiKeasHund_.SetState(parts.Thousands);

    bco::GetDigits(mach, parts);
    tdiMachOne_.SetState(parts.Tenths);
    tdiMachTen_.SetState(parts.Tens);
    tdiMachHund_.SetState(parts.Hundreds);

    status_.SetState(isOverSpeed ? SR71::StatusError : SR71::StatusOff);
}