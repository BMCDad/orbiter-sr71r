/*
ClockTimer - SR-71r Orbiter Addon
Copyright(C) 2025  Blake Christensen

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

-----

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
*/

#pragma once
#include <string>
#include <sstream>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class ClockTimer
{
public:
    ClockTimer() = default;
    ~ClockTimer() = default;

    void Setup(bco::Vessel& vessel);

    void Step(bco::Vessel& vessel, double simt, double simdt, double mjd);

    void ToggleTimer();
    void ResetElapsed() { elapsedMissionSeconds_ = 0.0; }

    void LoadState(const std::string& line);
    std::string GetState() const;
private:

    double  elapsedMissionSeconds_  { 0.0 };    // Total elapsed time in seconds since mission start
    double  timerSeconds_           { 0.0 };    // Total time on timer in seconds
    bool    isTimerRunning_         { false };

    // Animated values for the clock hands
    bco::AnimatedValue<bco::StateUpdateWrap> animClockSecond_{ 0.4 };
    bco::AnimatedValue<bco::StateUpdateWrap> animClockMinute_{ 0.4 };
    bco::AnimatedValue<bco::StateUpdateWrap> animClockHour_{ 0.4 };
    bco::AnimatedValue<bco::StateUpdateWrap> animClockTimerMinute_{ 0.4 };

    UINT aidVCClockSecond_{ 0 };
    UINT aidVCClockMinute_{ 0 };
    UINT aidVCClockHour_{ 0 };
    UINT aidVCClockTimerMinute_{ 0 };

};

inline void ClockTimer::Step(bco::Vessel& vessel, double simt, double simdt, double mjd)
{
    if (isTimerRunning_)
    {
        timerSeconds_ += simdt;
    }

    elapsedMissionSeconds_ += simdt;

    animClockHour_.SetTarget(           fmod((elapsedMissionSeconds_ / 3600), 12) / 12);
    animClockMinute_.SetTarget(         fmod((elapsedMissionSeconds_ / 60), 60) / 60);
    animClockSecond_.SetTarget(         fmod(timerSeconds_, 60) / 60);
    animClockTimerMinute_.SetTarget(    fmod(timerSeconds_ / 60, 60) / 60);

    animClockHour_.Update(simdt);
    animClockMinute_.Update(simdt);
    animClockSecond_.Update(simdt);
    animClockTimerMinute_.Update(simdt);

    if (oapiCockpitMode() == COCKPIT_VIRTUAL) {
        vessel.SetAnimation(aidVCClockHour_, animClockHour_.GetCurrent());
        vessel.SetAnimation(aidVCClockMinute_, animClockMinute_.GetCurrent());
        vessel.SetAnimation(aidVCClockSecond_, animClockSecond_.GetCurrent());
        vessel.SetAnimation(aidVCClockTimerMinute_, animClockTimerMinute_.GetCurrent());
    }

    if (oapiCockpitMode() == COCKPIT_PANELS) {
        // Get mesh handle for the 2d panel that has the clock.
        auto meshHandle = vessel.GetPanelMeshHandle(0); // Clock is on the main panel.
        if (meshHandle) {
            bco::RotateMesh(meshHandle, bm::pnl::pnlClockHour_id, bm::pnl::pnlClockHour_vrt, animClockHour_.GetCurrent() * -PI2);
            bco::RotateMesh(meshHandle, bm::pnl::pnlClockMinute_id, bm::pnl::pnlClockMinute_vrt, animClockMinute_.GetCurrent() * -PI2);
            bco::RotateMesh(meshHandle, bm::pnl::pnlClockSecond_id, bm::pnl::pnlClockSecond_vrt, animClockSecond_.GetCurrent() * -PI2);
            bco::RotateMesh(meshHandle, bm::pnl::pnlClockTimerMinute_id, bm::pnl::pnlClockTimerMinute_vrt, animClockTimerMinute_.GetCurrent() * -PI2);
        }
    }
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

inline void ClockTimer::Setup(bco::Vessel& vessel)
{
    // VC Events:
    vessel.RegisterVCEvent(bm::vc::ClockElapsedReset_loc, 0.01, [&]() { ResetElapsed(); }, 0);
    vessel.RegisterVCEvent(bm::vc::ClockTimerReset_loc, 0.01, [&]() { ToggleTimer(); }, 0);

    // Panel Events:
    vessel.RegisterPanelEvent(bm::pnl::pnlClockElapsedReset_RC, [&]() { ResetElapsed(); }, 0);
    vessel.RegisterPanelEvent(bm::pnl::pnlClockTimerReset_RC, [&]() { ToggleTimer(); }, 0);

    // VC Animations:
    auto vcIndex = vessel.GetMeshIndex(bm::vc::MESH_NAME);

    // VC Timer seconds hand
    aidVCClockSecond_ = vessel.BuildAnimation(
        vcIndex,
        { bm::vc::ClockSecond_id },
        bm::vc::ClockSecond_loc, bm::vc::ClockAxisFront_loc,
        (360.0 * RAD),
        0, 1);

    // VC Timer minutes hand
    aidVCClockTimerMinute_ = vessel.BuildAnimation(
        vcIndex,
        { bm::vc::ClockTimerMinute_id },
        bm::vc::ClockTimerMinute_loc, bm::vc::ClockAxisFront_loc,
        (360.0 * RAD),
        0, 1);

    // VC Mission elapsed minutes hand
    aidVCClockMinute_ = vessel.BuildAnimation(
        vcIndex,
        { bm::vc::ClockMinute_id },
        bm::vc::ClockMinute_loc, bm::vc::ClockAxisFront_loc,
        (360.0 * RAD),
        0, 1);

    // VC Mission elapsed hours hand
    aidVCClockHour_ = vessel.BuildAnimation(
        vcIndex,
        { bm::vc::ClockHour_id },
        bm::vc::ClockHour_loc, bm::vc::ClockAxisFront_loc,
        (360.0 * RAD),
        0, 1);
}