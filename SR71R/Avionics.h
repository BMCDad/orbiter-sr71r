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
#include "../bc_orbiter/SimpleEvent.h"
#include "../bc_orbiter/transform_display.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "SR71r_common.h"
#include "Common.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"
#include "SR71rPanelRight_mesh.h"

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

class Avionics 
  : public bco::VesselComponent,
    public bco::PostStep,
    public bco::ManageState,
    public bco::PowerConsumer
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

    bco::SimpleEvent<>		dialSetCourseIncrement_{
        bm::vc::CourseKnobInc_loc,
        0.01,
        0,
        bm::pnl::pnlSetCourseInc_RC,
        0
    };

    bco::SimpleEvent<>     dialSetCourseDecrement_{
        bm::vc::CourseKnobDec_loc,
        0.01,
        0,
        bm::pnl::pnlSetCourseDec_RC,
        0
    };

    bco::SimpleEvent<>     dialSetHeadingIncrement_{
        bm::vc::HeadingKnobInc_loc,
        0.01,
        0,
        bm::pnl::pnlSetHeadingInc_RC,
        0
    };

    bco::SimpleEvent<>     dialSetHeadingDecrement_{
        bm::vc::HeadingKnobDec_loc,
        0.01,
        0,
        bm::pnl::pnlSetHeadingDec_RC,
        0 
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
