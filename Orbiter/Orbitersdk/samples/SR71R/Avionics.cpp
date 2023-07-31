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

#include "bc_orbiter\vessel.h"

#include "Avionics.h"

Avionics::Avionics(bco::power_provider& pwr, bco::vessel& vessel) :
	power_(pwr),
	setHeadingSlot_([&](double v) { setHeadingSignal_.fire(v); })
{
	power_.attach_consumer(this);

	vessel.AddControl(&switchAvionMode_);
	vessel.AddControl(&switchAvionPower_);
	vessel.AddControl(&switchNavMode_);

	vessel.AddControl(&dialSetCourseDecrement_);
	vessel.AddControl(&dialSetCourseIncrement_);
	vessel.AddControl(&dialSetHeadingDecrement_);
	vessel.AddControl(&dialSetHeadingIncrement_);

	dialSetCourseDecrement_.attach([&]() { UpdateSetCourse(-0.0175); });
	dialSetCourseIncrement_.attach([&]() { UpdateSetCourse(0.0175); });
	dialSetHeadingDecrement_.attach([&]() { UpdateSetHeading(-0.0175); });
	dialSetHeadingIncrement_.attach([&]() { UpdateSetHeading(0.0175); });

	vessel.AddControl(&vsiHand_);
	vessel.AddControl(&vsiActiveFlag_);
	
	vessel.AddControl(&attitudeDisplay_);
	vessel.AddControl(&attitudeFlag_);

	vessel.AddControl(&aoaHand_);
	vessel.AddControl(&trimHand_);
	vessel.AddControl(&accelHand_);
}


// post_step
void Avionics::handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd) {
	double gforce	 = 0.0;
	double trim		 = 0.0;
	double aoa		 = 0.0;
	auto   vertSpeed = 0.0;
	double bank		 = 0.0;
	double pitch	 = 0.0;
	double dynPress  = 0.0;
	

	isAeroDataActive_.fire(IsPowered());

	avionicsModeSignal_.fire(switchAvionMode_.is_on());

	if (isAeroDataActive_.current()) {
		gforce		= bco::GetVesselGs(vessel);
		trim		= vessel.GetControlSurfaceLevel(AIRCTRL_ELEVATORTRIM);
		aoa			= vessel.GetAOA();
		vertSpeed	= bco::GetVerticalSpeedFPM(&vessel);
		pitch		= vessel.GetPitch();
		bank		= vessel.GetBank();
		dynPress	= vessel.GetDynPressure();
	}

	// Vertical speed:
	double isPos = (vertSpeed >= 0) ? 1 : -1;
	auto absSpd = abs(vertSpeed);
	if (absSpd > 6000) absSpd = 6000;
	double spRot = (1 - pow((6000 - absSpd) / 6000, 2)) / 2;
	
	// vsi
	vsiHand_.set_state(0.5 + (isPos * spRot));
	vsiActiveFlag_.set_state(IsPowered());

	// attitude
	attitudeDisplay_.SetAngle(bank);
	attitudeDisplay_.SetTransform(0.0, (-0.100093 * pitch));
	attitudeFlag_.set_state(IsPowered());

	// accel
	accelHand_.set_state((gforce + 2) / 6);

	// trim
	trimHand_.set_state((trim + 1) / 2);

	// aoa
	// ** AOA **
	// AOA gauge works from -5 to 20 degrees AOA (-.0873 to .3491)
	// AOA guage has a throw of 75 degrees (1.3090).
	// Guage ratio is 3 AOA -> guage position.  The gauge sits at -5 deg
	// which must be accounted for.
	auto aoaR = 0.0;

	// Only worry about AOA if in the atmosphere.
	if (dynPress > 200)
	{
		aoaR = aoa;
		if (aoaR < -0.0873) aoaR = -0.0873;
		if (aoaR > 0.3491) aoaR = 0.3491;

		aoaR = aoaR * 3; // Translate to guage angle.
	}

	aoaHand_.set_state((aoaR + 0.2619) / 1.136);
}

// manage_state
bool Avionics::handle_load_state(bco::vessel& vessel, const std::string& line) {
	std::istringstream in(line);
	in >> setCourseSignal_ >> setHeadingSignal_ >> switchAvionPower_ >> switchAvionMode_ >> switchNavMode_;
	return true;
}

std::string Avionics::handle_save_state(bco::vessel& vessel) {
	std::ostringstream os;
	os << setCourseSignal_ << " " << setHeadingSignal_ << " " << switchAvionPower_ << " " << switchAvionMode_ << " " << switchNavMode_;
	return os.str();
}

void Avionics::SetCourse(double s)
{
	setCourseSignal_.update(s * RAD);
	UpdateSetCourse(0.0);	 // force the signal to fire.
}

void Avionics::UpdateSetCourse(double i)
{
	auto inc = setCourseSignal_.current() + i;
	if (inc > PI2) inc -= PI2;
	if (inc < 0) inc += PI2;
	setCourseSignal_.fire(inc);

//	sprintf(oapiDebugString(), "Set Course: %+4.2f", setCourseSignal_.current());
}

void Avionics::SetHeading(double s)
{
	setHeadingSignal_.update(s * RAD);
	UpdateSetHeading(0.0);	 // force the signal to fire.
}

void Avionics::UpdateSetHeading(double i)
{
	auto inc = setHeadingSignal_.current() + i;
	if (inc > PI2) inc -= PI2;
	if (inc < 0) inc += PI2;
	setHeadingSignal_.fire(inc);

	//	sprintf(oapiDebugString(), "Set Course: %+4.2f", setCourseSignal_.current());
}
