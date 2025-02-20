//	Altimeter - SR-71r Orbiter Addon
//	Copyright(C) 2023  Blake Christensen
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
#include "../bc_orbiter/TextureRoll.h"
#include "../bc_orbiter/RotaryDisplay.h"
#include "../bc_orbiter/Vessel.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "Avionics.h"
#include "Common.h"

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

class Altimeter : public bco::VesselComponent
{

public:

    Altimeter(bco::Vessel& vessel, Avionics& avionics) :
        avionics_(avionics),
        vessel_(vessel) {
        vessel.AddControl(&onesHand_);
        vessel.AddControl(&tensHand_);
        vessel.AddControl(&hundsHand_);

        vessel.AddControl(&tdiAltOnes_);
        vessel.AddControl(&tdiAltTens_);
        vessel.AddControl(&tdiAltHunds_);
        vessel.AddControl(&tdiAltThou_);
        vessel.AddControl(&tdiAltTenThou_);

        vessel.AddControl(&enabledFlag_);
        vessel.AddControl(&altimeterExoModeFlag_);
    }

    ~Altimeter() {}


    // PostStep
    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override {
        double altFeet = 0.0;
        if (avionics_.IsAeroActive()) {
            auto altMode = avionics_.IsAeroAtmoMode() ? AltitudeMode::ALTMODE_GROUND : AltitudeMode::ALTMODE_MEANRAD;
            int res = 0;
            auto alt = vessel.GetAltitude(altMode, &res);
            altFeet = alt * 3.28084;
        }

        bco::IntParts iOut;
        if (altFeet < 100000) {
            bco::BreakTens((altFeet > 100000 ? 0.0 : altFeet), iOut);

            onesHand_.SetState(iOut.Hundreds / 10.0);
            tensHand_.SetState(iOut.Thousands / 10.0);
            hundsHand_.SetState(iOut.TenThousands / 10.0);
        }
        else {
            onesHand_.SetState(0);
            tensHand_.SetState(0);
            hundsHand_.SetState(0);
        }


        // ** ALTIMETER **
        bco::TensParts parts;
        bco::GetDigits((altFeet > 1000000) ? 0 : altFeet, parts);  // Limit to 100000

        //sprintf(oapiDebugString(), "Alt: : %+2i : %+2i : %+2i", altFeet, parts.Tens, parts.Hundreds);

        tdiAltOnes_.SetPosition(parts.Hundreds);
        tdiAltTens_.SetPosition(parts.Thousands);
        tdiAltHunds_.SetPosition(parts.TenThousands);
        tdiAltThou_.SetPosition(parts.HundredThousands);
        tdiAltTenThou_.SetPosition(parts.Millions);

        altimeterExoModeFlag_.SetState(    // FALSE show flag, TRUE hide flag
            vessel_,
            !avionics_.IsAeroActive()       // So if altimeter is OFF set TRUE so the flag does NOT show.
            ? true
            : avionics_.IsAeroAtmoMode());  // True for this switch means ATMO mode.

        enabledFlag_.SetState(             // FALSE show flag, TRUE hide flag
            vessel_,
            !avionics_.IsAeroActive()
            ? false                         // Avionics off, so show flag.
            : ((altFeet > 100000) &&        // If enabled, then SHOW if > 100000 ft
                avionics_.IsAeroAtmoMode()  // AND we are in atmo mode.
                ? false
                : true));
    }

private:
    bco::Vessel& vessel_;
    Avionics& avionics_;

    using dial = bco::RotaryDisplayWrap;
    using roll = bco::TextureRoll;
    const double ofs = 0.1084;		// TextureRoll offset.

    dial onesHand_{ bm::vc::vcAlt1Hand_id,	bm::vc::vcAlt1Hand_loc,		bm::vc::AltimeterAxis_loc, bm::pnl::pnlAlt1Hand_id,		bm::pnl::pnlAlt1Hand_vrt,	(360 * RAD), 2.0 };
    dial tensHand_{ bm::vc::vcAlt10Hand_id,	bm::vc::vcAlt10Hand_loc,	bm::vc::AltimeterAxis_loc, bm::pnl::pnlAlt10Hand_id,	bm::pnl::pnlAlt10Hand_vrt,	(360 * RAD), 2.0 };
    dial hundsHand_{ bm::vc::vcAlt100Hand_id,	bm::vc::vcAlt100Hand_loc,	bm::vc::AltimeterAxis_loc, bm::pnl::pnlAlt100Hand_id,	bm::pnl::pnlAlt100Hand_vrt,	(360 * RAD), 2.0 };

    roll tdiAltOnes_{ bm::vc::vcTDIAltOnes_id,		bm::vc::vcTDIAltOnes_vrt,		bm::pnl::pnlTDIAltOnes_id,		bm::pnl::pnlTDIAltOnes_vrt,		ofs };
    roll tdiAltTens_{ bm::vc::vcTDIAltTens_id,		bm::vc::vcTDIAltTens_vrt,		bm::pnl::pnlTDIAltTens_id,		bm::pnl::pnlTDIAltTens_vrt,		ofs };
    roll tdiAltHunds_{ bm::vc::vcTDIAltHunds_id,		bm::vc::vcTDIAltHunds_vrt,		bm::pnl::pnlTDIAltHund_id,		bm::pnl::pnlTDIAltHund_vrt,		ofs };
    roll tdiAltThou_{ bm::vc::vcTDIAltThous_id,		bm::vc::vcTDIAltThous_vrt,		bm::pnl::pnlTDIAltThous_id,		bm::pnl::pnlTDIAltThous_vrt,	ofs };
    roll tdiAltTenThou_{ bm::vc::vcTDIAltTenThous_id,	bm::vc::vcTDIAltTenThous_vrt,	bm::pnl::pnlTDIAltTenThou_id,	bm::pnl::pnlTDIAltTenThou_vrt,	ofs };

    bco::VesselTextureElement       enabledFlag_{
        bm::vc::AltimeterOffFlag_id,
        bm::vc::AltimeterOffFlag_vrt,
        cmn::vc::main,
        bm::pnl::pnlAltimeterOffFlag_id,
        bm::pnl::pnlAltimeterOffFlag_vrt,
        cmn::panel::main
    };

    bco::VesselTextureElement       altimeterExoModeFlag_ {
        bm::vc::AltimeterGround_id,
        bm::vc::AltimeterGround_vrt,
        cmn::vc::main,
        bm::pnl::pnlAltimeterGround_id,
        bm::pnl::pnlAltimeterGround_vrt,
        cmn::panel::main
    };
};

