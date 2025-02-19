//  AeroData - SR-71r Orbiter Addon
//  Copyright(C) 2023  Blake Christensen
//  
//  This program is free software : you can redistribute it and / or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "../bc_orbiter/control.h"
#include "../bc_orbiter/OnOffInput.h"
#include "../bc_orbiter/RotaryDisplay.h"
#include "../bc_orbiter/signals.h"
#include "../bc_orbiter/transform_display.h"
#include "../bc_orbiter/VesselEvent.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "SR71r_common.h"
#include "Common.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"
#include "SR71rPanelRight_mesh.h"

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

class Avionics : public bco::VesselComponent, public bco::PowerConsumer
{

public:
    enum AvionMode { AvionAtmo, AvionExo };

    Avionics(bco::Vessel& vessel, bco::PowerProvider& pwr);
    ~Avionics() {}

    // PostStep
    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override;

    // ManageState
    bool HandleLoadState(bco::Vessel& vessel, const std::string& line) override;
    std::string HandleSaveState(bco::Vessel& vessel) override;

    // PowerConsumer
    double AmpDraw() const { return IsPowered() ? 6.0 : 0.0; }

    void SetCourse(double s);
    void SetHeading(double s);


    bool	IsAeroActive() const { return isAeroDataActive_; }		// Is aero available (switch is on, power is adequate)
    bool	IsAeroAtmoMode() const { return isAtmoMode_; }

    // Signals:
    bco::signal<double>& SetCourseSignal() { return setCourseSignal_; }

    bco::signal<double>& SetHeadingSignal() { return setHeadingSignal_; }
    bco::slot<double>& SetHeadingSlot() { return setHeadingSlot_; }

    bco::signal<double>& GForceSignal() { return gforceSignal_; }
    bco::signal<double>& TrimSignal() { return trimSignal_; }
    bco::signal<double>& AOASignal() { return aoaSignal_; }

private:
    bco::Vessel& vessel_;
    bco::PowerProvider& power_;

    bool IsPowered() const { return switchAvionPower_.IsOn() && (power_.VoltsAvailable() > 24.0); }

    bool		isAeroDataActive_;
    bool		isAtmoMode_;

    // Signals:
    bco::signal<double>		setCourseSignal_;

    bco::signal<double>		setHeadingSignal_;
    bco::slot<double>		setHeadingSlot_;

    bco::signal<double>		gforceSignal_;
    bco::signal<double>		trimSignal_;
    bco::signal<double>		aoaSignal_;

    void UpdateSetCourse(double i);
    void UpdateSetHeading(double i);

    bco::OnOffInput       switchAvionPower_{      // Main avionics power
        { bm::vc::SwAvionics_id },
        bm::vc::SwAvionics_loc, bm::vc::PowerTopRightAxis_loc,
        toggleOnOff,
        bm::pnlright::pnlPwrAvion_id,
        bm::pnlright::pnlPwrAvion_vrt,
        bm::pnlright::pnlPwrAvion_RC,
        1
    };

    bco::OnOffInput       switchAvionMode_{       // Atmosphere=On, External=Off
        { bm::vc::vcAvionMode_id },
        bm::vc::vcAvionMode_loc, bm::vc::navPanelAxis_loc,
        toggleOnOff,
        bm::pnl::pnlAvionMode_id,
        bm::pnl::pnlAvionMode_vrt,
        bm::pnl::pnlAvionMode_RC,
    };

    bco::OnOffInput		switchNavMode_{		// Nav mode 1 2
        { bm::vc::vcNavMode_id },
        bm::vc::vcNavMode_loc, bm::vc::navPanelAxis_loc,
        toggleOnOff,
        bm::pnl::pnlNavMode_id,
        bm::pnl::pnlNavMode_vrt,
        bm::pnl::pnlNavMode_RC,
    };

