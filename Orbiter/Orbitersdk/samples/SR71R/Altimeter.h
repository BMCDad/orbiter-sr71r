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

#include "VesselAPI.h"

#include "bc_orbiter/control.h"
#include "bc_orbiter/signals.h"
#include "bc_orbiter/vessel.h"
#include "bc_orbiter/rotary_display.h"
//#include "bc_orbiter/flat_roll.h"
#include "bc_orbiter/vc_flat_roll.h"
#include "bc_orbiter/panel_flat_roll.h"

#include "Avionics.h"


namespace bco = bc_orbiter;

class Altimeter : public bco::VesselComponent, public bco::HandlesPostStep {
 public:

  Altimeter(bco::Vessel& Vessel, Avionics& avionics) :
      avionics_(avionics) {
    Vessel.AddControl(&onesHand_);
    Vessel.AddControl(&tensHand_);
    Vessel.AddControl(&hundsHand_);

    Vessel.AddControl(&vc_tdiAltOnes_);
    Vessel.AddControl(&vc_tdiAltTens_);
    Vessel.AddControl(&vc_tdiAltHunds_);
    Vessel.AddControl(&vc_tdiAltThou_);
    Vessel.AddControl(&vc_tdiAltTenThou_);

    Vessel.AddControl(&pnl_tdiAltOnes_);
    Vessel.AddControl(&pnl_tdiAltTens_);
    Vessel.AddControl(&pnl_tdiAltHunds_);
    Vessel.AddControl(&pnl_tdiAltThou_);
    Vessel.AddControl(&pnl_tdiAltTenThou_);

    Vessel.AddControl(&enabledFlag_);
    Vessel.AddControl(&altimeterExoModeFlag_);
  }
  
  ~Altimeter() {}

  // post_step
  void HandlePostStep(bco::Vessel& Vessel, double simt, double simdt, double mjd) override {
    double altFeet = 0.0;
    if (avionics_.IsAeroActive()) {
      auto altMode = avionics_.IsAeroAtmoMode() ? AltitudeMode::ALTMODE_GROUND : AltitudeMode::ALTMODE_MEANRAD;
      int res = 0;
      auto alt = Vessel.GetAltitude(altMode, &res);
      altFeet = alt * 3.28084;
    }

    bco::IntParts iOut;
    if (altFeet < 100000) {
      bco::BreakTens((altFeet > 100000 ? 0.0 : altFeet), iOut);
      onesHand_.set_state(iOut.Hundreds / 10.0);
      tensHand_.set_state(iOut.Thousands / 10.0);
      hundsHand_.set_state(iOut.TenThousands / 10.0);
    }
    else {
      onesHand_.set_state(0);
      tensHand_.set_state(0);
      hundsHand_.set_state(0);
    }

    // ** ALTIMETER **
    bco::TensParts parts;
    bco::GetDigits((altFeet > 1000000) ? 0 : altFeet, parts);  // Limit to 100000

    vc_tdiAltOnes_.SetPosition(parts.Hundreds);
    vc_tdiAltTens_.SetPosition(parts.Thousands);
    vc_tdiAltHunds_.SetPosition(parts.TenThousands);
    vc_tdiAltThou_.SetPosition(parts.HundredThousands);
    vc_tdiAltTenThou_.SetPosition(parts.Millions);

    pnl_tdiAltOnes_.SetPosition(parts.Hundreds);
    pnl_tdiAltTens_.SetPosition(parts.Thousands);
    pnl_tdiAltHunds_.SetPosition(parts.TenThousands);
    pnl_tdiAltThou_.SetPosition(parts.HundredThousands);
    pnl_tdiAltTenThou_.SetPosition(parts.Millions);

    altimeterExoModeFlag_.set_state(    // FALSE show flag, TRUE hide flag
      !avionics_.IsAeroActive()         // So if altimeter is OFF set TRUE so the flag does NOT show.
        ? true
        : avionics_.IsAeroAtmoMode());  // True for this switch means ATMO mode.

    enabledFlag_.set_state(             // FALSE show flag, TRUE hide flag
      !avionics_.IsAeroActive()
        ? false                         // Avionics off, so show flag.
        : (	(altFeet > 100000) &&       // If enabled, then SHOW if > 100000 ft
      avionics_.IsAeroAtmoMode()        // AND we are in atmo mode.
        ? false
        : true));
	}

private:
  Avionics& avionics_;

