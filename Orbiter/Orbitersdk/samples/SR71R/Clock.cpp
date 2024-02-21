//	Clock - SR-71r Orbiter Addon
//	Copyright(C) 2016  Blake Christensen
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

#include "StdAfx.h"

#include "bc_orbiter\Tools.h"

#include "Clock.h"
#include "Orbitersdk.h"
#include "SR71r_mesh.h"


Clock::Clock(bco::Vessel& Vessel) :
	startElapsedTime_(0.0),
	startTimerTime_(0.0),
	isTimerRunning_(false),
	currentTimerTime_(0.0)
{
	Vessel.AddControl(&clockTimerMinutesHand_);
	Vessel.AddControl(&clockTimerSecondsHand_);
	Vessel.AddControl(&clockElapsedHoursHand_);
	Vessel.AddControl(&clockElapsedMinutesHand_);
	Vessel.AddControl(&clockTimerReset_);
	Vessel.AddControl(&clockElapsedReset_);

	clockTimerReset_.Attach([&]() { ResetTimer(); });
	clockElapsedReset_.Attach([&]() { ResetElapsed(); });
}

void Clock::ResetElapsed()
{
    // The elapsed time is always isTimerRunning, when
    // pressed the elapsed time just restarts at zero.
    startElapsedTime_ = oapiGetSimTime();
}

void Clock::ResetTimer()
{
    // The timer has three states; isTimerRunning, stopped, reset.
    // If isTimerRunning, then we just stop.  If stopped we reset
    // and if reset we start isTimerRunning.
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

void Clock::HandlePostStep(bco::Vessel& Vessel, double simt, double simdt, double mjd)
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
bool Clock::HandleLoadState(bco::Vessel& Vessel, const std::string& line)
{
	int elapsedMission = 0;
	int isTimerRunning = 0;
	int elapsedTimer = 0;

	std::istringstream in(line);

	if (in >> elapsedMission >> isTimerRunning >> elapsedTimer) {
		auto Current = oapiGetSimTime();
		startElapsedTime_ = Current - (double)elapsedMission;

		isTimerRunning_ = (isTimerRunning == 1);

		if (isTimerRunning_)
		{
			startTimerTime_ = Current - elapsedTimer;
		}
		else
		{
			currentTimerTime_ = elapsedTimer;
		}
	}

	return true;
}

std::string Clock::HandleSaveState(bco::Vessel& Vessel)
{
	std::ostringstream os;

	auto Current = oapiGetSimTime();

	os << (int)(Current - startElapsedTime_) << " " << (isTimerRunning_ ? 1 : 0) << " " << (int)(Current - startTimerTime_);
	return os.str();
}
