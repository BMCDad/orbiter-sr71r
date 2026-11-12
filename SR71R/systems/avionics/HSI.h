/*
HSI - SR-71r Orbiter Addon
Copyright(C) 2023  Blake Christensen

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.


HSI - Horizontal Situation Indicator
The HSI has the following moving parts:
    [Rose Compass]:  Direction the nose of the vessel is pointing.  This is the vessel Yaw value.
        * VC: vessel animation,  Panel: mesh rotation
    [Heading Bug]:  Compass heading set by the user.  Travels around with the rose compass.  The Heading is the direction the auto pilot will follow.
        * VC: vessel animation,  Panel: mesh rotation
    [Course]:  Compass heading set by the user.  Points to the desired course heading.
        * VC: vessel animation,  Panel: mesh rotation
    [Course Error]:  A needle aligned with the Course.  It translates left and right of course based on the current course alignment and selected course.
        * VC: UV transform,  Panel: UV transform
    [Bearing]:  Points to the selected course NAV if tuned in.  Bearing is used to calculate Course Error but is also an arrow if NAV tuned in.
        * VC: vessel animation,  Panel: mesh rotation
    [Course barrel digits]:  The user set course in three digit form.
        * VC/Panel:  TranslateUV
    [Glide Slope]:  A needle that moves up and down based on the current glide slope of the tuned in NAV aid.
        * VC/Panel:  TranslateUV
    [Miles barrel digits]:  Distance to NAV aid, if tuned in, in three digit form.
        * VC/Panel:  TranslateUV
    [Inactive flag]:  The HSI is electrically powered, if no power is present it will display the 'inactive' flag.
        * VC/Panel:  TranslateUV

The Course and Heading are set as part of the Avionics component.  The HSI itelf has no user input.
*/
#pragma once

#include <cmath>

#include "bc_orbiter/vessel.h"
#include "bc_orbiter/AnimatedValue.h"
#include "bc_orbiter/MeshTools.h"
#include "bc_orbiter/Tools.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "controls/SR71WrapGauge.h"
#include "controls/SR71Barrel.h"
#include "controls/SR71Transform.h"

#include "interfaces/IAvionics.h"
#include "interfaces/IPowerProvider.h"

namespace bco = bc_orbiter;

class HSI
{
public:
    HSI(bco::Vessel& vessel);
    ~HSI() = default;

    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power, IAvionics& avionics);

