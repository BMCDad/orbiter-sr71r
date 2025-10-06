/*
Altimeter - SR-71r Orbiter Addon
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

Altimeter

The altimeter shows the vessel altitude in the altimeter gauge, and on the TDI.  The gauge
has three hands, 10s, 100s, 1000s.

OFF FLAG - Shows when the avionics is off, or the altitude exceeds 100000 feet.
GND FLAG - Shows when the avionics is in 'Exo' mode.  Altitude then shows the altitude over the mean radius of the gravity body.

*/

#pragma once

#include <cmath>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71r2DMain_mesh.h"

#include "ShipMets.h"
#include "SR71WrapGauge.h"
#include "SR71Light.h"
#include "SR71Barrel.h"

#include "IAvionics.h"

namespace bco = bc_orbiter;

class Altimeter
{

public:

    Altimeter(bco::Vessel& vessel);
    ~Altimeter() = default;

    void UpdateState(bco::Vessel& vessel, double simdt, IAvionics& avionics);

    void LoadState(const std::string& line);
    std::string GetState() const;

private:

    //const double RollOffset = 0.1084;		// flat_roll offset.

    //bco::AnimatedValue<bco::StateUpdateWrap>    animAltOnes_{ 2.0 };
    //bco::AnimatedValue<bco::StateUpdateWrap>    animAltTens_{ 2.0 };
    //bco::AnimatedValue<bco::StateUpdateWrap>    animAltHunds_{ 2.0 };

    //UINT aidVCAltOnes   { 0 };
    //UINT aidVCAltTens   { 0 };
    //UINT aidVCAltHunds  { 0 };

    //bco::AnimatedValue<bco::StateUpdateWrap>    animTDIOnes_{ 1.0 };
    //bco::AnimatedValue<bco::StateUpdateWrap>    animTDITens_{ 1.0 };
    //bco::AnimatedValue<bco::StateUpdateWrap>    animTDIHunds_{ 1.0 };
    //bco::AnimatedValue<bco::StateUpdateWrap>    animTDIThous_{ 1.0 };
    //bco::AnimatedValue<bco::StateUpdateWrap>    animTDITenThous_{ 1.0 };

    SR71::Barrel tdiOnes_ {     bm::vc::vcTDIAltOnes_id,        bm::vc::vcTDIAltOnes_vrt,       bm::pnl::pnlTDIAltOnes_id,      bm::pnl::pnlTDIAltOnes_vrt,     SR71R::MainPanel_ID };
    SR71::Barrel tdiTens_{      bm::vc::vcTDIAltTens_id,        bm::vc::vcTDIAltTens_vrt,       bm::pnl::pnlTDIAltTens_id,      bm::pnl::pnlTDIAltTens_vrt,     SR71R::MainPanel_ID };
    SR71::Barrel tdiHunds_{     bm::vc::vcTDIAltHunds_id,       bm::vc::vcTDIAltHunds_vrt,      bm::pnl::pnlTDIAltHund_id,      bm::pnl::pnlTDIAltHund_vrt,     SR71R::MainPanel_ID };
    SR71::Barrel tdiThous_{     bm::vc::vcTDIAltThous_id,       bm::vc::vcTDIAltThous_vrt,      bm::pnl::pnlTDIAltThous_id,     bm::pnl::pnlTDIAltThous_vrt,    SR71R::MainPanel_ID };
    SR71::Barrel tdiTenThous_{  bm::vc::vcTDIAltTenThous_id,    bm::vc::vcTDIAltTenThous_vrt,   bm::pnl::pnlTDIAltTenThou_id,   bm::pnl::pnlTDIAltTenThou_vrt,  SR71R::MainPanel_ID };

    double alt1Level_{ 0.0 };
    double alt10Level_{ 0.0 };
    double alt100Level_{ 0.0 };

    SR71::WrapGauge gaugeAltOnes_{
        { bm::vc::vcAlt1Hand_id },
        bm::vc::vcAlt1Hand_loc, bm::vc::AltimeterAxis_loc,
        bm::pnl::pnlAlt1Hand_id,
        bm::pnl::pnlAlt1Hand_vrt,
        alt1Level_,
        SR71R::MainPanel_ID,
        2.0
    };

