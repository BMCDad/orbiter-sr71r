/*
ClockTimer - SR-71r Orbiter Addon
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

ClockTimer has a mission elapsed timer that uses hours and seconds.  It can
be reset to 0, but is always running.  It also has a stop watch timer that
uses minutes and seconds.  It can be started, stopped and reset.  It uses
four hands:

VC: ClockSecond
    ClockMinute
    ClockHour
    ClockTimerMinute

Panel:
    pnlClockSecond
    pnlClockMinute
    pnlClockHour
    pnlClockTimerMinute

Events:
    ResetMissionElapsed:  Resets the mission elapsed time to zero.
    ResetTimer:  If timer is zero, it starts the timer.  If timer
        is running, it stops the timer.  If timer is stopped and
        not zero, the timer is reset to zero.
*/

#pragma once
#include <string>
#include <sstream>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"
#include "..\bc_orbiter\Tools.h"

#include "SR71r_mesh.h"
#include "SR71WrapGauge.h"
#include "SR71MouseClick.h"

namespace bco = bc_orbiter;

class ClockTimer
{
public:
    ClockTimer(bco::Vessel& vessel);
    ~ClockTimer() = default;

    void Setup(bco::Vessel& vessel);
    void UpdateState(double simdt);             // Called from Step() in the vessel to update clock state.

    //void LoadVC();       // Called when the VC is loaded to setup animations.
    //void LoadPanel(bco::Vessel& vessel, PANELHANDLE handle);    // Called when the 2D panel is loaded to setup animations.

    void ToggleTimer();
    void ResetElapsed() { elapsedMissionSeconds_ = 0.0; }

    void LoadState(const std::string& line);
    std::string GetState() const;
private:

    double  elapsedMissionSeconds_  { 0.0 };    // Total elapsed time in seconds since mission start
    double  timerSeconds_           { 0.0 };    // Total time on timer in seconds
    bool    isTimerRunning_         { false };

    // Animated values for the clock hands.  These anim values are part of the componet state
    // and are updated each step of the simulation.
    const double HandSpeed = 0.4;

    int eventId_ResetElapsed_{ -1 };
    int eventId_ToggleTimer_{ -1 };

    double secondValue_{ 0.0 };
    double minuteValue_{ 0.0 };
    double hourValue_{ 0.0 };
    double timerMinuteValue_{ 0.0 };

    SR71::WrapGauge secondHand_{
        { bm::vc::ClockTimerSecond_id },
        bm::vc::ClockTimerSecond_loc, bm::vc::ClockAxisFront_loc,
        bm::pnl::pnlClockSecond_id,
        bm::pnl::pnlClockSecond_vrt,
        secondValue_,
        SR71R::MainPanel_ID
    };

    SR71::WrapGauge minuteHand_{
        { bm::vc::ClockMinute_id },
        bm::vc::ClockMinute_loc, bm::vc::ClockAxisFront_loc,
        bm::pnl::pnlClockMinute_id,
        bm::pnl::pnlClockMinute_vrt,
        minuteValue_,
        SR71R::MainPanel_ID
    };

    SR71::WrapGauge hourHand_{
        { bm::vc::ClockHour_id },
        bm::vc::ClockHour_loc, bm::vc::ClockAxisFront_loc,
        bm::pnl::pnlClockHour_id,
        bm::pnl::pnlClockHour_vrt,
        hourValue_,
        SR71R::MainPanel_ID
    };

    SR71::WrapGauge timerMinuteHand_{
        { bm::vc::ClockTimerMinute_id },
        bm::vc::ClockTimerMinute_loc, bm::vc::ClockAxisFront_loc,
        bm::pnl::pnlClockTimerMinute_id,
        bm::pnl::pnlClockTimerMinute_vrt,
        timerMinuteValue_,
        SR71R::MainPanel_ID
    };

