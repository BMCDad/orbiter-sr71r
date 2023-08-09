//	Airspeed - SR-71r Orbiter Addon
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
#include "bc_orbiter/status_display.h"

#include "AvionBase.h"

namespace bco = bc_orbiter;

class Airspeed :
	  public AvionBase
	, public bco::post_step 
{
public:

	Airspeed(bco::vessel& vessel) {
		vessel.AddControl(&kiesHand_);
		vessel.AddControl(&machHand_);
		vessel.AddControl(&maxMachHand_);

		vessel.AddControl(&airspeedKeasOnes_);
		vessel.AddControl(&airspeedKeasTens_);
		vessel.AddControl(&airspeedKeasHunds_);

		vessel.AddControl(&airspeedMACHOnes_);
		vessel.AddControl(&airspeedMACHTens_);
		vessel.AddControl(&airspeedMACHHunds_);

		vessel.AddControl(&speedIsEnabledFlag_);
		vessel.AddControl(&speedIsVelocityFlag_);

		vessel.AddControl(&status_);
	}

	~Airspeed() {}

	// post_step
	void handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd) override {
		double  keas			= 0.0;		// equivalent airspeed, shows in TDI
		double  kias			= 0.0;		// indicated, shows as dial.
		double  mach			= 0.0;		// shows in TDI and as a dial
		double  maxMach			= 0.0;		// Error bar on dial
		double  speedRatio		= 0.0;		// 
		double  maxMachRatio	= 0.0;
		double  kiasSpeed		= 0.0;
		bool	isOverSpeed		= false;
		bool	isAtmoMode		= AvionicsModeSlot().value();

		if (EnabledSlot().value()) {
			keas = bco::GetVesselKeas(&vessel);
			kias = bco::GetVesselKias(&vessel);
			mach = vessel.GetMachNumber();
			auto atmDens = vessel.GetAtmDensity();

			if (atmDens > 0.0)
			{
				maxMach = sqrt(MaxPress / atmDens) / 331.34;
				isOverSpeed = mach > maxMach;
			}

			auto machGauge = (mach > MAX_MACH) ? MAX_MACH : mach;		// machGauge will be pinned, mach itself will be used to set the dials, so it not pinned.

			if (!isAtmoMode)  // if exo mode, use velocity for machGauge
			{
				machGauge = vessel.GetAirspeed() / 100;
				maxMach = 22.0;
			}

			// Set WARNING MAX MACH gauge : MACH scale is log base 22 (max mach):
			// LOG range 1 to 23 (22 positions) to avoid <1 values.

			// Calculate the max mach needle.
			if (maxMach > 22.0) maxMach = 22.0;		// Pin MAX to 22 and 1
			if (maxMach < 0.0) maxMach = 0.0;
			maxMachRatio = (maxMach == 0.0)
				? 0.0 
				: ((log(maxMach + 1) / l22) * MAX_PIN_RAD) / MAX_PIN_RAD;	// Determine LOG based on speed then convert to 0-1 ration for the gauge.

			// Kies dial
			speedRatio = ((log(machGauge + 1.0) / l22) * MAX_PIN_RAD) / MAX_PIN_RAD;
			auto kRatio = kias / 600; // 600 max kias speed
			kiasSpeed = (isAtmoMode) ? (speedRatio - kRatio) : speedRatio;	// If atmo, subtract the speedRatio to get correct rotation.
		}

		bco::TensParts parts;
		bco::GetDigits(keas, parts);
		airspeedKeasOnes_.SlotTransform().notify(parts.Tens);
		airspeedKeasTens_.SlotTransform().notify(parts.Hundreds);
		airspeedKeasHunds_.SlotTransform().notify(parts.Thousands);

		maxMachHand_.set_state(maxMachRatio);
		machHand_.set_state(speedRatio);
		kiesHand_.set_state(speedRatio - (kias / 600));

//		sprintf(oapiDebugString(), "speedRatio (mach hand): %+4.2f", speedRatio);

		bco::GetDigits(mach, parts);
		airspeedMACHOnes_.SlotTransform().notify(parts.Tenths);
		airspeedMACHTens_.SlotTransform().notify(parts.Tens);
		airspeedMACHHunds_.SlotTransform().notify(parts.Hundreds);

		status_.set_state(isOverSpeed ? bco::status_display::status::error : bco::status_display::status::off);

		speedIsEnabledFlag_.set_state(EnabledSlot().value());

		speedIsVelocityFlag_.set_state(
			EnabledSlot().value() 
			? AvionicsModeSlot().value()
			: true);
	}

