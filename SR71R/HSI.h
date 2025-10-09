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

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"
#include "..\bc_orbiter\Tools.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "SR71WrapGauge.h"

#include "IAvionics.h"
#include "IPowerProvider.h"

namespace bco = bc_orbiter;

class HSI
{
public:
    HSI(bco::Vessel& vessel);
    ~HSI() = default;

    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power, IAvionics& avionics);
    void UpdateVCUI(bco::Vessel& vessel, MESHHANDLE mesh);   // Called to update the VC UI when the VC is active.
    void UpdateMainPanelUI(MESHHANDLE mesh);    // Called to update the 2D panel UI when the panel is active.

    void LoadState(const std::string& line);
    std::string GetState() const;

private:

    const double RollOffset = 0.1084;		// flat_roll offset.

    bco::AnimatedValue<bco::StateUpdateWrap>    animCourseError_;

    bco::AnimatedValue<bco::StateUpdateWrap>    animCourseOnes_{ 0.0 };
    bco::AnimatedValue<bco::StateUpdateWrap>    animCourseTens_{ 0.0 };
    bco::AnimatedValue<bco::StateUpdateWrap>    animCourseHunds_{ 0.0 };

    bco::AnimatedValue<bco::StateUpdateWrap>    animMilesOnes_{ 0.0 };
    bco::AnimatedValue<bco::StateUpdateWrap>    animMilesTens_{ 0.0 };
    bco::AnimatedValue<bco::StateUpdateWrap>    animMilesHunds_{ 0.0 };

    bool isComActive_{ false };

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

    SR71::WrapGauge bearing_{
        { bm::vc::HSIBearingArrow_id },
        bm::vc::HSIBearingArrow_loc, bm::vc::HSIAxis_loc,
        bm::pnl::pnlHSIBearingArrow_id,
        bm::pnl::pnlHSIBearingArrow_vrt,
        bearingValue_,
        SR71R::MainPanel_ID
    };


    //bco::on_off_display		hsiOffFlag_{
    //   bm::vc::HSIOffFlag_id,
    //      bm::vc::HSIOffFlag_vrt,
    //      bm::pnl::pnlHSIOffFlag_id,
    //      bm::pnl::pnlHSIOffFlag_vrt,
    //      0.0244
    //};

    //bco::on_off_display		hsiExoFlag_{
    //   bm::vc::HSIExoFlag_id,
    //      bm::vc::HSIExoFlag_vrt,
    //      bm::pnl::pnlHSIExoFlag_id,
    //      bm::pnl::pnlHSIExoFlag_vrt,
    //      0.0244
    //};

    //bco::on_off_display		comStatusFlag_{
    //   bm::vc::COMStatusPanel_id,
    //      bm::vc::COMStatusPanel_vrt,
    //      bm::pnl::pnlCOMStatusPanel_id,
    //      bm::pnl::pnlCOMStatusPanel_vrt,
    //      0.0244
    //};
};

inline HSI::HSI(bco::Vessel& vessel)
{
    vessel.RegisterUIControl(roseCompass_);
    vessel.RegisterUIControl(headingBug_);
    vessel.RegisterUIControl(course_);
    vessel.RegisterUIControl(bearing_);
}

