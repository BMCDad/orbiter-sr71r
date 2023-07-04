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

#include "bc_orbiter\control.h"
#include "bc_orbiter\signals.h"
#include "bc_orbiter\BaseVessel.h"

#include "AvionBase.h"

namespace bco = bc_orbiter;

class Airspeed :
	public AvionBase,
	public bco::post_step {

public:

	Airspeed() {}
	~Airspeed() {}

	void OnEnabledChanged() override {
		signalIsEnabled_.fire(EnabledSlot().value());
		OnAvionModeChanged();
	}

	void OnAvionModeChanged() override {
		signalIsVelocityFlag_.fire(EnabledSlot().value() && AvionicsModeSlot().value());
	}

	// post_step
	void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override {
		auto keas = 0.0;
		auto kias = 0.0;
		double mach = 0.0;
		double maxMach = 0.0;
		double speedRatio = 0.0;
		double maxMachRatio = 0.0;
		double kiasSpeed = 0.0;

		if (EnabledSlot().value()) {
			keas = bco::GetVesselKeas(&vessel);
			kias = bco::GetVesselKias(&vessel);
			mach = vessel.GetMachNumber();
			auto atmDens = vessel.GetAtmDensity();

			if (atmDens > 0.0)
			{
				maxMach = sqrt(MaxPress / atmDens) / 331.34;
				signalIsOverspeed_.fire(mach > maxMach);
			}

			auto spGauge = (mach > MAX_MACH) ? MAX_MACH : mach;

			if (AvionicsModeSlot().value())  // if exo mode, use velocity for spGauge
			{
				spGauge = vessel.GetAirspeed() / 100;
				maxMach = 22.0;
			}

			// Set WARNING MAX MACH gauge : MACH scale is log base 22 (max mach):
			// LOG range 1 to 23 (22 positions) to avoid <1 values.
			auto r = MAX_PIN * RAD;					// Max amount in RAD the gauge can move.

			if (maxMach > 22.0) maxMach = 22.0;		// Pin MAX to 22 and 1
			if (maxMach < 0.0) maxMach = 0.0;

			auto rm = (maxMach == 0.0) ? 0.0 : log(maxMach + 1) / l22;	// Determine LOG based on speed.
			auto mmRot = rm * r;					// Get rotation RAD
			maxMachRatio = mmRot / r;

			// Kies dial
			auto rr = log(spGauge + 1.0) / l22;
			auto spRot = rr * r;

			speedRatio = spRot / r;
			auto kRatio = kias / 600;
			kiasSpeed = (AvionicsModeSlot().value()) ? speedRatio : (kRatio - speedRatio);
		}

		bco::TensParts parts;
		bco::GetDigits(keas, parts);
		signalKeas_.fire(keas);
		signalKeasOnes_.fire(parts.Tens);
		signalKeasTens_.fire(parts.Hundreds);
		signalKeasHunds_.fire(parts.Thousands);
		signalMachMax_.fire(maxMachRatio);			// Set based on ratio
		signalSpeed_.fire(speedRatio);
		signalKiasSpeed_.fire(kiasSpeed);

		bco::GetDigits(mach, parts);
		signalMACHOnes_.fire(parts.Tens);
		signalMACHTens_.fire(parts.Hundreds);
		signalMACHHunds_.fire(parts.Thousands);
	}

	bco::signal<double>&	AirspeedKeasSignal()	{ return signalKeas_; }
	bco::signal<double>&	KeasOnesSignal()		{ return signalKeasOnes_; }
	bco::signal<double>&	KeasTensSignal()		{ return signalKeasTens_; }
	bco::signal<double>&	KeasHundsSignal()		{ return signalKeasHunds_; }
	bco::signal<bool>&		IsOverspeedSignal()		{ return signalIsOverspeed_; }
	bco::signal<double>&	MACHMaxSignal()			{ return signalMachMax_; }
	bco::signal<double>&	SpeedSignal()			{ return signalSpeed_; }
	bco::signal<double>&	KiasSpeedSignal()		{ return signalKiasSpeed_; }
	bco::signal<bool>&		IsEnabledSignal()		{ return signalIsEnabled_; }
	bco::signal<bool>&		IsVelocityFlagSignal()	{ return signalIsVelocityFlag_; }
	bco::signal<double>&	MACHOnesSignal()		{ return signalMACHOnes_; }
	bco::signal<double>&	MACHTensSignal()		{ return signalMACHTens_; }
	bco::signal<double>&	MACHHundsSignal()		{ return signalMACHHunds_; }


private:
	const double MaxPress = 60000.0; // 30.0 * 1000 * 2 = 60000 --> a guess at the dynamic values of SR71r.
	// ** SPEED **
	const double MIN_PIN = 0.0;
	const double MAX_PIN = 300.0;
	const double MAX_MACH = 22.0;
	double l22 = log(23);

	bco::signal<double>	signalKeas_;
	bco::signal<double>	signalKeasOnes_;
	bco::signal<double>	signalKeasTens_;
	bco::signal<double>	signalKeasHunds_;
	bco::signal<bool>	signalIsOverspeed_;
	bco::signal<double> signalMachMax_;
	bco::signal<double> signalSpeed_;
	bco::signal<double> signalKiasSpeed_;
	bco::signal<bool>	signalIsEnabled_;
	bco::signal<bool>	signalIsVelocityFlag_;
	bco::signal<double>	signalMACHOnes_;
	bco::signal<double>	signalMACHTens_;
	bco::signal<double>	signalMACHHunds_;
};