private:
	const double MaxPress = 60000.0; // 30.0 * 1000 * 2 = 60000 --> a guess at the dynamic values of SR71r.
	// ** SPEED **
	const double MIN_PIN		=  0.0;
	const double MAX_PIN_RAD	=  5.236; // 300.0 deg
	const double MAX_MACH		= 22.0;
	double l22 = log(23);

	bco::rotary_display_target	machHand_{
		{ bm::vc::SpeedNeedle_id },
			bm::vc::SpeedNeedle_location, bm::vc::SpeedAxis_location,
			bm::pnl::pnlSpeedNeedle_id,
			bm::pnl::pnlSpeedNeedle_verts,
			(300 * RAD),	// Clockwise
			2.0
	};

	bco::rotary_display_target	kiesHand_{
		{ bm::vc::SpeedIndicatorKies_id },
			bm::vc::SpeedIndicatorKies_location, bm::vc::SpeedAxis_location,
			bm::pnl::pnlSpeedIndicatorKies_id,
			bm::pnl::pnlSpeedIndicatorKies_verts,
			(300 * RAD),	// Clockwise
			2.0
	};

	bco::rotary_display_target	maxMachHand_{
		{ bm::vc::SpeedNeedleMax_id },
			bm::vc::SpeedNeedleMax_location, bm::vc::SpeedAxis_location,
			bm::pnl::pnlSpeedNeedleMax_id,
			bm::pnl::pnlSpeedNeedleMax_verts,
			(300 * RAD),	// Clockwise
			2.0
	};

	bco::flat_roll			airspeedKeasOnes_{
		bm::vc::vcTDIKeasOnes_id,
			bm::vc::vcTDIKeasOnes_verts,
			bm::pnl::pnlTDIKEASOnes_id,
			bm::pnl::pnlTDIKEASOnes_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::flat_roll			airspeedKeasTens_{
		bm::vc::vcTDIKeasTens_id,
			bm::vc::vcTDIKeasTens_verts,
			bm::pnl::pnlTDIKEASTens_id,
			bm::pnl::pnlTDIKEASTens_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::flat_roll			airspeedKeasHunds_{
		bm::vc::vcTDIKeasHunds_id,
			bm::vc::vcTDIKeasHunds_verts,
			bm::pnl::pnlTDIKEASHunds_id,
			bm::pnl::pnlTDIKEASHunds_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::flat_roll			airspeedMACHOnes_{
		bm::vc::vcTDIMachOne_id,
			bm::vc::vcTDIMachOne_verts,
			bm::pnl::pnlTDIMACHOne_id,
			bm::pnl::pnlTDIMACHOne_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::flat_roll			airspeedMACHTens_{
		bm::vc::vcTDIMachTens_id,
			bm::vc::vcTDIMachTens_verts,
			bm::pnl::pnlTDIMACHTens_id,
			bm::pnl::pnlTDIMACHTens_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::flat_roll			airspeedMACHHunds_{
		bm::vc::vcTDIMachHunds_id,
			bm::vc::vcTDIMachHunds_verts,
			bm::pnl::pnlTDIMACHHunds_id,
			bm::pnl::pnlTDIMACHHunds_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::on_off_display		speedIsEnabledFlag_{
			bm::vc::SpeedFlagOff_id,
			bm::vc::SpeedFlagOff_verts,
			bm::pnl::pnlSpeedFlagOff_id,
			bm::pnl::pnlSpeedFlagOff_verts,
			0.0244
	};

	// Flags: true is off (not showing)
	bco::on_off_display		speedIsVelocityFlag_{
		bm::vc::SpeedVelocityFlag_id,
			bm::vc::SpeedVelocityFlag_verts,
			bm::pnl::pnlSpeedVelocityFlag_id,
			bm::pnl::pnlSpeedVelocityFlag_verts,
			0.0244
	};

	bco::status_display     status_{
		bm::vc::MsgLightKeasWarn_id,
		bm::vc::MsgLightKeasWarn_verts,
		bm::pnl::pnlMsgLightKeasWarn_id,
		bm::pnl::pnlMsgLightKeasWarn_verts,
		0.0361
	};
};


