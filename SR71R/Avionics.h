/*
Avionics - SR-71r Orbiter Addon
Copyright(C) 2025  Blake Christensen

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

AVIONICS a b c d e
a = 0/1 avionics power off/on.
b = heading in radians.
c = course in radians.
d = 0/1 nav select 
e = 0/1 atmo/exo mode

*/

#pragma once

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\MeshTools.h"
#include "..\bc_orbiter\IUIControl.h"

#include "SR71r_mesh.h"
#include "IAvionics.h"
#include "IPowerProvider.h"
#include "SR71Toggle.h"
#include "SR71MouseClick.h"
#include "SR71Gauge.h"
#include "SR71Light.h"
#include "SR71Transform.h"

namespace bco = bc_orbiter;

class Avionics : public IAvionics
{
public:
    Avionics(bco::Vessel& vessel);
    ~Avionics() = default;
    
    enum AvionMode { AvionAtmo, AvionExo };

    // IAvionics
    bool	IsAeroActive() const override { return isAeroDataActive_; }		// Is aero available (switch is on, power is adequate)
    bool	IsAeroAtmoMode() const override { return isAtmoMode_; }
    double GetSetHeading() const override { return setHeading_; }  // Radians
    double GetSetCourse() const override { return setCourse_; }  // Radians
    DWORD GetNavSource() const override { return switchNavMode_.IsOn() ? 0 : 1; }
    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power);

    void LoadState(const std::string& line);
    std::string GetState() const;

private:

    bool    isAeroDataActive_{ false };
    bool    isAtmoMode_{ false };

    double  setCourse_{ 0.0 };        // degrees
    double  setHeading_{ 0.0 };       // degrees

    double vsiRangeValue_{ 0.0 };  // VSI expressed as 0.0 to 1.0 for the gauge.
    double aoaRangeValue_{ 0.0 };  // AOA expressed as 0.0 to 1.0 for the gauge.
    double trimRangeValue_{ 0.5 }; // Trim expressed as 0.0 to 1.0 for the gauge.
    double accelRangeValue_{ 0.0 }; // GForce expressed as 0.0 to 1.0 for the gauge.    

    void UpdateSetCourse(double i);
    void UpdateSetHeading(double i);

    SR71::Toggle switchAvionPower_{
        bm::vc::SwAvionics_id,
        bm::vc::SwAvionics_loc, bm::vc::PowerTopRightAxis_loc,
        bm::pnlright::pnlPwrAvion_id,
        bm::pnlright::pnlPwrAvion_vrt,
        bm::pnlright::pnlPwrAvion_RC,
        SR71R::RightPanel_ID
    };

    SR71::Toggle switchAvionMode_{
        bm::vc::vcAvionMode_id,
        bm::vc::vcAvionMode_loc, bm::vc::navPanelAxis_loc,
        bm::pnl::pnlAvionMode_id,
        bm::pnl::pnlAvionMode_vrt,
        bm::pnl::pnlAvionMode_RC,
        SR71R::MainPanel_ID
    };

    SR71::Toggle switchNavMode_{
        bm::vc::vcNavMode_id,
        bm::vc::vcNavMode_loc, bm::vc::navPanelAxis_loc,
        bm::pnl::pnlNavMode_id,
        bm::pnl::pnlNavMode_vrt,
        bm::pnl::pnlNavMode_RC,
        SR71R::MainPanel_ID
    };

    SR71::MouseClick	dialSetCourseIncrement_{
        bm::vc::CourseKnobInc_loc,
        0.01,
        bm::pnl::pnlSetCourseInc_RC,
        SR71R::MainPanel_ID,
        [&]() { UpdateSetCourse(0.0175); }
    };

    SR71::MouseClick	dialSetCourseDecrement_{
        bm::vc::CourseKnobDec_loc,
        0.01,
        bm::pnl::pnlSetCourseDec_RC,
        SR71R::MainPanel_ID,
        [&]() { UpdateSetCourse(-0.0175); }
    };

    SR71::MouseClick	dialSetHeadingIncrement_{
        bm::vc::HeadingKnobInc_loc,
        0.01,
        bm::pnl::pnlSetHeadingInc_RC,
        SR71R::MainPanel_ID,
        [&]() { UpdateSetHeading(0.0175); }
    };

    SR71::MouseClick	dialSetHeadingDecrement_{
        bm::vc::HeadingKnobDec_loc,
        0.01,
        bm::pnl::pnlSetHeadingDec_RC,
        SR71R::MainPanel_ID,
        [&]() { UpdateSetHeading(-0.0175); }
    };

    SR71::Gauge vsiHand_{ 
        bm::vc::gaVSINeedle_id,
        bm::vc::gaVSINeedle_loc, bm::vc::VSIAxis_loc,
        bm::pnl::pnlVSINeedle_id,
        bm::pnl::pnlVSINeedle_vrt,
        (340 * RAD),
        vsiRangeValue_,
        SR71R::MainPanel_ID
    };

    SR71::Light vsiActiveFlag_{
        bm::vc::VSIOffFlag_id,
        bm::vc::VSIOffFlag_vrt,
        bm::pnl::pnlVSIOffFlag_id,
        bm::pnl::pnlVSIOffFlag_vrt,
        SR71R::MainPanel_ID
    };

    SR71::Transform attitudeDisplay_{
         bm::vc::AttitudeIndicator_id,
         bm::vc::AttitudeIndicator_vrt,
         bm::pnl::pnlAttitudeIndicator_id,
         bm::pnl::pnlAttitudeIndicator_vrt,
         SR71R::MainPanel_ID
    };

    SR71::Light attitudeFlag_{
        bm::vc::AttitudeFlagOff_id,
        bm::vc::AttitudeFlagOff_vrt,
        bm::pnl::pnlAttitudeFlagOff_id,
        bm::pnl::pnlAttitudeFlagOff_vrt,
        SR71R::MainPanel_ID
    };

    // ***  AOA  TRIM  GFORCE  ***  //

    SR71::Gauge  aoaHand_{
        bm::vc::AOANeedle_id,
        bm::vc::AOANeedle_loc, bm::vc::AOAAxis_loc,
        bm::pnl::pnlAOANeedle_id,
        bm::pnl::pnlAOANeedle_vrt,
        (75 * RAD), // max angle
        aoaRangeValue_, // value reference
        SR71R::MainPanel_ID
    };

    SR71::Gauge trimHand_{ 
        bm::vc::TrimNeedle_id,
        bm::vc::TrimNeedle_loc, bm::vc::TrimAxis_loc,
        bm::pnl::pnlTrimNeedle_id,
        bm::pnl::pnlTrimNeedle_vrt,
        (180 * RAD), // max angle
        trimRangeValue_, // value reference
        SR71R::MainPanel_ID
    };

    SR71::Gauge accelHand_{
        bm::vc::AccelNeedle_id,
        bm::vc::AccelNeedle_loc, bm::vc::AccelAxis_loc,
        bm::pnl::pnlAccelNeedle_id,
        bm::pnl::pnlAccelNeedle_vrt,
        (295 * RAD), // max angle
        accelRangeValue_,
        SR71R::MainPanel_ID
    };
};

