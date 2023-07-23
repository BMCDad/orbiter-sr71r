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

#include "bc_orbiter/control.h"
#include "bc_orbiter/signals.h"
#include "bc_orbiter/vessel.h"
#include "bc_orbiter/rotary_display.h"
#include "bc_orbiter/flat_roll.h"

#include "AvionBase.h"

namespace bco = bc_orbiter;

class Altimeter :
	public AvionBase,
	public bco::post_step {

public:

	Altimeter(bco::vessel& vessel) {
		vessel.AddControl(&altimeter1Hand_);
		vessel.AddControl(&altimeter10Hand_);
		vessel.AddControl(&altimeter100Hand_);

		vessel.AddControl(&tdiAltOnes_);
		vessel.AddControl(&tdiAltTens_);
		vessel.AddControl(&tdiAltHunds_);
		vessel.AddControl(&tdiAltThou_);
		vessel.AddControl(&tdiAltTenThou_);

		vessel.AddControl(&altimeterEnabledFlag_);
		vessel.AddControl(&altimeterExoModeFlag_);
	}

	~Altimeter() {}

	void OnEnabledChanged() override { OnChanged(); }
	void OnAvionModeChanged() override { OnChanged(); }


	// post_step
	void handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd) override {
		double altFeet = 0.0;
		if (EnabledSlot().value()) {
			auto altMode = AvionicsModeSlot().value() ? AltitudeMode::ALTMODE_GROUND : AltitudeMode::ALTMODE_MEANRAD;
			int res = 0;
			auto alt = vessel.GetAltitude(altMode, &res);
			altFeet = alt * 3.28084;
		}

		bco::TensParts parts;

		// ** ALTIMETER **
		bco::BreakTens((altFeet > 100000) ? 0 : altFeet, parts);  // Limit to 100000

		altimeter1Hand_.set_state(parts.Tens);
		altimeter10Hand_.set_state(parts.Hundreds);
		altimeter100Hand_.set_state(parts.Thousands);

		tdiAltOnes_.SlotTransform().notify(parts.Tens);
		tdiAltTens_.SlotTransform().notify(parts.Hundreds);
		tdiAltHunds_.SlotTransform().notify(parts.Thousands);
		tdiAltThou_.SlotTransform().notify(parts.TenThousands);
		tdiAltTenThou_.SlotTransform().notify(parts.HundredThousands);
	}

private:

	void OnChanged() {
		// Logic for flags and stuff here.
		altimeterEnabledFlag_.set_state(EnabledSlot().value());
		altimeterExoModeFlag_.set_state(!EnabledSlot().value() ? true : AvionicsModeSlot().value());
	}

	bco::rotary_display<bco::animation_wrap>	altimeter1Hand_{
		{ bm::vc::gaAlt1Needle_id },
			bm::vc::gaAlt1Needle_location, bm::vc::AltimeterAxis_location,
			bm::pnl::pnlAlt1Needle_id,
			bm::pnl::pnlAlt1Needle_verts,
			(360 * RAD),	// Clockwise
			2.0,
			[](double d) {return ((double)d / 10.0); }	// Transform to anim range.
	};

	bco::rotary_display<bco::animation_wrap>	altimeter10Hand_{
		{ bm::vc::gaAlt10Needle_id },
			bm::vc::gaAlt10Needle_location, bm::vc::AltimeterAxis_location,
			bm::pnl::pnlAlt10Needle_id,
			bm::pnl::pnlAlt10Needle_verts,
			(360 * RAD),	// Clockwise
			2.0,
			[](double d) {return (d / 10); }	// Transform to anim range.
	};

	bco::rotary_display<bco::animation_wrap>	altimeter100Hand_{
		{ bm::vc::gaAlt100Needle_id },
			bm::vc::gaAlt100Needle_location, bm::vc::AltimeterAxis_location,
			bm::pnl::pnlAlt100Needle_id,
			bm::pnl::pnlAlt100Needle_verts,
			(360 * RAD),	// Clockwise
			2.0,
			[](double d) {return (d / 10); }	// Transform to anim range.
	};

	bco::flat_roll			tdiAltOnes_{
		bm::vc::vcTDIAltOnes_id,
			bm::vc::vcTDIAltOnes_verts,
			bm::pnl::pnlTDIAltOnes_id,
			bm::pnl::pnlTDIAltOnes_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::flat_roll			tdiAltTens_{
		bm::vc::vcTDIAltTens_id,
			bm::vc::vcTDIAltTens_verts,
			bm::pnl::pnlTDIAltTens_id,
			bm::pnl::pnlTDIAltTens_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::flat_roll			tdiAltHunds_{
		bm::vc::vcTDIAltHunds_id,
			bm::vc::vcTDIAltHunds_verts,
			bm::pnl::pnlTDIAltHund_id,
			bm::pnl::pnlTDIAltHund_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::flat_roll			tdiAltThou_{
		bm::vc::vcTDIAltThous_id,
			bm::vc::vcTDIAltThous_verts,
			bm::pnl::pnlTDIAltThous_id,
			bm::pnl::pnlTDIAltThous_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::flat_roll			tdiAltTenThou_{
		bm::vc::vcTDIAltTenThous_id,
			bm::vc::vcTDIAltTenThous_verts,
			bm::pnl::pnlTDIAltTenThou_id,
			bm::pnl::pnlTDIAltTenThou_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::on_off_display		altimeterEnabledFlag_{
		bm::vc::AltimeterOffFlag_id,
			bm::vc::AltimeterOffFlag_verts,
			bm::pnl::pnlAltimeterOffFlag_id,
			bm::pnl::pnlAltimeterOffFlag_verts,
			0.0244
	};

	bco::on_off_display		altimeterExoModeFlag_{
		bm::vc::AltimeterGround_id,
			bm::vc::AltimeterGround_verts,
			bm::pnl::pnlAltimeterGround_id,
			bm::pnl::pnlAltimeterGround_verts,
			0.0244
	};

};