    bco::VesselEvent        dialSetCourseIncrement_{
        bm::vc::CourseKnobInc_loc,
        0.01,
        cmn::vc::main,
        bm::pnl::pnlSetCourseInc_RC,
        cmn::panel::main
    };

    bco::VesselEvent     dialSetCourseDecrement_{
        bm::vc::CourseKnobDec_loc,
        0.01,
        cmn::vc::main,
        bm::pnl::pnlSetCourseDec_RC,
        cmn::panel::main
    };

    bco::VesselEvent     dialSetHeadingIncrement_{
        bm::vc::HeadingKnobInc_loc,
        0.01,
        cmn::vc::main,
        bm::pnl::pnlSetHeadingInc_RC,
        cmn::panel::main
    };

    bco::VesselEvent     dialSetHeadingDecrement_{
        bm::vc::HeadingKnobDec_loc,
        0.01,
        cmn::vc::main,
        bm::pnl::pnlSetHeadingDec_RC,
        cmn::panel::main 
    };

    // ***   VSI  *** //
    bco::RotaryDisplayTarget  vsiHand_ {
        { bm::vc::gaVSINeedle_id },
        bm::vc::gaVSINeedle_loc, bm::vc::VSIAxis_loc,
        bm::pnl::pnlVSINeedle_id,
        bm::pnl::pnlVSINeedle_vrt,
        (340 * RAD),
        2.0
    };

    bco::VesselTextureElement       vsiActiveFlag_ {
        bm::vc::VSIOffFlag_id,
        bm::vc::VSIOffFlag_vrt,
        cmn::vc::main,
        bm::pnl::pnlVSIOffFlag_id,
        bm::pnl::pnlVSIOffFlag_vrt,
        cmn::panel::main
    };

    // ***  ATTITUDE  *** //
    bco::transform_display	     attitudeDisplay_ {
        bm::vc::AttitudeIndicator_id,
        bm::vc::AttitudeIndicator_vrt,
        bm::pnl::pnlAttitudeIndicator_id,
        bm::pnl::pnlAttitudeIndicator_vrt
    };

    bco::VesselTextureElement           attitudeFlag_{
        bm::vc::AttitudeFlagOff_id,
        bm::vc::AttitudeFlagOff_vrt,
        cmn::vc::main,
        bm::pnl::pnlAttitudeFlagOff_id,
        bm::pnl::pnlAttitudeFlagOff_vrt,
        cmn::panel::main
    };

    // ***  AOA  TRIM  GFORCE  ***  //
    bco::RotaryDisplayTarget  aoaHand_ {
        { bm::vc::AOANeedle_id },
        bm::vc::AOANeedle_loc, bm::vc::AOAAxis_loc,
        bm::pnl::pnlAOANeedle_id,
        bm::pnl::pnlAOANeedle_vrt,
        (75 * RAD),
        2.0
    };

    bco::RotaryDisplayTarget  trimHand_ {
        { bm::vc::TrimNeedle_id },
        bm::vc::TrimNeedle_loc, bm::vc::TrimAxis_loc,
        bm::pnl::pnlTrimNeedle_id,
        bm::pnl::pnlTrimNeedle_vrt,
        (180 * RAD),
        1.0
    };

    bco::RotaryDisplayTarget  accelHand_ {
        { bm::vc::AccelNeedle_id },
        bm::vc::AccelNeedle_loc, bm::vc::AccelAxis_loc,
        bm::pnl::pnlAccelNeedle_id,
        bm::pnl::pnlAccelNeedle_vrt,
        (295 * RAD),
        1.0
    };
};

