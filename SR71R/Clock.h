//	Clock - SR-71r Orbiter Addon
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

#include "../bc_orbiter/Vessel.h"
#include "../bc_orbiter/control.h"
#include "../bc_orbiter/signals.h"
#include "../bc_orbiter/RotaryDisplay.h"
#include "../bc_orbiter/VesselEvent.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"
#include "SR71rPanelRight_mesh.h"


namespace bco = bc_orbiter;

class Clock : public bco::VesselComponent
{
public:
    Clock(bco::Vessel& vessel);

    // PostStep
    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override;

    // ManageState
    bool HandleLoadState(bco::Vessel& vessel, const std::string& line) override;
    std::string HandleSaveState(bco::Vessel& vessel) override;

private:
    void ResetElapsed();
    void ResetTimer();

    const char* ConfigKey = "CLOCK";

    double					startElapsedTime_;
    double					startTimerTime_;
    double					currentTimerTime_;
    bool					isTimerRunning_;

    bco::RotaryDisplayWrap	clockTimerSecondsHand_{
                                                { bm::vc::ClockSecond_id },
                                                bm::vc::ClockSecond_loc, bm::vc::ClockAxisFront_loc,
                                                bm::pnl::pnlClockSecond_id,
                                                bm::pnl::pnlClockSecond_vrt,
                                                (360 * RAD),	// Clockwise
                                                0.4
    };

    bco::RotaryDisplayWrap	clockTimerMinutesHand_{
                                                { bm::vc::ClockTimerMinute_id },
                                                bm::vc::ClockTimerMinute_loc, bm::vc::ClockAxisFront_loc,
                                                bm::pnl::pnlClockTimerMinute_id,
                                                bm::pnl::pnlClockTimerMinute_vrt,
                                                (360 * RAD),	// Clockwise
                                                0.4
    };

    bco::RotaryDisplayWrap	clockElapsedMinutesHand_{
                                                { bm::vc::ClockMinute_id },
                                                bm::vc::ClockMinute_loc, bm::vc::ClockAxisFront_loc,
                                                bm::pnl::pnlClockMinute_id,
                                                bm::pnl::pnlClockMinute_vrt,
                                                (360 * RAD),	// Clockwise
                                                0.4
    };

    bco::RotaryDisplayWrap	clockElapsedHoursHand_{
                                                { bm::vc::ClockHour_id },
                                                bm::vc::ClockHour_loc, bm::vc::ClockAxisFront_loc,
                                                bm::pnl::pnlClockHour_id,
                                                bm::pnl::pnlClockHour_vrt,
                                                (360 * RAD),	// Clockwise
                                                0.4
    };

    bco::VesselEvent         clockTimerReset_{
        bm::vc::ClockTimerReset_loc,
        0.01,
        cmn::vc::main,
        bm::pnl::pnlClockTimerReset_RC,
        cmn::panel::main
    };

    bco::VesselEvent         clockElapsedReset_{
        bm::vc::ClockElapsedReset_loc,
        0.01,
        cmn::vc::main,
        bm::pnl::pnlClockElapsedReset_RC,
        cmn::panel::main
    };

};

inline Clock::Clock(bco::Vessel& vessel) 
  : startElapsedTime_(0.0),
    startTimerTime_(0.0),
    isTimerRunning_(false),
    currentTimerTime_(0.0)
{
    vessel.AddControl(&clockTimerMinutesHand_);
    vessel.AddControl(&clockTimerSecondsHand_);
    vessel.AddControl(&clockElapsedHoursHand_);
    vessel.AddControl(&clockElapsedMinutesHand_);
    vessel.AddControl(&clockTimerReset_);
    vessel.AddControl(&clockElapsedReset_);

    clockTimerReset_.Attach([&](VESSEL4&) { ResetTimer(); });
    clockElapsedReset_.Attach([&](VESSEL4&) { ResetElapsed(); });
}

inline void Clock::ResetElapsed()
{
    // The elapsed time is always isTimerRunning, when
    // pressed the elapsed time just restarts at zero.
    startElapsedTime_ = oapiGetSimTime();
}

inline void Clock::ResetTimer()
{
    // The timer has three states; isTimerRunning, stopped, Reset.
    // If isTimerRunning, then we just stop.  If stopped we Reset
    // and if Reset we start isTimerRunning.
    if (isTimerRunning_)
    {
        isTimerRunning_ = false;
    }
    else
    {
        if (currentTimerTime_ == 0.0)
        {
            startTimerTime_ = oapiGetSimTime();
            currentTimerTime_ = startTimerTime_;
            isTimerRunning_ = true;
        }
        else
        {
            currentTimerTime_ = 0.0;
            startTimerTime_ = 0.0;
        }
    }
}

inline void Clock::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd)
{
    // simt is simulator time in seconds.
    //  3600 - seconds in 60 minutes (minute hand).
    // 43200 - seconds in 12 hours (hour hand).

    auto elapsedRun = simt - startElapsedTime_;


    clockElapsedMinutesHand_.set_state(fmod((elapsedRun / 60), 60) / 60);
    clockElapsedHoursHand_.set_state(fmod((elapsedRun / 3600), 12) / 60);

    if (isTimerRunning_)
    {
        currentTimerTime_ = simt - startTimerTime_;
    }

    clockTimerSecondsHand_.set_state(fmod(currentTimerTime_, 60) / 60);
    clockTimerMinutesHand_.set_state(fmod((currentTimerTime_ / 60), 60) / 60);
}

// [elapsedMissionTime] [isTimerRunning] [elapsedTimer]
inline bool Clock::HandleLoadState(bco::Vessel& vessel, const std::string& line)
{
    int elapsedMission = 0;
    int isTimerRunning = 0;
    int elapsedTimer = 0;

    std::istringstream in(line);

    if (in >> elapsedMission >> isTimerRunning >> elapsedTimer) {
        auto current = oapiGetSimTime();
        startElapsedTime_ = current - (double)elapsedMission;

        isTimerRunning_ = (isTimerRunning == 1);

        if (isTimerRunning_)
        {
            startTimerTime_ = current - elapsedTimer;
        }
        else
        {
            currentTimerTime_ = elapsedTimer;
        }
    }

    return true;
}

inline std::string Clock::HandleSaveState(bco::Vessel& vessel)
{
    std::ostringstream os;

    auto current = oapiGetSimTime();

    os << (int)(current - startElapsedTime_) << " " << (isTimerRunning_ ? 1 : 0) << " " << (int)(current - startTimerTime_);
    return os.str();
}