private:

    const double RollOffset = 0.1084;		// flat_roll offset.

    bco::AnimatedValue<bco::StateUpdateWrap>    animCourseError_ { 12.0 };  // Course error needle animation.

    SR71::Barrel courseBarrelOnes_  { bm::vc::vcCrsOnes_id,     bm::vc::vcCrsOnes_vrt,  bm::pnl::pnlHSICRSOnes_id,  bm::pnl::pnlHSICRSOnes_vrt,     SR71R::MainPanel_ID };
    SR71::Barrel courseBarrelTens_  { bm::vc::vcCrsTens_id,     bm::vc::vcCrsTens_vrt,  bm::pnl::pnlHSICRSTens_id,  bm::pnl::pnlHSICRSTens_vrt,     SR71R::MainPanel_ID };
    SR71::Barrel courseBarrelHunds_ { bm::vc::vcCrsHunds_id,    bm::vc::vcCrsHunds_vrt, bm::pnl::pnlHSICRSHunds_id, bm::pnl::pnlHSICRSHunds_vrt,    SR71R::MainPanel_ID };

    SR71::Barrel milesBarrelOnes_  { bm::vc::vcMilesOnes_id,     bm::vc::vcMilesOnes_vrt,  bm::pnl::pnlHSIMilesOnes_id,  bm::pnl::pnlHSIMilesOnes_vrt,     SR71R::MainPanel_ID };
    SR71::Barrel milesBarrelTens_  { bm::vc::vcMilesTens_id,     bm::vc::vcMilesTens_vrt,  bm::pnl::pnlHSIMilesTens_id,  bm::pnl::pnlHSIMilesTens_vrt,     SR71R::MainPanel_ID };
    SR71::Barrel milesBarrelHunds_ { bm::vc::vcMilesHunds_id,    bm::vc::vcMilesHunds_vrt, bm::pnl::pnlHSIMilesHunds_id, bm::pnl::pnlHSIMilesHunds_vrt,    SR71R::MainPanel_ID };

    bool isComActive_{ false };

    /// <summary>
    /// Calculate the NAV metrics based on the current vessel state and the selected NAV source.
    /// </summary>
    /// <param name="vessel">The current vessel.</param>
    /// <param name="handle">The NAV handle from GetNavSource.</param>
    /// <param name="setCourse">The course set on the HSI by the user.</param>
    /// <param name="bearing">The calculated bearing in radians.</param>
    /// <param name="glideSlope">The calculated glide slope.</param>
    /// <param name="navError">The calculated navigation error.</param>
    /// <param name="milesBeacon">The calculated miles to beacon.</param>
    /// <returns>True if the metrics were calculated successfully; otherwise, false.</returns>
    bool CalcNavMetrics(bco::Vessel& vessel, NAVHANDLE handle, double setCourse, double& bearing, double& glideSlope, double& navError, double& milesBeacon);

    double roseValue_{ 0.0 };
    double headingValue_{ 0.0 };
    double courseValue_{ 0.0 };
    double bearingValue_{ 0.0 };

    SR71::WrapGauge roseCompass_{
        { bm::vc::RoseCompass_id },
        bm::vc::RoseCompass_loc, bm::vc::HSIAxis_loc,
        bm::pnl::pnlRoseCompass_id,
        bm::pnl::pnlRoseCompass_vrt,
        roseValue_,
        SR71R::MainPanel_ID
    };

    SR71::WrapGauge headingBug_{
        { bm::vc::HSICompassHeading_id },
        bm::vc::HSICompassHeading_loc, bm::vc::HSIAxis_loc,
        bm::pnl::pnlHSICompassHeading_id,
        bm::pnl::pnlHSICompassHeading_vrt,
        headingValue_,
        SR71R::MainPanel_ID
    };

    SR71::WrapGauge course_{
        { bm::vc::HSICourse_id },
        bm::vc::HSICourse_loc, bm::vc::HSIAxis_loc,
        bm::pnl::pnlHSICourse_id,
        bm::pnl::pnlHSICourse_vrt,
        courseValue_,
        SR71R::MainPanel_ID
    };

    SR71::Transform courseErrorNeedle_{
         bm::vc::HSICourseNeedle_id,
         bm::vc::HSICourseNeedle_vrt,
         bm::pnl::pnlHSICourseNeedle_id,
         bm::pnl::pnlHSICourseNeedle_vrt,
         SR71R::MainPanel_ID
    };

    SR71::WrapGauge bearing_{
        { bm::vc::HSIBearingArrow_id },
        bm::vc::HSIBearingArrow_loc, bm::vc::HSIAxis_loc,
        bm::pnl::pnlHSIBearingArrow_id,
        bm::pnl::pnlHSIBearingArrow_vrt,
        bearingValue_,
        SR71R::MainPanel_ID
    };


    SR71::Light hsiOffFlag_{
        bm::vc::HSIOffFlag_id,
        bm::vc::HSIOffFlag_vrt,
        bm::pnl::pnlHSIOffFlag_id,
        bm::pnl::pnlHSIOffFlag_vrt,
        SR71R::MainPanel_ID
    };

    SR71::Light comStatusFlag_{
        bm::vc::COMStatusPanel_id,
        bm::vc::COMStatusPanel_vrt,
        bm::pnl::pnlCOMStatusPanel_id,
        bm::pnl::pnlCOMStatusPanel_vrt,
        SR71R::MainPanel_ID
    };
};

inline HSI::HSI(bco::Vessel& vessel)
{
    vessel.RegisterUIControl(roseCompass_);
    vessel.RegisterUIControl(headingBug_);
    vessel.RegisterUIControl(course_);
    vessel.RegisterUIControl(bearing_);
    vessel.RegisterUIControl(hsiOffFlag_);
    vessel.RegisterUIControl(comStatusFlag_);
    vessel.RegisterUIControl(courseErrorNeedle_);

    vessel.RegisterUIControl(courseBarrelOnes_);
    vessel.RegisterUIControl(courseBarrelTens_);
    vessel.RegisterUIControl(courseBarrelHunds_);
    
    vessel.RegisterUIControl(milesBarrelOnes_);
    vessel.RegisterUIControl(milesBarrelTens_);
    vessel.RegisterUIControl(milesBarrelHunds_);
}

