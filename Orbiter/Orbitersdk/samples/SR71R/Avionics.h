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

#pragma

#include "bc_orbiter/control.h"
#include "bc_orbiter/signals.h"
#include "bc_orbiter/on_off_input.h"
#include "bc_orbiter/on_off_display.h"
#include "bc_orbiter/simple_event.h"
#include "bc_orbiter/rotary_display.h"
#include "bc_orbiter/transform_display.h"

#include "SR71r_mesh.h"
#include "SR71r_common.h"

namespace bco = bc_orbiter;

class Avionics :
	  public bco::vessel_component
	, public bco::post_step
	, public bco::manage_state
	, public bco::power_consumer
{

public:
	enum AvionMode { AvionAtmo, AvionExo };

	Avionics(bco::power_provider& pwr, bco::vessel& vessel);
	~Avionics() {}

	// post_step
	void handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd) override;

	// manage_state
	bool handle_load_state(bco::vessel& vessel, const std::string& line) override;
	std::string handle_save_state(bco::vessel& vessel) override;

	// power_consumer
	double amp_draw() const { return IsPowered() ? 6.0 : 0.0; }

	void SetCourse(double s);
	void SetHeading(double s);

	// Slots:
	bco::signal<bool>&		IsAeroActiveSignal()	{ return isAeroDataActive_; }		// Is aero available (switch is on, power is adequate)
	bco::signal<bool>&		IsAeroModeAtmoSignal()	{ return avionicsModeSignal_; }

	// Signals:
	bco::signal<double>&	SetCourseSignal()		{ return setCourseSignal_; }
	bco::signal<double>&	SetHeadingSignal()		{ return setHeadingSignal_; }

	bco::signal<double>&	GForceSignal()			{ return gforceSignal_; }
	bco::signal<double>&	TrimSignal()			{ return trimSignal_; }
	bco::signal<double>&	AOASignal()				{ return aoaSignal_; }

private:
	bco::power_provider&	power_;

	bool IsPowered() const { return switchAvionPower_.is_on() && (power_.volts_available() > 24.0); }

	// Signals:
	bco::signal<bool>		avionicsModeSignal_;
	bco::signal<bool>		isAeroDataActive_;
	bco::signal<double>		setCourseSignal_;
	bco::signal<double>		setHeadingSignal_;

	bco::signal<double>		gforceSignal_;
	bco::signal<double>		trimSignal_;
	bco::signal<double>		aoaSignal_;

	void UpdateSetCourse(double i);
	void UpdateSetHeading(double i);

	bco::on_off_input		switchAvionPower_{		// Main avionics power
		{ bm::vc::SwAvionics_id },
			bm::vc::SwAvionics_location, bm::vc::PowerTopRightAxis_location,
			toggleOnOff,
			bm::pnl::pnlPwrAvion_id,
			bm::pnl::pnlPwrAvion_verts,
			bm::pnl::pnlPwrAvion_RC
	};

	bco::on_off_input		switchAvionMode_{		// Atmosphere=On, External=Off
		{ bm::vc::vcAvionMode_id },
			bm::vc::vcAvionMode_location, bm::vc::vcAttitudeSwitchesAxis_location,
			toggleOnOff,
			bm::pnl::pnlAvionMode_id,
			bm::pnl::pnlAvionMode_verts,
			bm::pnl::pnlAvionMode_RC
	};

	bco::on_off_input		switchNavMode_{		// Nav mode 1 2
		{ bm::vc::vcNavMode_id },
			bm::vc::vcNavMode_location, bm::vc::vcAttitudeSwitchesAxis_location,
			toggleOnOff,
			bm::pnl::pnlNavMode_id,
			bm::pnl::pnlNavMode_verts,
			bm::pnl::pnlNavMode_RC
	};

	bco::simple_event<>		dialSetCourseIncrement_{
		bm::vc::CourseKnob_location,
			0.01,
			bm::pnl::pnlSetCourseInc_RC
	};

	bco::simple_event<>		dialSetCourseDecrement_{
		bm::vc::CourseKnob_location,
			0.01,
			bm::pnl::pnlSetCourseDec_RC
	};

	bco::simple_event<>		dialSetHeadingIncrement_{
		bm::vc::HeadingKnob_location,
			0.01,
			bm::pnl::pnlSetHeadingInc_RC
	};

	bco::simple_event<>		dialSetHeadingDecrement_{

		bm::vc::HeadingKnob_location,
			0.01,
			bm::pnl::pnlSetHeadingDec_RC
	};

	// ***   VSI  *** //
	bco::rotary_display_target		vsiHand_ {	  { bm::vc::gaVSINeedle_id }
												, bm::vc::gaVSINeedle_location, bm::vc::VSIAxis_location
												, bm::pnl::pnlVSINeedle_id
												, bm::pnl::pnlVSINeedle_verts
												, (340 * RAD)
												, 2.0
												, [](double d) {return (d); }
											};

	bco::on_off_display		vsiActiveFlag_{
		bm::vc::VSIOffFlag_id,
			bm::vc::VSIOffFlag_verts,
			bm::pnl::pnlVSIOffFlag_id,
			bm::pnl::pnlVSIOffFlag_verts,
			0.0244
	};

	// ***  ATTITUDE  *** //
	bco::transform_display	attitudeDisplay_{
		bm::vc::AttitudeIndicator_id,
			bm::vc::AttitudeIndicator_verts,
			bm::pnl::pnlAttitudeIndicator_id,
			bm::pnl::pnlAttitudeIndicator_verts
	};
	
	bco::on_off_display		attitudeFlag_{
		bm::vc::AttitudeFlagOff_id,
			bm::vc::AttitudeFlagOff_verts,
			bm::pnl::pnlAttitudeFlagOff_id,
			bm::pnl::pnlAttitudeFlagOff_verts,
			0.0244
	};

	// ***  AOA  TRIM  GFORCE  ***  //
	bco::rotary_display_target	aoaHand_ {	  { bm::vc::AOANeedle_id }
											, bm::vc::AOANeedle_location, bm::vc::AOAAxis_location
											, bm::pnl::pnlAOANeedle_id
											, bm::pnl::pnlAOANeedle_verts
											, (75 * RAD)
											, 2.0
											, [](double d) {return (d); }
										};

	bco::rotary_display_target	trimHand_ {	  { bm::vc::TrimNeedle_id }
											, bm::vc::TrimNeedle_location, bm::vc::TrimAxis_location
											, bm::pnl::pnlTrimNeedle_id
											, bm::pnl::pnlTrimNeedle_verts
											, (180 * RAD)
											, 2.0
											, [](double d) {return (d); }
										};

	bco::rotary_display_target	accelHand_ {  { bm::vc::AccelNeedle_id }
											, bm::vc::AccelNeedle_location, bm::vc::AccelAxis_location
											, bm::pnl::pnlAccelNeedle_id
											, bm::pnl::pnlAccelNeedle_verts
											, (295 * RAD)
											, 2.0
											, [](double d) {return (d); }
										};

};
