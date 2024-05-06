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

#include "../bc_orbiter/control.h"
#include "../bc_orbiter/signals.h"
#include "../bc_orbiter/vessel.h"
#include "../bc_orbiter/status_display.h"

#include "Avionics.h"

namespace bco = bc_orbiter;

class Airspeed :
	  public bco::vessel_component
	, public bco::post_step 
{
public:

	Airspeed(bco::vessel& vessel, Avionics& avionics) : 
		avionics_(avionics)
	{
		vessel.AddControl(&kiesHand_);
		vessel.AddControl(&machHand_);
		vessel.AddControl(&maxMachHand_);

		vessel.AddControl(&tdiKeasOnes_);
		vessel.AddControl(&tdieasTens_);
		vessel.AddControl(&tdiKeasHunds_);

		vessel.AddControl(&tdiMhOnes_);
		vessel.AddControl(&tdiMhTens_);
		vessel.AddControl(&tdiMhHunds_);

		vessel.AddControl(&enabledFlag_);
		vessel.AddControl(&velocityFlag_);

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

		if (avionics_.IsAeroActive()) {
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

			if (!avionics_.IsAeroAtmoMode())  // if exo mode, use velocity for machGauge
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
			kiasSpeed = (avionics_.IsAeroAtmoMode()) ? (speedRatio - kRatio) : speedRatio;	// If atmo, subtract the speedRatio to get correct rotation.
		}

		bco::TensParts parts;
		bco::GetDigits(keas, parts);
		tdiKeasOnes_.set_position(parts.Tens);
		tdieasTens_.set_position(parts.Hundreds);
		tdiKeasHunds_.set_position(parts.Thousands);

		maxMachHand_.set_state(maxMachRatio);
		machHand_.set_state(speedRatio);
		kiesHand_.set_state(speedRatio - (kias / 600));

//		sprintf(oapiDebugString(), "speedRatio (mach hand): %+4.2f", speedRatio);

		bco::GetDigits(mach, parts);
		tdiMhOnes_.set_position(parts.Tenths);
		tdiMhTens_.set_position(parts.Tens);
		tdiMhHunds_.set_position(parts.Hundreds);

		status_.set_state(isOverSpeed ? bco::status_display::status::error : bco::status_display::status::off);

		enabledFlag_.set_state(avionics_.IsAeroActive());

		velocityFlag_.set_state(
			avionics_.IsAeroActive()
			? avionics_.IsAeroAtmoMode()
			: true);
	}

private:
	Avionics& avionics_;

	const double MaxPress = 60000.0; // 30.0 * 1000 * 2 = 60000 --> a guess at the dynamic values of SR71r.
	// ** SPEED **
	const double MIN_PIN		=  0.0;
	const double MAX_PIN_RAD	=  5.236; // 300.0 deg
	const double MAX_MACH		= 22.0;
	double l22 = log(23);

	using dial = bco::rotary_display_target;
	using roll = bco::flat_roll;
	const double ofs = 0.1084;		// flat_roll offset.

	dial machHand_		{ bm::vc::vcMachHand_id,	bm::vc::vcMachHand_loc,		bm::vc::SpeedAxis_loc,	bm::pnl::pnlMachHand_id,	bm::pnl::pnlMachHand_vrt,	(300 * RAD), 2.0 };
	dial kiesHand_		{ bm::vc::vcKiesHand_id,	bm::vc::vcKiesHand_loc,		bm::vc::SpeedAxis_loc,	bm::pnl::pnlKiesHand_id,	bm::pnl::pnlKiesHand_vrt,	(300 * RAD), 2.0 };
	dial maxMachHand_	{ bm::vc::vcMachMaxHand_id,	bm::vc::vcMachMaxHand_loc,	bm::vc::SpeedAxis_loc,	bm::pnl::pnlMachMaxHand_id,	bm::pnl::pnlMachMaxHand_vrt,(300 * RAD), 2.0 };

	roll tdiKeasOnes_	{ bm::vc::vcTDIKeasOnes_id,		bm::vc::vcTDIKeasOnes_vrt,	bm::pnl::pnlTDIKEASOnes_id,		bm::pnl::pnlTDIKEASOnes_vrt,	ofs };
	roll tdieasTens_	{ bm::vc::vcTDIKeasTens_id,		bm::vc::vcTDIKeasTens_vrt,	bm::pnl::pnlTDIKEASTens_id,		bm::pnl::pnlTDIKEASTens_vrt,	ofs };
	roll tdiKeasHunds_	{ bm::vc::vcTDIKeasHunds_id,	bm::vc::vcTDIKeasHunds_vrt,	bm::pnl::pnlTDIKEASHunds_id,	bm::pnl::pnlTDIKEASHunds_vrt,	ofs };

	roll tdiMhOnes_		{ bm::vc::vcTDIMachOne_id,		bm::vc::vcTDIMachOne_vrt,	bm::pnl::pnlTDIMACHOne_id,		bm::pnl::pnlTDIMACHOne_vrt,		ofs };
	roll tdiMhTens_		{ bm::vc::vcTDIMachTens_id,		bm::vc::vcTDIMachTens_vrt,	bm::pnl::pnlTDIMACHTens_id,		bm::pnl::pnlTDIMACHTens_vrt,	ofs };
	roll tdiMhHunds_	{ bm::vc::vcTDIMachHunds_id,	bm::vc::vcTDIMachHunds_vrt,	bm::pnl::pnlTDIMACHHunds_id,	bm::pnl::pnlTDIMACHHunds_vrt,	ofs };

	bco::on_off_display		enabledFlag_{
			bm::vc::SpeedFlagOff_id,
			bm::vc::SpeedFlagOff_vrt,
			bm::pnl::pnlSpeedFlagOff_id,
			bm::pnl::pnlSpeedFlagOff_vrt,
			0.0244
	};

	// Flags: true is off (not showing)
	bco::on_off_display		velocityFlag_{
		bm::vc::SpeedVelocityFlag_id,
			bm::vc::SpeedVelocityFlag_vrt,
			bm::pnl::pnlSpeedVelocityFlag_id,
			bm::pnl::pnlSpeedVelocityFlag_vrt,
			0.0244
	};

	bco::status_display     status_{
		bm::vc::MsgLightKeasWarn_id,
		bm::vc::MsgLightKeasWarn_vrt,
		bm::pnl::pnlMsgLightKeasWarn_id,
		bm::pnl::pnlMsgLightKeasWarn_vrt,
		0.0361
	};
};