inline void HSI::UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power, IAvionics& avionics) 
{
    double      vesselHeadingRadians    = vessel.GetYaw();          // From vessel, the direction the nose of the vessel is pointing.
    double      setCourseRadians        = avionics.GetSetCourse();  // From HSI Course knob, used to line up nav aids and calculate course error.
    double      setHeadingRadians       = avionics.GetSetHeading(); // From HSI Heading knob, the heading for the autopilot to follow.

    double      rotCourseRadians        = vesselHeadingRadians - setCourseRadians;; // Adjust course bug relative to the rose compass.
    double      rotHeadingRadians       = vesselHeadingRadians - setHeadingRadians; // Adjust heading bug relative to the rose compass.

    // Adjust the values to be in the range of 0.0 to 1.0 for the gauge animations.
    roseValue_      = -vesselHeadingRadians / PI2;
    headingValue_   = -rotHeadingRadians / PI2;
    courseValue_    = -rotCourseRadians / PI2;

    // Calculate the NAV metrics if the avionics is active and a NAV source is selected.
    double      bearing = 0.0;		// from CalcNavMetrics
    double      glideSlope = 0.0;		// from CalcNavMetrics
    double      navError = 0.0;		// from CalcNavMetrics
    double      milesBeacon = 0.0;		// from CalcNavMetrics
    
    isComActive_ = false;

    if (avionics.IsAeroActive()) {
        hsiOffFlag_.SetState(true);
        NAVHANDLE navHandle = vessel.GetNavSource(avionics.GetNavSource());

        if (navHandle != nullptr) {
            isComActive_ = CalcNavMetrics(vessel, navHandle, setCourseRadians, bearing, glideSlope, navError, milesBeacon);
        }
    }
    else {
        hsiOffFlag_.SetState(false);
    }

    comStatusFlag_.SetState(isComActive_);
    bearingValue_   = -bearing / PI2; // Adjust value to be in the range of 0.0 to 1.0 for the gauge animations.

    animCourseError_.Update(simdt, navError);
    courseErrorNeedle_.SetAngle(-rotCourseRadians); // Set the course error needle to the current course heading, adjusted for the rose compass.
    courseErrorNeedle_.SetTranslate(animCourseError_.GetCurrent(), 0.0);

    // Set course barrels * this is the raw value, not the one adjusted for the rose compass.  The barrels are set to the user selected course.
    bco::TensParts parts;
    bco::GetDigits(setCourseRadians * DEG, parts);

    courseBarrelOnes_.SetState(parts.Tens);
    courseBarrelTens_.SetState(parts.Hundreds);
    courseBarrelHunds_.SetState(parts.Thousands);

    // TODO Handle glide slope animation
    // signalGlideScope_.fire(glideSlope);

    // Miles barrels
    bco::GetDigits(milesBeacon, parts);
    milesBarrelOnes_.SetState(parts.Tens);
    milesBarrelTens_.SetState(parts.Hundreds);
    milesBarrelHunds_.SetState(parts.Thousands);
}

inline bool HSI::CalcNavMetrics(
    bco::Vessel& vessel, 
    NAVHANDLE handle,
    double setCourse,
    double& bearing, 
    double& glideSlope, 
    double& navError, 
    double& milesBeacon) 
{
    
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

        double courseDeviation = bearing - setCourse;
        if (courseDeviation < -PI) courseDeviation += PI2;
        else if (courseDeviation >= PI) courseDeviation -= PI2;
        if (courseDeviation < -PI05) courseDeviation = -PI - courseDeviation;
        else if (courseDeviation >= PI05) courseDeviation = PI - courseDeviation;

        const double Range = 0.14;
        const double Slide = 0.025;

        auto devB = min(Range, max(-Range, courseDeviation));
        navError = devB / Range * Slide;

        // Glide slope
        double altitudeAboveSource = vrad - rad;    // Vertical distance above the nav source, in meters.
        slope = atan2(altitudeAboveSource, navDistance);

        const double targetSlope = 3.0 * RAD;           // Hard coded target slope for glide slope, 3 degrees in radians.
        const double targetSlopeVariance = 0.6 * RAD;   // Hard coded target slope variance for glide slope, 0.6 degrees in radians.

        double slopeError = slope - targetSlope;
        
        // Clamp the slope error to the target slope variance, so that the glide slope needle doesn't go off the scale.
        if (slopeError > targetSlopeVariance) slopeError = targetSlopeVariance;
        if (slopeError < -targetSlopeVariance) slopeError = -targetSlopeVariance;

        glideSlope = 0.01218 * (slopeError / targetSlopeVariance); // Scale the slope error to the glide slope needle range, which is 0.01218 meters.

        milesBeacon = navDistance / 1609.34; // convert to miles.
        
        // Clamp miles beacon to 999, since the HSI only has three digits for miles.
        if (milesBeacon > 999) milesBeacon = 999;

        result = true;
    }

    return result;
}