  using dial = bco::rotary_display_wrap;
  using vc_roll = bco::VCFlatRoll;
  using pnl_roll = bco::PanelFlatRoll;

  const double ofs = 0.1084;		// flat_roll offset.

  dial onesHand_	{ bm::vc::vcAlt1Hand_id,	bm::vc::vcAlt1Hand_loc,		bm::vc::AltimeterAxis_loc, bm::pnl::pnlAlt1Hand_id,		bm::pnl::pnlAlt1Hand_vrt,	(360 * RAD), 2.0};
  dial tensHand_	{ bm::vc::vcAlt10Hand_id,	bm::vc::vcAlt10Hand_loc,	bm::vc::AltimeterAxis_loc, bm::pnl::pnlAlt10Hand_id,	bm::pnl::pnlAlt10Hand_vrt,	(360 * RAD), 2.0};
  dial hundsHand_	{ bm::vc::vcAlt100Hand_id,	bm::vc::vcAlt100Hand_loc,	bm::vc::AltimeterAxis_loc, bm::pnl::pnlAlt100Hand_id,	bm::pnl::pnlAlt100Hand_vrt,	(360 * RAD), 2.0};

  vc_roll vc_tdiAltOnes_    { bm::vc::vcTDIAltOnes_id,      bm::vc::vcTDIAltOnes_vrt,     ofs };
  vc_roll vc_tdiAltTens_    { bm::vc::vcTDIAltTens_id,      bm::vc::vcTDIAltTens_vrt,     ofs };
  vc_roll vc_tdiAltHunds_   { bm::vc::vcTDIAltHunds_id,     bm::vc::vcTDIAltHunds_vrt,    ofs };
  vc_roll vc_tdiAltThou_    { bm::vc::vcTDIAltThous_id,     bm::vc::vcTDIAltThous_vrt,    ofs };
  vc_roll vc_tdiAltTenThou_ { bm::vc::vcTDIAltTenThous_id,  bm::vc::vcTDIAltTenThous_vrt, ofs };

  pnl_roll pnl_tdiAltOnes_    { bm::pnl::pnlTDIAltOnes_id,		bm::pnl::pnlTDIAltOnes_vrt,		  ofs };
  pnl_roll pnl_tdiAltTens_    { bm::pnl::pnlTDIAltTens_id,		bm::pnl::pnlTDIAltTens_vrt,		  ofs };
  pnl_roll pnl_tdiAltHunds_   { bm::pnl::pnlTDIAltHund_id,		bm::pnl::pnlTDIAltHund_vrt,		  ofs };
  pnl_roll pnl_tdiAltThou_    { bm::pnl::pnlTDIAltThous_id,		bm::pnl::pnlTDIAltThous_vrt,	  ofs };
  pnl_roll pnl_tdiAltTenThou_ { bm::pnl::pnlTDIAltTenThou_id,	bm::pnl::pnlTDIAltTenThou_vrt,  ofs };

	bco::on_off_display		enabledFlag_{
		bm::vc::AltimeterOffFlag_id,
			bm::vc::AltimeterOffFlag_vrt,
			bm::pnl::pnlAltimeterOffFlag_id,
			bm::pnl::pnlAltimeterOffFlag_vrt,
			0.0244
	};

	bco::on_off_display		altimeterExoModeFlag_{
		bm::vc::AltimeterGround_id,
			bm::vc::AltimeterGround_vrt,
			bm::pnl::pnlAltimeterGround_id,
			bm::pnl::pnlAltimeterGround_vrt,
			0.0244
	};
};

