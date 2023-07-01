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

#include "bc_orbiter\control.h"
#include "bc_orbiter\signals.h"
#include "bc_orbiter\BaseVessel.h"

#include "AvionBase.h"

namespace bco = bc_orbiter;

class HSI :
	public AvionBase,
	public bco::post_step {

public:

	HSI() {}
	~HSI() {}

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

		signalCrsOnes_.fire(parts.Tens);
		signalCrsTens_.fire(parts.Hundreds);
		signalCrsHunds_.fire(parts.Thousands);

		signalYaw_.fire(yaw);
		signalSetHeading_.fire(rotHdg);
		signalSetCourse_.fire(rotCrs);
		signalBearing_.fire(bearing);
		signalGlideScope_.fire(glideSlope);
		signalNavError_.fire(navError);
		signalComStatus_.fire(comStatus);

		// Miles barrels
		bco::GetDigits(milesBeacon, parts);
		signalMilesOnes_.fire(parts.Tens);
		signalMilesTens_.fire(parts.Hundreds);
		signalMilesHunds_.fire(parts.Thousands);

		signalShowOffFlag_.fire(!EnabledSlot().value());
		signalShowExoFlag_.fire(EnabledSlot().value() && AvionicsModeSlot().value());
	}

	bco::signal<double>&	YawSignal()			{ return signalYaw_; }
	bco::signal<double>&	SetHeadingSignal()	{ return signalSetHeading_; }
	bco::signal<double>&	SetCourseSignal()	{ return signalSetCourse_; }
	bco::signal<double>&	BearingSignal()		{ return signalBearing_; }
	bco::signal<double>&	GlideScopeSignal()	{ return signalGlideScope_; }
	bco::signal<double>&	NavErrorSignal()	{ return signalNavError_; }
	bco::signal<bool>&		ComStatusSignal()	{ return signalComStatus_; }

	bco::slot<double>&		SetCourseSlot()		{ return slotSetCourse_; }
	bco::slot<double>&		SetHeadingSlot()	{ return slotSetHeading_; }

	bco::signal<double>&	CrsOnesSignal()		{ return signalCrsOnes_; }
	bco::signal<double>&	CrsTensSignal()		{ return signalCrsTens_; }
	bco::signal<double>&	CrsHundsSignal()	{ return signalCrsHunds_; }

	bco::signal<double>&	MilesOnesSignal()	{ return signalMilesOnes_; }
	bco::signal<double>&	MilesTensSignal()	{ return signalMilesTens_; }
	bco::signal<double>&	MilesHundsSignal()	{ return signalMilesHunds_; }

	bco::slot<bool>&		NavModeSignal()		{ return slotNavMode_; }

	bco::signal<bool>&		ShowOffFlagSignal() { return signalShowOffFlag_; }			// Off flag
	bco::signal<bool>&		ShowExoFlagSignal() { return signalShowExoFlag_; }

private:

	bco::signal<double>		signalYaw_;
	bco::signal<double>		signalSetHeading_;
	bco::signal<double>		signalSetCourse_;
	bco::signal<double>		signalBearing_;
	bco::signal<double>		signalGlideScope_;
	bco::signal<double>		signalNavError_;		// Course error transform, rotation matches course.
	bco::signal<bool>		signalComStatus_;
	bco::signal<bool>		signalShowOffFlag_;			// Off flag
	bco::signal<bool>		signalShowExoFlag_;

	bco::signal<double>		signalCrsOnes_;
	bco::signal<double>		signalCrsTens_;
	bco::signal<double>		signalCrsHunds_;

	bco::signal<double>		signalMilesOnes_;
	bco::signal<double>		signalMilesTens_;
	bco::signal<double>		signalMilesHunds_;

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
};