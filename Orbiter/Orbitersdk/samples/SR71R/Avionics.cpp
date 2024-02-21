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

Avionics::Avionics(bco::Vessel& Vessel, bco::PowerProvider& pwr) :
	power_(pwr),
	setHeadingSlot_([&](double v) { setHeadingSignal_.Fire(v); })
{
	power_.AttachConsumer(this);

	Vessel.AddControl(&switchAvionMode_);
	Vessel.AddControl(&switchAvionPower_);
	Vessel.AddControl(&switchNavMode_);

	Vessel.AddControl(&dialSetCourseDecrement_);
	Vessel.AddControl(&dialSetCourseIncrement_);
	Vessel.AddControl(&dialSetHeadingDecrement_);
	Vessel.AddControl(&dialSetHeadingIncrement_);

	dialSetCourseDecrement_.Attach([&]() { UpdateSetCourse(-0.0175); });
	dialSetCourseIncrement_.Attach([&]() { UpdateSetCourse(0.0175); });
	dialSetHeadingDecrement_.Attach([&]() { UpdateSetHeading(-0.0175); });
	dialSetHeadingIncrement_.Attach([&]() { UpdateSetHeading(0.0175); });

	Vessel.AddControl(&vsiHand_);
	Vessel.AddControl(&vsiActiveFlag_);
	
	Vessel.AddControl(&attitudeDisplay_);
	Vessel.AddControl(&attitudeFlag_);

	Vessel.AddControl(&aoaHand_);
	Vessel.AddControl(&trimHand_);
	Vessel.AddControl(&accelHand_);
}


// post_step
void Avionics::HandlePostStep(bco::Vessel& Vessel, double simt, double simdt, double mjd) {
	double gforce	 = 0.0;
	double trim		 = 0.0;
	double aoa		 = 0.0;
	auto   vertSpeed = 0.0;
	double bank		 = 0.0;
	double pitch	 = 0.0;
	double dynPress  = 0.0;
	

	isAeroDataActive_ = IsPowered();
	isAtmoMode_ = switchAvionMode_.IsOn();

	if (isAeroDataActive_) {
		gforce		= bco::GetVesselGs(Vessel);
		trim		= Vessel.GetControlSurfaceLevel(AIRCTRL_ELEVATORTRIM);
		aoa			= Vessel.GetAOA();
		vertSpeed	= bco::GetVerticalSpeedFPM(&Vessel);
		pitch		= Vessel.AvGetPitch();
		bank		= Vessel.AvGetBank();
		dynPress	= Vessel.GetDynPressure();
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
bool Avionics::HandleLoadState(bco::Vessel& Vessel, const std::string& line) {
	//sscanf_s(configLine + 8, "%i%i%i%i%i", &power, &heading, &course, &navSelect, &navMode);
	std::istringstream in(line);
	in >> switchAvionPower_ >> setHeadingSignal_ >> setCourseSignal_ >> switchNavMode_ >> switchAvionMode_;
	return true;
}

std::string Avionics::HandleSaveState(bco::Vessel& Vessel) {
	std::ostringstream os;
	os << switchAvionPower_  << " " << setHeadingSignal_ << " " << setCourseSignal_ << " " << switchNavMode_ << " " << switchAvionMode_;
	return os.str();
}

void Avionics::SetCourse(double s)
{
	setCourseSignal_.Update(s * RAD);
	UpdateSetCourse(0.0);	 // force the signal to fire.
}

void Avionics::UpdateSetCourse(double i)
{
	auto inc = setCourseSignal_.Current() + i;
	if (inc > PI2) inc -= PI2;
	if (inc < 0) inc += PI2;
	setCourseSignal_.Fire(inc);

//	sprintf(oapiDebugString(), "Set Course: %+4.2f", setCourseSignal_.current());
}

void Avionics::SetHeading(double s)
{
	setHeadingSignal_.Update(s * RAD);
	UpdateSetHeading(0.0);	 // force the signal to fire.
}

void Avionics::UpdateSetHeading(double i)
{
	auto inc = setHeadingSignal_.Current() + i;
	if (inc > PI2) inc -= PI2;
	if (inc < 0) inc += PI2;
	setHeadingSignal_.Fire(inc);

	//	sprintf(oapiDebugString(), "Set Course: %+4.2f", setCourseSignal_.current());
}