inline Avionics::Avionics(bco::Vessel& vessel)
{ 
    vessel.RegisterUIControl(switchAvionPower_);
    vessel.RegisterUIControl(switchAvionMode_);
    vessel.RegisterUIControl(switchNavMode_);
    vessel.RegisterUIControl(dialSetCourseIncrement_);
    vessel.RegisterUIControl(dialSetCourseDecrement_);
    vessel.RegisterUIControl(dialSetHeadingIncrement_);
    vessel.RegisterUIControl(dialSetHeadingDecrement_);
    vessel.RegisterUIControl(vsiHand_);
    vessel.RegisterUIControl(vsiActiveFlag_);
    vessel.RegisterUIControl(attitudeDisplay_);
    vessel.RegisterUIControl(attitudeFlag_);
    vessel.RegisterUIControl(aoaHand_);
    vessel.RegisterUIControl(accelHand_);
    vessel.RegisterUIControl(trimHand_);
   
}

inline void Avionics::UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power)
{
    double gforce       = 0.0;
    double trim         = 0.0;
    double aoa          = 0.0;
    auto   vertSpeed    = 0.0;
    double bank         = 0.0;
    double pitch        = 0.0;
    double dynPress     = 0.0;


    isAeroDataActive_ = switchAvionPower_.IsOn() && (power.GetPowerLevel() > 20.0);
    isAtmoMode_ = switchAvionMode_.IsOn();

    if (isAeroDataActive_) {
        gforce = vessel.GetGs();
        trim = vessel.GetControlSurfaceLevel(AIRCTRL_ELEVATORTRIM);
        aoa = vessel.GetAOA();
        vertSpeed = vessel.GetVerticalSpeedFPM();
        pitch = vessel.GetPitch();
        bank = vessel.GetBank();
        dynPress = vessel.GetDynPressure();
    }

    // Vertical speed:
    double isPos = (vertSpeed >= 0) ? 1 : -1;
    auto absSpd = abs(vertSpeed);
    if (absSpd > 6000) absSpd = 6000;
    double spRot = (1 - pow((6000 - absSpd) / 6000, 2)) / 2;

    // vsi
    vsiRangeValue_ = 0.5 + (isPos * spRot);
    vsiActiveFlag_.SetState(isAeroDataActive_);

    // attitude
    attitudeDisplay_.SetAngle(bank);
    attitudeDisplay_.SetTranslate(0.0, (-0.100093 * pitch));
    attitudeFlag_.SetState(isAeroDataActive_);

    // accel
    accelRangeValue_ = (gforce + 2) / 6;

    // trim
    trimRangeValue_ = (trim + 1) / 2;

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

    aoaRangeValue_ = (aoaR + 0.2619) / 1.136;
}

inline void Avionics::UpdateSetCourse(double i)
{
    auto inc = setCourse_ + i;
    if (inc > PI2) inc -= PI2;
    if (inc < 0) inc += PI2;
    setCourse_ = inc;
}

inline void Avionics::UpdateSetHeading(double i)
{
    auto inc = setHeading_ + i;
    if (inc > PI2) inc -= PI2;
    if (inc < 0) inc += PI2;
    setHeading_ = inc;
}

inline void Avionics::LoadState(const std::string& line)
{
    std::istringstream in(line);
    int power, nav, mode;
    double heading, course;
    in >> power >> heading >> course >> nav >> mode;
    switchAvionPower_.SetState(power != 0);
    setHeading_ = heading;
    setCourse_ = course;
    switchNavMode_.SetState(nav != 0);
    switchAvionMode_.SetState(mode != 0);
}

inline std::string Avionics::GetState() const
{
    std::ostringstream os;
    os << (switchAvionPower_.IsOn() ? 1 : 0)
        << " " << bco::FormatDouble(setHeading_)
        << " " << bco::FormatDouble(setCourse_)
        << " " << (switchNavMode_.IsOn() ? 1 : 0)
        << " " << (switchAvionMode_.IsOn() ? 1 : 0);
    return os.str();
}
