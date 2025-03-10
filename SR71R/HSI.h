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

#include "../bc_orbiter/control.h"
#include "../bc_orbiter/signals.h"
#include "../bc_orbiter/vessel.h"
#include "../bc_orbiter/rotary_display.h"
#include "../bc_orbiter/transform_display.h"
#include "../bc_orbiter/flat_roll.h"

#include "Avionics.h"

namespace bco = bc_orbiter;

class HSI :
	public bco::vessel_component,
	public bco::post_step {

public:

	HSI(bco::vessel& vessel, Avionics& avionics) : 
		avionics_(avionics) 
	{
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

	// post_step
	void handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd) override {
		double		yaw			= 0.0;
		double		rotHdg		= 0.0;
		double		rotCrs		= 0.0;
		NAVHANDLE	navHandle	= nullptr;
		double		bearing		= 0.0;		// from CalcNavMetrics
		double		glideSlope	= 0.0;		// from CalcNavMetrics
		double		navError	= 0.0;		// from CalcNavMetrics
		double		milesBeacon = 0.0;		// from CalcNavMetrics
		bool		comStatus	= false;	// from CalcNavMetrics

		if (avionics_.IsAeroActive()) {
			yaw = vessel.GetYaw();
			rotHdg = yaw - slotSetHeading_.value();
			rotCrs = yaw - slotSetCourse_.value();

			navHandle = vessel.GetNavSource(slotNavMode_.value() ? 0 : 1);

			if (navHandle != nullptr) {
				comStatus = CalcNavMetrics(vessel, navHandle, bearing, glideSlope, navError, milesBeacon);
			}
		}

		// Set course barrels
		auto deg = slotSetCourse_.value();
		bco::TensParts parts;
		bco::GetDigits(deg * DEG, parts);

		// sprintf(oapiDebugString(), "CRS %+4f %+4f %+4f %+4f", deg, parts.Thousands, parts.Hundreds, parts.Tens );

		CRSOnes_.set_position(parts.Tens / 10);
		CRSTens_.set_position(parts.Hundreds / 10);
		CRSHunds_.set_position(parts.Thousands / 10);

		signalGlideScope_.fire(glideSlope);
		
		hsiRoseCompass_	.set_state(-yaw/PI2);
		hsiHeadingBug_	.set_state(-rotHdg/PI2);
		hsiCourse_		.set_state(-rotCrs/PI2);
		
		hsiCourseError_.SetAngle(-rotCrs);
		hsiCourseError_.SetTransform(navError, 0.0);

		comStatusFlag_.set_state(comStatus);

		// Miles barrels
		bco::GetDigits(milesBeacon, parts);
		MilesOnes_.set_position(parts.Tens);
		MilesTens_.set_position(parts.Hundreds);
		MilesHunds_.set_position(parts.Thousands);

		hsiOffFlag_.set_state(avionics_.IsAeroActive());
		hsiExoFlag_.set_state(
			!avionics_.IsAeroActive()
			? true
			: avionics_.IsAeroAtmoMode());
	}

	bco::signal<double>&	GlideScopeSignal()	{ return signalGlideScope_; }

	bco::slot<double>&		SetCourseSlot()		{ return slotSetCourse_; }
	bco::slot<double>&		SetHeadingSlot()	{ return slotSetHeading_; }

	bco::slot<bool>&		NavModeSignal()		{ return slotNavMode_; }

private:

	Avionics&				avionics_;
	bco::signal<double>		signalGlideScope_;

	bco::slot<double>		slotSetCourse_;
	bco::slot<double>		slotSetHeading_;
	bco::slot<bool>			slotNavMode_;

	bool CalcNavMetrics(bco::vessel& vessel, NAVHANDLE handle, double& bearing, double& glideSlope, double& navError, double& milesBeacon) {
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

	bco::rotary_display_wrap	hsiRoseCompass_{
		{ bm::vc::RoseCompass_id },
			bm::vc::RoseCompass_loc, bm::vc::HSIAxis_loc,
			bm::pnl::pnlRoseCompass_id,
			bm::pnl::pnlRoseCompass_vrt,
			(360 * RAD),	// Clockwise
			1.0
	};

	bco::rotary_display_wrap	hsiHeadingBug_{
		{ bm::vc::HSICompassHeading_id },
			bm::vc::HSICompassHeading_loc, bm::vc::HSIAxis_loc,
			bm::pnl::pnlHSICompassHeading_id,
			bm::pnl::pnlHSICompassHeading_vrt,
			(360 * RAD),	// Clockwise
			1.0
	};

	bco::rotary_display_wrap	hsiCourse_{
		{ bm::vc::HSICourse_id },
			bm::vc::HSICourse_loc, bm::vc::HSIAxis_loc,
			bm::pnl::pnlHSICourse_id,
			bm::pnl::pnlHSICourse_vrt,
			(360 * RAD),
			1.0
	};

	bco::rotary_display_wrap	hsiBearing_{
		{ bm::vc::HSIBearingArrow_id },
			bm::vc::HSIBearingArrow_loc, bm::vc::HSIAxis_loc,
			bm::pnl::pnlHSIBearingArrow_id,
			bm::pnl::pnlHSIBearingArrow_vrt,
			(360 * RAD),
			1.0
	};

	bco::transform_display	hsiCourseError_{
		bm::vc::HSICourseNeedle_id,
			bm::vc::HSICourseNeedle_vrt,
			bm::pnl::pnlHSICourseNeedle_id,
			bm::pnl::pnlHSICourseNeedle_vrt
	};

	bco::flat_roll			CRSOnes_{
		bm::vc::vcCrsOnes_id,
			bm::vc::vcCrsOnes_vrt,
			bm::pnl::pnlHSICRSOnes_id,
			bm::pnl::pnlHSICRSOnes_vrt,
			0.1084};

	bco::flat_roll			CRSTens_{
		bm::vc::vcCrsTens_id,
			bm::vc::vcCrsTens_vrt,
			bm::pnl::pnlHSICRSTens_id,
			bm::pnl::pnlHSICRSTens_vrt,
			0.1084};

	bco::flat_roll			CRSHunds_{
		bm::vc::vcCrsHunds_id,
			bm::vc::vcCrsHunds_vrt,
			bm::pnl::pnlHSICRSHunds_id,
			bm::pnl::pnlHSICRSHunds_vrt,
			0.1084};

	bco::flat_roll			MilesOnes_{
		bm::vc::vcMilesOnes_id,
			bm::vc::vcMilesOnes_vrt,
			bm::pnl::pnlHSIMilesOnes_id,
			bm::pnl::pnlHSIMilesOnes_vrt,
			0.1084};

	bco::flat_roll			MilesTens_{
		bm::vc::vcMilesTens_id,
			bm::vc::vcMilesTens_vrt,
			bm::pnl::pnlHSIMilesTens_id,
			bm::pnl::pnlHSIMilesTens_vrt,
			0.1084};

	bco::flat_roll			MilesHunds_{
		bm::vc::vcMilesHunds_id,
			bm::vc::vcMilesHunds_vrt,
			bm::pnl::pnlHSIMilesHunds_id,
			bm::pnl::pnlHSIMilesHunds_vrt,
			0.1084};

	bco::on_off_display		hsiOffFlag_{
		bm::vc::HSIOffFlag_id,
			bm::vc::HSIOffFlag_vrt,
			bm::pnl::pnlHSIOffFlag_id,
			bm::pnl::pnlHSIOffFlag_vrt,
			0.0244
	};

	bco::on_off_display		hsiExoFlag_{
		bm::vc::HSIExoFlag_id,
			bm::vc::HSIExoFlag_vrt,
			bm::pnl::pnlHSIExoFlag_id,
			bm::pnl::pnlHSIExoFlag_vrt,
			0.0244
	};

	bco::on_off_display		comStatusFlag_{
		bm::vc::COMStatusPanel_id,
			bm::vc::COMStatusPanel_vrt,
			bm::pnl::pnlCOMStatusPanel_id,
			bm::pnl::pnlCOMStatusPanel_vrt,
			0.0244
	};
};