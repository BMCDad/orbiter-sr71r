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

#include "bc_orbiter\control.h"
#include "bc_orbiter\signals.h"
#include "bc_orbiter\BaseVessel.h"

#include "AvionBase.h"

namespace bco = bc_orbiter;

class Altimeter :
	public AvionBase,
	public bco::post_step {

public:

	Altimeter() {}
	~Altimeter() {}

	void OnEnabledChanged() override { OnChanged(); }
	void OnAvionModeChanged() override { OnChanged(); }


	// post_step
	void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override {
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

		altimeterTens_.fire(parts.Tens);
		altimeterHundreds_.fire(parts.Hundreds);
		altimeterThousands_.fire(parts.Thousands);
		altimeterTenThousands_.fire(parts.TenThousands);
		altimeterHundThousands_.fire(parts.HundredThousands);
	}

	bco::signal<double>&	AltimeterTensSignal()			{ return altimeterTens_; }
	bco::signal<double>&	AltimeterHundredsSignal()		{ return altimeterHundreds_; }
	bco::signal<double>&	AltimeterThousandsSignal()		{ return altimeterThousands_; }
	bco::signal<double>&	AltimeterTenThousandsSignal()	{ return altimeterTenThousands_; }
	bco::signal<double>&	AltimeterHundThousandsSignal()	{ return altimeterHundThousands_; }

	bco::signal<bool>&		IsEnabledSignal()				{ return isEnabledSignal_; }
	bco::signal<bool>&		IsExoModeSignal()				{ return isExoMode_; }

private:

	void OnChanged() {
		// Logic for flags and stuff here.
		isEnabledSignal_.fire(EnabledSlot().value());
		isExoMode_.fire(EnabledSlot().value() && AvionicsModeSlot().value());
	}

	bco::signal<double>	altimeterTens_;
	bco::signal<double>	altimeterHundreds_;
	bco::signal<double>	altimeterThousands_;
	bco::signal<double>	altimeterTenThousands_;
	bco::signal<double>	altimeterHundThousands_;

	bco::signal<bool> isEnabledSignal_;
	bco::signal<bool> isExoMode_;
};

