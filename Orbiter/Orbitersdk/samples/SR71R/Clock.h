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

#include "bc_orbiter\BaseVessel.h"
#include "bc_orbiter\Control.h"
#include "bc_orbiter\signals.h"

namespace bco = bc_orbiter;

class Clock : 
    public bco::vessel_component, 
    public bco::post_step, 
    public bco::manage_state
{
public:
	Clock();

    // post_step
    void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override;

	// manage_state
	bool handle_load_state(const std::string& line) override;
    std::string handle_save_state() override;

    bco::signal<double>&    TimerSecondsSignal()    { return signalTimerSecond_; }
    bco::signal<double>&    TimerMinutesSignal()    { return signalTimerMinute_; }
    bco::signal<double>&    ElapsedMinutesSignal()  { return signalMinute_; }
    bco::signal<double>&    ElapsedHoursSignal()    { return signalHour_; }

    bco::slot<bool>&        TimerResetSlot()        { return slotTimerReset_; }
    bco::slot<bool>&        ElapsedResetSlot()      { return slotElapsedReset_; }

private:
    void ResetElapsed();
    void ResetTimer();

	const char*				ConfigKey = "CLOCK";

    bco::signal<double>     signalTimerSecond_;         // Current timer seconds (0 - 59)
    bco::signal<double>     signalTimerMinute_;         // Current timer minutes (0 - 59)
    bco::signal<double>     signalMinute_;              // Current mission time minutes (0 - 59)
    bco::signal<double>     signalHour_;                // Current mission time hours (0 - 11)

    bco::slot<bool>         slotTimerReset_;
    bco::slot<bool>         slotElapsedReset_;

	double					startElapsedTime_;
	double					startTimerTime_;
	double					currentTimerTime_;
	bool					isTimerRunning_;
};