    SR71::MouseClick btnResetElapsed_{
        bm::vc::ClockElapsedReset_loc,
        SR71R::ToggleHitRadius,
        bm::pnl::pnlClockElapsedReset_RC,
        SR71R::MainPanel_ID,
        [this]() {ResetElapsed(); }
    };

    SR71::MouseClick btnToggleTimer_{
        bm::vc::ClockTimerReset_loc,
        SR71R::ToggleHitRadius,
        bm::pnl::pnlClockTimerReset_RC,
        SR71R::MainPanel_ID,
        [this]() {ToggleTimer(); }
    };
};

inline ClockTimer::ClockTimer(bco::Vessel& vessel)
{
    vessel.RegisterUIControl(secondHand_);
    vessel.RegisterUIControl(minuteHand_);
    vessel.RegisterUIControl(hourHand_);
    vessel.RegisterUIControl(timerMinuteHand_);
    vessel.RegisterUIControl(btnResetElapsed_);
    vessel.RegisterUIControl(btnToggleTimer_);
}

inline void ClockTimer::Setup(bco::Vessel& vessel)
{
    //eventId_ResetElapsed_ = vessel.RegisterEventHandler([this](int, int) { ResetElapsed(); return true; });
    //eventId_ToggleTimer_ = vessel.RegisterEventHandler([this](int, int) { ToggleTimer(); return true; });
}

inline void ClockTimer::UpdateState(double simdt)
{
    if (isTimerRunning_)
    {
        timerSeconds_ += simdt;
    }

    elapsedMissionSeconds_ += simdt;

    hourValue_ = fmod((elapsedMissionSeconds_ / 3600), 12) / 12;
    minuteValue_ = fmod((elapsedMissionSeconds_ / 60), 60) / 60;
    secondValue_ = fmod(timerSeconds_, 60) / 60;
    timerMinuteValue_ = fmod(timerSeconds_ / 60, 60) / 60;
}

inline void ClockTimer::ToggleTimer()
{
    if (isTimerRunning_)
    {
        isTimerRunning_ = false;
    }
    else
    {
        if (timerSeconds_ > 0.0) {
            timerSeconds_ = 0.0;   // Timer is stopped, but not at zero, so reset it to zero.
        }
        else {
            isTimerRunning_ = true; // If timer is stopped and time is zero, start it.
        }
    }
}

/* LoadState
* Load clock timer state from a line of text.
* Format:
* [elapsed Mission Time in seconds (int)] [is timer running 0/1 (int)] [timer time in seconds (int)]
*/
inline void ClockTimer::LoadState(const std::string& line)
{
    int elapsedMission = 0;
    int isTimerRunning = 0;
    int elapsedTimer = 0;

    std::istringstream in(line);

    if (in >> elapsedMission >> isTimerRunning >> elapsedTimer) {
        elapsedMissionSeconds_ = (double)elapsedMission;
        timerSeconds_ = (double)elapsedTimer;
        isTimerRunning_ = (isTimerRunning == 1);
    }
}

/* GetState
* Returns a string representing the current state of the clock timer.
*/
inline std::string ClockTimer::GetState() const
{
    std::ostringstream out;
    out << (int)elapsedMissionSeconds_ << " " << (isTimerRunning_ ? 1 : 0) << " " << (int)timerSeconds_;
    return out.str();
}

//inline void ClockTimer::LoadVC()
//{
//    bco::LoadVCSimpleEvent(eventId_ResetElapsed_, bm::vc::ClockElapsedReset_loc, 0.01);
//    bco::LoadVCSimpleEvent(eventId_ToggleTimer_, bm::vc::ClockTimerReset_loc, 0.01);
//}
//
//inline void ClockTimer::LoadPanel(bco::Vessel& vessel, PANELHANDLE handle)
//{
//    bco::LoadPanelSimpleEvent(vessel, eventId_ResetElapsed_, handle, bm::pnl::pnlClockElapsedReset_RC);
//    bco::LoadPanelSimpleEvent(vessel, eventId_ToggleTimer_, handle, bm::pnl::pnlClockTimerReset_RC);
//}
