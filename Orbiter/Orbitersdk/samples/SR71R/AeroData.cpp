//	AeroData - SR-71r Orbiter Addon
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

#include "StdAfx.h"

#include "bc_orbiter\BaseVessel.h"

#include "AeroData.h"

AeroData::AeroData() :
	vessel_component(),
	post_step(),
//	enabledSlot_([&](bool v) {}),
//	avionicsModeSlot_([&](bool v) { avionicsModeSignal_.fire(v ? AvionMode::AvionAtmo : AvionMode::AvionExo); }),
	setCourseIncSlot_([&](bool v) { UpdateSetCourse(0.0175); setCourseIncSlot_.set(); }),
	setCourseDecSlot_([&](bool v) { UpdateSetCourse(-0.0175); setCourseDecSlot_.set(); }),
	setHeadingIncSlot_([&](bool v) { UpdateSetHeading(0.0175); setHeadingIncSlot_.set(); }),
	setHeadingDecSlot_([&](bool v) { UpdateSetHeading(-0.0175); setHeadingDecSlot_.set(); })
{}


// post_step
void AeroData::handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) {
	double gforce	 = 0.0;
	double trim		 = 0.0;
	double aoa		 = 0.0;
	auto   vertSpeed = 0.0;
	double bank		 = 0.0;
	double pitch	 = 0.0;

	isAeroDataActive_.fire(enabledSlot_.value() && (voltsInputSlot_.value() > MIN_VOLTS));

	if (isAeroDataActive_.current()) {
		gforce		= bco::GetVesselGs(&vessel);
		trim		= vessel.GetControlSurfaceLevel(AIRCTRL_ELEVATORTRIM);
		aoa			= vessel.GetAOA();
		vertSpeed	= bco::GetVerticalSpeedFPM(&vessel);
		pitch		= vessel.GetPitch();
		bank		= vessel.GetBank();
	}

	// Vertical speed:
	double isPos = (vertSpeed >= 0) ? 1 : -1;
	auto absSpd = abs(vertSpeed);
	if (absSpd > 6000) absSpd = 6000;
	double spRot = (1 - pow((6000 - absSpd) / 6000, 2)) / 2;
	
	signalBank_		 .fire(bank);
	signalPitch_	 .fire(0.100093 * pitch);
	vsiNeedleSignal_ .fire(0.5 + (isPos * spRot));
	gforceSignal_	 .fire(gforce);
	trimSignal_		 .fire(trim);
	aoaSignal_		 .fire(aoa);
}

// manage_state
bool AeroData::handle_load_state(const std::string& line) {
	// [a b]  :  [course heading]
	int course = 0.0;
	int heading = 0.0;

	std::istringstream in(line);

	if (in >> course >> heading) {
		setCourseSignal_.fire(course);
		setHeadingSignal_.fire(heading);
		return true;
	}
	else {
		return false;
	}

}

std::string AeroData::handle_save_state() {
	std::ostringstream os;
	os << setCourseSignal_.current() << " " << setHeadingSignal_.current();
	return os.str();
}

void AeroData::SetCourse(double s)
{
	setCourseSignal_.update(s * RAD);
	UpdateSetCourse(0.0);	 // force the signal to fire.
}

void AeroData::UpdateSetCourse(double i)
{
	auto inc = setCourseSignal_.current() + i;
	if (inc > PI2) inc -= PI2;
	if (inc < 0) inc += PI2;
	setCourseSignal_.fire(inc);

//	sprintf(oapiDebugString(), "Set Course: %+4.2f", setCourseSignal_.current());
}

void AeroData::SetHeading(double s)
{
	setHeadingSignal_.update(s * RAD);
	UpdateSetHeading(0.0);	 // force the signal to fire.
}

void AeroData::UpdateSetHeading(double i)
{
	auto inc = setHeadingSignal_.current() + i;
	if (inc > PI2) inc -= PI2;
	if (inc < 0) inc += PI2;
	setHeadingSignal_.fire(inc);

	//	sprintf(oapiDebugString(), "Set Course: %+4.2f", setCourseSignal_.current());
}