    SR71::WrapGauge gaugeAltTens_{
        { bm::vc::vcAlt10Hand_id },
        bm::vc::vcAlt10Hand_loc, bm::vc::AltimeterAxis_loc,
        bm::pnl::pnlAlt10Hand_id,
        bm::pnl::pnlAlt10Hand_vrt,
        alt10Level_,
        SR71R::MainPanel_ID,
        2.0
    };

    SR71::WrapGauge gaugeAltHunds_{
        { bm::vc::vcAlt100Hand_id },
        bm::vc::vcAlt100Hand_loc, bm::vc::AltimeterAxis_loc,
        bm::pnl::pnlAlt100Hand_id,
        bm::pnl::pnlAlt100Hand_vrt,
        alt100Level_,
        SR71R::MainPanel_ID,
        2.0
    };

    SR71::Light enabledFlag_{
        bm::vc::AltimeterOffFlag_id,
        bm::vc::AltimeterOffFlag_vrt,
        bm::pnl::pnlAltimeterOffFlag_id,
        bm::pnl::pnlAltimeterOffFlag_vrt,
        SR71R::MainPanel_ID
    };

    SR71::Light altimeterExoModeFlag_{
        bm::vc::AltimeterGround_id,
        bm::vc::AltimeterGround_vrt,
        bm::pnl::pnlAltimeterGround_id,
        bm::pnl::pnlAltimeterGround_vrt,
        SR71R::MainPanel_ID
    };
};

inline Altimeter::Altimeter(bco::Vessel& vessel)
{
    vessel.RegisterUIControl(gaugeAltOnes_);
    vessel.RegisterUIControl(gaugeAltTens_);
    vessel.RegisterUIControl(gaugeAltHunds_);
    vessel.RegisterUIControl(enabledFlag_);
    vessel.RegisterUIControl(altimeterExoModeFlag_);

    vessel.RegisterUIControl(tdiOnes_);
    vessel.RegisterUIControl(tdiTens_);
    vessel.RegisterUIControl(tdiHunds_);
    vessel.RegisterUIControl(tdiThous_);
    vessel.RegisterUIControl(tdiTenThous_);
}

inline void Altimeter::UpdateState(bco::Vessel& vessel, double simdt, IAvionics& avionics)
{
    double altFeet = 0.0;
    if (avionics.IsAeroActive()) {
        auto altMode = avionics.IsAeroAtmoMode() ? AltitudeMode::ALTMODE_GROUND : AltitudeMode::ALTMODE_MEANRAD;
        int res = 0;
        auto alt = vessel.GetAltitude(altMode, &res);
        altFeet = alt * 3.28084;
    }

    bco::IntParts iOut;
    if (altFeet < 100000) {
        bco::BreakTens((altFeet > 100000 ? 0.0 : altFeet), iOut);
        alt1Level_ = iOut.Hundreds / 10.0;
        alt10Level_ = iOut.Thousands / 10.0;
        alt100Level_ = iOut.TenThousands / 10.0;
    }
    else {
        alt1Level_ = 0.0;
        alt10Level_ = 0.0;
        alt100Level_ = 0.0;
    }


    // ** ALTIMETER **
    bco::TensParts parts;
    bco::GetDigits((altFeet > 1000000) ? 0 : altFeet, parts);  // Limit to 100000

    tdiOnes_.SetState(parts.Hundreds);
    tdiTens_.SetState(parts.Thousands);
    tdiHunds_.SetState(parts.TenThousands);
    tdiThous_.SetState(parts.HundredThousands);
    tdiTenThous_.SetState(parts.Millions);

    altimeterExoModeFlag_.SetState(     // FALSE show flag, TRUE hide flag
        !avionics.IsAeroActive()        // So if altimeter is OFF set TRUE so the flag does NOT show.
        ? true
        : avionics.IsAeroAtmoMode());   // True for this switch means ATMO mode.

    enabledFlag_.SetState(              // FALSE show flag, TRUE hide flag
        !avionics.IsAeroActive()
        ? false                         // Avionics off, so show flag.
        : ((altFeet > 100000) &&        // If enabled, then SHOW if > 100000 ft
            avionics.IsAeroAtmoMode()   // AND we are in atmo mode.
            ? false
            : true));
}
