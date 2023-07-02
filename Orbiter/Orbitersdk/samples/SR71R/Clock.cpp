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


Clock::Clock(bco::BaseVessel * vessel) :
	bco::Component(vessel),
	startElapsedTime_(0.0),
	startTimerTime_(0.0),
	isTimerRunning_(false),
	currentTimerTime_(0.0)
{
    //switchResetElapsed_.SetPressedFunc([this] { ResetElapsed(); });
    //vessel->RegisterVCEventTarget(&switchResetElapsed_);

    //switchStopWatch_.SetPressedFunc([this] { ResetTimer(); });
    //vessel->RegisterVCEventTarget(&switchStopWatch_);
}

void Clock::OnSetClassCaps()
{
    auto vessel = GetBaseVessel();
    
    //gaSecondHand_.Setup(vessel);
	//gaSecondHand_.Setup2(vessel, vessel->GetVCMeshIndex());
 //   gaTimerMinute_.Setup2(vessel, vessel->GetVCMeshIndex());
    //gaHourHand_.Setup2(vessel, vessel->GetVCMeshIndex());
    //gaMinuteHand_.Setup2(vessel, vessel->GetVCMeshIndex());
}

void Clock::ResetElapsed()
{
    // The elapsed time is always running, when
    // pressed the elapsed time just restarts at zero.
    startElapsedTime_ = oapiGetSimTime();
}

void Clock::ResetTimer()
{
    // The timer has three states; running, stopped, reset.
    // If running, then we just stop.  If stopped we reset
    // and if reset we start running.
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

void Clock::Step(double simt, double simdt, double mjd)
{
	// simt is simulator time in seconds.
	//  3600 - seconds in 60 minutes (minute hand).
	// 43200 - seconds in 12 hours (hour hand).

	auto elapsedRun = simt - startElapsedTime_;

	//auto hourHandT =	fmod(elapsedRun, 43200) / 43200;
	//auto minHandT =		fmod(elapsedRun, 3600) / 3600;

	signalMinute_.fire(fmod((elapsedRun / 60), 60));
	signalHour_.fire(fmod((elapsedRun / 3600), 12));

	if (isTimerRunning_)
	{
		currentTimerTime_ = simt - startTimerTime_;
	}
	
	//auto secHandT =		fmod(currentTimerTime_, 60) / 60;
	//auto timerHandT =	fmod(currentTimerTime_, 3600) / 3600;

	signalTimerSecond_.fire(fmod(currentTimerTime_, 60));
	signalTimerMinute_.fire(fmod((currentTimerTime_ / 60), 60));

    //gaSecondHand_.SetState(secHandT);
    //gaTimerMinute_.SetState(timerHandT);
    //gaHourHand_.SetState(hourHandT);
    //gaMinuteHand_.SetState(minHandT);

	// the anim* classes will eventually drive all cockpit mode UI elements, so it stays outside of the cockpit mode switch.
	// This will also keep the animations from 'jumping' when a cockpit mode is switched.
	//animTimerSecHand_.Step(secHandT, simdt);
	//animTimerMinuteHand_.Step(timerHandT, simdt);
	//animHourHand_.Step(hourHandT, simdt);
	//animMinuteHand_.Step(minHandT, simdt);

	//auto pMesh = GetBaseVessel()->GetpanelMeshHandle0();

	//switch (oapiCockpitMode())
	//{
	//case COCKPIT_PANELS:
	//	// panel anims.
	//	//bco::RotateMesh(pMesh, bm::pnl::pnlClockSecond_id,		bm::pnl::pnlClockSecond_verts,		animTimerSecHand_.GetState() * -PI2);
	//	//bco::RotateMesh(pMesh, bm::pnl::pnlClockTimerMinute_id,	bm::pnl::pnlClockTimerMinute_verts,	animTimerMinuteHand_.GetState() * -PI2);
	//	bco::RotateMesh(pMesh, bm::pnl::pnlClockHour_id,			bm::pnl::pnlClockHour_verts,		animHourHand_.GetState() * -PI2);
	//	bco::RotateMesh(pMesh, bm::pnl::pnlClockMinute_id,		bm::pnl::pnlClockMinute_verts,		animMinuteHand_.GetState() * -PI2);
	//	break;

	//case COCKPIT_VIRTUAL:
	//	//gaSecondHand_.SetAnimation2(GetBaseVessel(), animTimerSecHand_.GetState());
	//	//gaTimerMinute_.SetAnimation2(GetBaseVessel(), animTimerMinuteHand_.GetState());
	//	gaHourHand_.SetAnimation2(GetBaseVessel(), animHourHand_.GetState());
	//	gaMinuteHand_.SetAnimation2(GetBaseVessel(), animMinuteHand_.GetState());
	//	break;
	//}
}

bool Clock::OnLoadConfiguration(char * key, FILEHANDLE scn, const char * configLine)
{
	if (_strnicmp(key, ConfigKey, 5) != 0)
	{
		return false;
	}

	int elapsedMission;
	int isTimerRunning;
	int elapsedTimer;

	sscanf_s(configLine + 5, "%i%i%i", &elapsedMission, &isTimerRunning, &elapsedTimer);

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

	return true;
}

void Clock::OnSaveConfiguration(FILEHANDLE scn) const
{
	char cbuf[256];
	auto current = oapiGetSimTime();
	auto elMission = (int)(current - startElapsedTime_);

	auto elTimer = 0;

	if (isTimerRunning_)
	{
		elTimer = (int)(current - startTimerTime_);
	}
	else
	{
		elTimer = (int)currentTimerTime_;
	}
	
	auto running = isTimerRunning_ ? 1 : 0;

	sprintf_s(cbuf, "%i %i %i", elMission, running, elTimer);
	oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}

bool Clock::OnLoadPanel2D(int id, PANELHANDLE hPanel)
{
	for each (auto& v in pnlEvents_)
	{
		oapiRegisterPanelArea(v.id, v.rc, PANEL_REDRAW_NEVER);
		GetBaseVessel()->RegisterPanelArea(hPanel, v.id, v.rc, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
	}

	return true;
}

bool Clock::OnPanelMouseEvent(int id, int event)
{
	return bco::RunFor<PE>(pnlEvents_,
		[&](const PE& d) {return d.id == id; },
		[this](const PE& d) { d.update();});
}