inline Avionics::Avionics(bco::Vessel& vessel, bco::PowerProvider& pwr) :
    power_(pwr),
    setHeadingSlot_([&](double v) { setHeadingSignal_.fire(v); }),
    vessel_(vessel)
{
    power_.AttachConsumer(this);

    vessel.AddControl(&switchAvionMode_);
    vessel.AddControl(&switchAvionPower_);
    vessel.AddControl(&switchNavMode_);

    vessel.AddControl(&dialSetCourseDecrement_);
    vessel.AddControl(&dialSetCourseIncrement_);
    vessel.AddControl(&dialSetHeadingDecrement_);
    vessel.AddControl(&dialSetHeadingIncrement_);

    dialSetCourseDecrement_.Attach([&](VESSEL4&) { UpdateSetCourse(-0.0175); });
    dialSetCourseIncrement_.Attach([&](VESSEL4&) { UpdateSetCourse(0.0175); });
    dialSetHeadingDecrement_.Attach([&](VESSEL4&) { UpdateSetHeading(-0.0175); });
    dialSetHeadingIncrement_.Attach([&](VESSEL4&) { UpdateSetHeading(0.0175); });

    vessel.AddControl(&vsiHand_);
    vessel.AddControl(&vsiActiveFlag_);

    vessel.AddControl(&attitudeDisplay_);
    vessel.AddControl(&attitudeFlag_);

    vessel.AddControl(&aoaHand_);
    vessel.AddControl(&trimHand_);
    vessel.AddControl(&accelHand_);
}


// PostStep
inline void Avionics::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) {
    double gforce = 0.0;
    double trim = 0.0;
    double aoa = 0.0;
    auto   vertSpeed = 0.0;
    double bank = 0.0;
    double pitch = 0.0;
    double dynPress = 0.0;


    isAeroDataActive_ = IsPowered();
    isAtmoMode_ = switchAvionMode_.IsOn();

    if (isAeroDataActive_) {
        gforce = bco::GetVesselGs(vessel);
        trim = vessel.GetControlSurfaceLevel(AIRCTRL_ELEVATORTRIM);
        aoa = vessel.GetAOA();
        vertSpeed = bco::GetVerticalSpeedFPM(&vessel);
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
    vsiHand_.set_state(0.5 + (isPos * spRot));
    vsiActiveFlag_.set_state(vessel_, IsPowered());

    // attitude
    attitudeDisplay_.SetAngle(bank);
    attitudeDisplay_.SetTransform(0.0, (-0.100093 * pitch));
    attitudeFlag_.set_state(vessel, IsPowered());

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

// ManageState
inline bool Avionics::HandleLoadState(bco::Vessel& vessel, const std::string& line) {
    //sscanf_s(configLine + 8, "%i%i%i%i%i", &power, &heading, &course, &navSelect, &navMode);
    std::istringstream in(line);
    in >> switchAvionPower_ >> setHeadingSignal_ >> setCourseSignal_ >> switchNavMode_ >> switchAvionMode_;
    return true;
}

inline std::string Avionics::HandleSaveState(bco::Vessel& vessel) {
    std::ostringstream os;
    os << switchAvionPower_ << " " << setHeadingSignal_ << " " << setCourseSignal_ << " " << switchNavMode_ << " " << switchAvionMode_;
    return os.str();
}

inline void Avionics::SetCourse(double s)
{
    setCourseSignal_.update(s * RAD);
    UpdateSetCourse(0.0);	 // force the signal to fire.
}

inline void Avionics::UpdateSetCourse(double i)
{
    auto inc = setCourseSignal_.current() + i;
    if (inc > PI2) inc -= PI2;
    if (inc < 0) inc += PI2;
    setCourseSignal_.fire(inc);

    //	sprintf(oapiDebugString(), "Set Course: %+4.2f", setCourseSignal_.current());
}

inline void Avionics::SetHeading(double s)
{
    setHeadingSignal_.update(s * RAD);
    UpdateSetHeading(0.0);	 // force the signal to fire.
}

inline void Avionics::UpdateSetHeading(double i)
{
    auto inc = setHeadingSignal_.current() + i;
    if (inc > PI2) inc -= PI2;
    if (inc < 0) inc += PI2;
    setHeadingSignal_.fire(inc);

    //	sprintf(oapiDebugString(), "Set Course: %+4.2f", setCourseSignal_.current());
}
