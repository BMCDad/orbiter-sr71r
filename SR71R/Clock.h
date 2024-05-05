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

#include "bc_orbiter/vessel.h"
#include "bc_orbiter/control.h"
#include "bc_orbiter/signals.h"
#include "bc_orbiter/rotary_display.h"
#include "bc_orbiter/simple_event.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class Clock : 
    public bco::vessel_component, 
    public bco::post_step, 
    public bco::manage_state
{
public:
	Clock(bco::vessel& vessel);

    // post_step
    void handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd) override;

	// manage_state
	bool handle_load_state(bco::vessel& vessel, const std::string& line) override;
    std::string handle_save_state(bco::vessel& vessel) override;

private:
    void ResetElapsed();
    void ResetTimer();

	const char*				ConfigKey = "CLOCK";

	double					startElapsedTime_;
	double					startTimerTime_;
	double					currentTimerTime_;
	bool					isTimerRunning_;

	bco::rotary_display_wrap	clockTimerSecondsHand_ {
												{ bm::vc::ClockSecond_id },
												bm::vc::ClockSecond_loc, bm::vc::ClockAxisFront_loc,
												bm::pnl::pnlClockSecond_id,
												bm::pnl::pnlClockSecond_vrt,
												(360 * RAD),	// Clockwise
												0.4
											};

	bco::rotary_display_wrap	clockTimerMinutesHand_ {
												{ bm::vc::ClockTimerMinute_id },
												bm::vc::ClockTimerMinute_loc, bm::vc::ClockAxisFront_loc,
												bm::pnl::pnlClockTimerMinute_id,
												bm::pnl::pnlClockTimerMinute_vrt,
												(360 * RAD),	// Clockwise
												0.4
											};

	bco::rotary_display_wrap	clockElapsedMinutesHand_ {
												{ bm::vc::ClockMinute_id },
												bm::vc::ClockMinute_loc, bm::vc::ClockAxisFront_loc,
												bm::pnl::pnlClockMinute_id,
												bm::pnl::pnlClockMinute_vrt,
												(360 * RAD),	// Clockwise
												0.4
											};

	bco::rotary_display_wrap	clockElapsedHoursHand_ {
												{ bm::vc::ClockHour_id },
												bm::vc::ClockHour_loc, bm::vc::ClockAxisFront_loc,
												bm::pnl::pnlClockHour_id,
												bm::pnl::pnlClockHour_vrt,
												(360 * RAD),	// Clockwise
												0.4
											};

	bco::simple_event<>						clockTimerReset_ {
												bm::vc::ClockTimerReset_loc,
												0.01,
												bm::pnl::pnlClockTimerReset_RC
											};

	bco::simple_event<>						clockElapsedReset_ {
												bm::vc::ClockElapsedReset_loc,
												0.01,
												bm::pnl::pnlClockElapsedReset_RC
											};

};