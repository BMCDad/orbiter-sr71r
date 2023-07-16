//	HSI - SR-71r Orbiter Addon
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
#include "bc_orbiter/BaseVessel.h"
#include "bc_orbiter/rotary_display.h"
#include "bc_orbiter/transform_display.h"
#include "bc_orbiter/flat_roll.h"

#include "AvionBase.h"

namespace bco = bc_orbiter;

class HSI :
	public AvionBase,
	public bco::post_step {

public:

	HSI(bco::BaseVessel& vessel) {
		vessel.AddControl(&hsiBearing_);
		vessel.AddControl(&hsiCourseError_);
		vessel.AddControl(&hsiCourse_);
		vessel.AddControl(&hsiHeadingBug_);
		vessel.AddControl(&hsiRoseCompass_);

		vessel.AddControl(&CRSOnes_);
		vessel.AddControl(&CRSTens_);
		vessel.AddControl(&CRSHunds_);

		vessel.AddControl(&MilesOnes_);
		vessel.AddControl(&MilesTens_);
		vessel.AddControl(&MilesHunds_);

		vessel.AddControl(&hsiOffFlag_);
		vessel.AddControl(&hsiExoFlag_);

		vessel.AddControl(&comStatusFlag_);
	}

	~HSI() {}

	void OnEnabledChanged() override {
		hsiOffFlag_.set_state(EnabledSlot().value());
		OnAvionModeChanged();
	}

	void OnAvionModeChanged() override {
		hsiExoFlag_.set_state(!EnabledSlot().value() ? true : AvionicsModeSlot().value());
	}

	// post_step
	void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override {
		double		yaw			= 0.0;
		double		rotHdg		= 0.0;
		double		rotCrs		= 0.0;
		NAVHANDLE	navHandle	= nullptr;
		double		bearing		= 0.0;		// from CalcNavMetrics
		double		glideSlope	= 0.0;		// from CalcNavMetrics
		double		navError	= 0.0;		// from CalcNavMetrics
		double		milesBeacon = 0.0;		// from CalcNavMetrics
		bool		comStatus	= false;	// from CalcNavMetrics

		if (EnabledSlot().value()) {
			yaw = vessel.GetYaw();
			rotHdg = yaw - slotSetHeading_.value();
			rotCrs = yaw - slotSetCourse_.value();

			navHandle = vessel.GetNavSource(slotNavMode_.value() ? 0 : 1);

			if (navHandle != nullptr) {
				comStatus = CalcNavMetrics(vessel, navHandle, bearing, glideSlope, navError, milesBeacon);
			}
		}

		// Set course barrels
		auto deg = slotSetCourse_.value() * 57.2958;
		bco::TensParts parts;
		bco::GetDigits(deg, parts);

		// sprintf(oapiDebugString(), "CRS %+4f %+4f %+4f %+4f", deg, parts.Thousands, parts.Hundreds, parts.Tens );

		CRSOnes_.SlotTransform().notify(parts.Tens);
		CRSOnes_.SlotTransform().notify(parts.Hundreds);
		CRSOnes_.SlotTransform().notify(parts.Thousands);

		signalGlideScope_.fire(glideSlope);
		
		hsiRoseCompass_	.set_state(yaw);
		hsiHeadingBug_	.set_state(rotHdg);
		hsiCourse_		.set_state(rotCrs);
		
		hsiCourseError_.SlotAngle()		.notify(rotCrs);
		hsiCourseError_.SlotTransform()	.notify(navError);

		comStatusFlag_.set_state(comStatus);

		// Miles barrels
		bco::GetDigits(milesBeacon, parts);
		MilesOnes_.SlotTransform().notify(parts.Tens);
		MilesTens_.SlotTransform().notify(parts.Hundreds);
		MilesHunds_.SlotTransform().notify(parts.Thousands);

//		hsiOffFlag_.set_state(EnabledSlot().value());
//		hsiExoFlag_.set_state(EnabledSlot().value() && AvionicsModeSlot().value());
	}

	bco::signal<double>&	GlideScopeSignal()	{ return signalGlideScope_; }

	bco::slot<double>&		SetCourseSlot()		{ return slotSetCourse_; }
	bco::slot<double>&		SetHeadingSlot()	{ return slotSetHeading_; }

	bco::slot<bool>&		NavModeSignal()		{ return slotNavMode_; }

private:

	bco::signal<double>		signalGlideScope_;

	bco::slot<double>		slotSetCourse_;
	bco::slot<double>		slotSetHeading_;
	bco::slot<bool>			slotNavMode_;

	bool CalcNavMetrics(bco::BaseVessel& vessel, NAVHANDLE handle, double& bearing, double& glideSlope, double& navError, double& milesBeacon) {
		bool result = false;  //comstatus

		auto navType = oapiGetNavType(handle);
		if ((navType == TRANSMITTER_ILS) || (navType == TRANSMITTER_VOR) || (navType == TRANSMITTER_VTOL))
		{
			NAVDATA data;
			oapiGetNavData(handle, &data);

			// Get the location of the NAV beacon.
			// We get it in global coords, then convert
			// to local lon/lat/rad coords.
			VECTOR3 navPosition;
			auto navRef = vessel.GetSurfaceRef();
			oapiGetNavPos(handle, &navPosition);
			double navlng, navlat, rad;
			oapiGlobalToEqu(navRef, navPosition, &navlng, &navlat, &rad);

			// Now get the location of our vessel.
			double vlng, vlat, vrad, slope;
			double navDistance;
			OBJHANDLE hRef = vessel.GetEquPos(vlng, vlat, vrad);

			// Now calc the distance and bearing between the two.
			bco::Orthodome(vlng, vlat, navlng, navlat, navDistance, bearing);
			navDistance *= oapiGetSize(hRef);

			double courseDeviation = bearing - slotSetCourse_.value();
			if (courseDeviation < -PI) courseDeviation += PI2;
			else if (courseDeviation >= PI) courseDeviation -= PI2;
			if (courseDeviation < -PI05) courseDeviation = -PI - courseDeviation;
			else if (courseDeviation >= PI05) courseDeviation = PI - courseDeviation;

			const double Range = 0.14;
			const double Slide = 0.025;

			auto devB = min(Range, max(-Range, courseDeviation));
			navError = devB / Range * Slide;

			// Glide slope
			slope = atan2(vessel.GetAltitude(), navDistance);

			const double tgtslope = 3.0 * RAD;
			const double tgtvar = 0.6 * RAD;

			double dslope = slope - tgtslope;
			if (dslope > tgtvar) dslope = tgtvar;
			if (dslope < -tgtvar) dslope = -tgtvar;

			glideSlope = 0.01218 * (dslope / tgtvar);

			milesBeacon = navDistance / 1609.34; // convert to miles.
			if (milesBeacon > 999) milesBeacon = 999;

			result = true;
		}

		return result;
	}

	bco::rotary_display<bco::AnimationWrap>	hsiRoseCompass_{
		{ bm::vc::RoseCompass_id },
			bm::vc::RoseCompass_location, bm::vc::HSIAxis_location,
			bm::pnl::pnlRoseCompass_id,
			bm::pnl::pnlRoseCompass_verts,
			(360 * RAD),	// Clockwise
			1.0,
			[](double d) {return bco::AngleToState(-d); }	// Transform to anim range.
	};

	bco::rotary_display<bco::AnimationWrap>	hsiHeadingBug_{
		{ bm::vc::HSICompassHeading_id },
			bm::vc::HSICompassHeading_location, bm::vc::HSIAxis_location,
			bm::pnl::pnlHSICompassHeading_id,
			bm::pnl::pnlHSICompassHeading_verts,
			(360 * RAD),	// Clockwise
			1.0,
			[](double d) {return bco::AngleToState(-d); }	// Transform to anim range.
	};

	bco::rotary_display<bco::AnimationWrap> hsiCourse_{
		{ bm::vc::HSICourse_id },
			bm::vc::HSICourse_location, bm::vc::HSIAxis_location,
			bm::pnl::pnlHSICourse_id,
			bm::pnl::pnlHSICourse_verts,
			(360 * RAD),
			1.0,
			[](double d) {return bco::AngleToState(-d); }
	};

	bco::rotary_display<bco::AnimationWrap> hsiBearing_{
		{ bm::vc::HSIBearingArrow_id },
			bm::vc::HSIBearingArrow_location, bm::vc::HSIAxis_location,
			bm::pnl::pnlHSIBearingArrow_id,
			bm::pnl::pnlHSIBearingArrow_verts,
			(360 * RAD),
			1.0,
			[](double d) {return bco::AngleToState(-d); }
	};

	bco::transform_display	hsiCourseError_{
		bm::vc::HSICourseNeedle_id,
			bm::vc::HSICourseNeedle_verts,
			bm::pnl::pnlHSICourseNeedle_id,
			bm::pnl::pnlHSICourseNeedle_verts
	};

	bco::flat_roll			CRSOnes_{
		bm::vc::vcCrsOnes_id,
			bm::vc::vcCrsOnes_verts,
			bm::pnl::pnlHSICRSOnes_id,
			bm::pnl::pnlHSICRSOnes_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::flat_roll			CRSTens_{
		bm::vc::vcCrsTens_id,
			bm::vc::vcCrsTens_verts,
			bm::pnl::pnlHSICRSTens_id,
			bm::pnl::pnlHSICRSTens_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::flat_roll			CRSHunds_{
		bm::vc::vcCrsHunds_id,
			bm::vc::vcCrsHunds_verts,
			bm::pnl::pnlHSICRSHunds_id,
			bm::pnl::pnlHSICRSHunds_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::flat_roll			MilesOnes_{
		bm::vc::vcMilesOnes_id,
			bm::vc::vcMilesOnes_verts,
			bm::pnl::pnlHSIMilesOnes_id,
			bm::pnl::pnlHSIMilesOnes_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::flat_roll			MilesTens_{
		bm::vc::vcMilesTens_id,
			bm::vc::vcMilesTens_verts,
			bm::pnl::pnlHSIMilesTens_id,
			bm::pnl::pnlHSIMilesTens_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::flat_roll			MilesHunds_{
		bm::vc::vcMilesHunds_id,
			bm::vc::vcMilesHunds_verts,
			bm::pnl::pnlHSIMilesHunds_id,
			bm::pnl::pnlHSIMilesHunds_verts,
			0.1084,
			[](double v) {return floor(v) / 10; }
	};

	bco::on_off_display		hsiOffFlag_{
		bm::vc::HSIOffFlag_id,
			bm::vc::HSIOffFlag_verts,
			bm::pnl::pnlHSIOffFlag_id,
			bm::pnl::pnlHSIOffFlag_verts,
			0.0244
	};

	bco::on_off_display		hsiExoFlag_{
		bm::vc::HSIExoFlag_id,
			bm::vc::HSIExoFlag_verts,
			bm::pnl::pnlHSIExoFlag_id,
			bm::pnl::pnlHSIExoFlag_verts,
			0.0244
	};

	bco::on_off_display		comStatusFlag_{
		bm::vc::COMStatusPanel_id,
			bm::vc::COMStatusPanel_verts,
			bm::pnl::pnlCOMStatusPanel_id,
			bm::pnl::pnlCOMStatusPanel_verts,
			0.0244
	};
};