inline void HSI::UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power, IAvionics& avionics) 
{
    double      yaw = 0.0;
    double      rotHdg = 0.0;
    double      rotCrs = 0.0;
    NAVHANDLE   navHandle = nullptr;
    double      bearing = 0.0;		// from CalcNavMetrics
    double      glideSlope = 0.0;		// from CalcNavMetrics
    double      navError = 0.0;		// from CalcNavMetrics
    double      milesBeacon = 0.0;		// from CalcNavMetrics
    
    isComActive_ = false;

    if (avionics.IsAeroActive()) {
        yaw = vessel.GetYaw();
        rotHdg = yaw - avionics.GetSetHeading();
        rotCrs = yaw - avionics.GetSetCourse();

        navHandle = vessel.GetNavSource(avionics.GetNavSource());

        if (navHandle != nullptr) {
            isComActive_ = CalcNavMetrics(vessel, navHandle, avionics.GetSetCourse(), bearing, glideSlope, navError, milesBeacon);
        }
    }

    roseValue_      = yaw / PI2;
    headingValue_   = rotHdg / PI2;
    courseValue_    = rotCrs / PI2;
    bearingValue_   = bearing / PI2;

    animCourseError_.Update(simdt, navError);

    // Set course barrels
    auto deg = avionics.GetSetCourse();
    bco::TensParts parts;
    bco::GetDigits(deg * DEG, parts);

    animCourseOnes_.Update(simdt, parts.Tens / 10.0);
    animCourseTens_.Update(simdt, parts.Hundreds / 10.0);
    animCourseHunds_.Update(simdt, parts.Thousands / 10.0);

    // TODO Handle glice slope animation
    // signalGlideScope_.fire(glideSlope);

    //hsiCourseError_.SetAngle(-rotCrs);
    //hsiCourseError_.SetTransform(navError, 0.0);

    //comStatusFlag_.set_state(comStatus);

    // Miles barrels
    bco::GetDigits(milesBeacon, parts);
    animMilesOnes_.Update(simdt, parts.Tens);
    animMilesTens_.Update(simdt, parts.Hundreds);
    animMilesHunds_.Update(simdt, parts.Thousands);

    //hsiOffFlag_.set_state(avionics_.IsAeroActive());
    //hsiExoFlag_.set_state(
    //    !avionics_.IsAeroActive()
    //    ? true
    //    : avionics_.IsAeroAtmoMode());
}

inline void HSI::UpdateVCUI(bco::Vessel& vessel, MESHHANDLE mesh)
{
    bco::TranslateUVQuad(mesh, bm::vc::vcCrsOnes_id, bm::vc::vcCrsOnes_vrt, 0.0, animCourseOnes_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::vc::vcCrsTens_id, bm::vc::vcCrsTens_vrt, 0.0, animCourseTens_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::vc::vcCrsHunds_id, bm::vc::vcCrsHunds_vrt, 0.0, animCourseHunds_.GetCurrent() * RollOffset);

    bco::TranslateUVQuad(mesh, bm::vc::vcMilesOnes_id, bm::vc::vcMilesHunds_vrt, 0.0, animMilesOnes_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::vc::vcMilesTens_id, bm::vc::vcMilesTens_vrt, 0.0, animMilesTens_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::vc::vcMilesHunds_id, bm::vc::vcMilesHunds_vrt, 0.0, animMilesHunds_.GetCurrent() * RollOffset);
}

inline void HSI::UpdateMainPanelUI(MESHHANDLE mesh)
{
    bco::TranslateUVQuad(mesh, bm::pnl::pnlHSICRSOnes_id,    bm::pnl::pnlHSICRSOnes_vrt,    0.0, animCourseOnes_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::pnl::pnlHSICRSTens_id,    bm::pnl::pnlHSICRSTens_vrt,    0.0, animCourseTens_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::pnl::pnlHSICRSOnes_id,    bm::pnl::pnlHSICRSOnes_vrt,    0.0, animCourseHunds_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::pnl::pnlHSIMilesOnes_id,  bm::pnl::pnlHSIMilesOnes_vrt,   0.0, animMilesOnes_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::pnl::pnlHSIMilesTens_id,  bm::pnl::pnlHSIMilesTens_vrt,  0.0, animMilesTens_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::pnl::pnlHSIMilesHunds_id, bm::pnl::pnlHSIMilesHunds_vrt, 0.0, animMilesHunds_.GetCurrent() * RollOffset);
}

inline void HSI::LoadState(const std::string& line){}
inline std::string HSI::GetState() const {}

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
