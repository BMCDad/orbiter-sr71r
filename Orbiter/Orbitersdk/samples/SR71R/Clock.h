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

#include "bc_orbiter\Component.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\BaseVessel.h"
#include "bc_orbiter\signals.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class Clock : public bco::Component
{
public:
	Clock(bco::BaseVessel* vessel);

	// Component
	void OnSetClassCaps() override;
	bool OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	void OnSaveConfiguration(FILEHANDLE scn) const override;

    bool OnLoadPanel2D(int id, PANELHANDLE hPanel) override;
    bool OnPanelMouseEvent(int id, int event) override;

	void Step(double simt, double simdt, double mjd);
	
    bco::signal<double>&    TimerSecondsSignal()    { return signalTimerSecond_; }
    bco::signal<double>&    TimerMinutesSignal()    { return signalTimerMinute_; }
    bco::signal<double>&    ElapsedMinutesSignal()  { return signalMinute_; }
    bco::signal<double>&    ElapsedHoursSignal()    { return signalHour_; }

private:
    void ResetElapsed();
    void ResetTimer();

	const char*				ConfigKey = "CLOCK";

    bco::signal<double>     signalTimerSecond_;         // Current timer seconds (0 - 59)
    bco::signal<double>     signalTimerMinute_;         // Current timer minutes (0 - 59)
    bco::signal<double>     signalMinute_;              // Current mission time minutes (0 - 59)
    bco::signal<double>     signalHour_;                // Current mission time hours (0 - 11)

    //bco::PushButtonSwitch	switchResetElapsed_ { bm::vc::ClockElapsedReset_location,  0.01 };
    //bco::PushButtonSwitch	switchStopWatch_    { bm::vc::ClockTimerReset_location,    0.01 };

	double					startElapsedTime_;
	double					startTimerTime_;
	double					currentTimerTime_;
	bool					isTimerRunning_;

	int						eidResetElapsed_;
	int						eidResetTimer_;

    struct AD
    {
        const UINT group;
        const NTVERTEX* verts;
    };

    const int ID_ResetTimer     = GetBaseVessel()->GetIdForComponent(this);
    const int ID_ResetElapsed   = GetBaseVessel()->GetIdForComponent(this);

    struct PE
    {
        int id;
        const UINT group;
        const RECT rc;
        std::function<void(void)> update;
    };
    std::vector<PE> pnlEvents_
    {
        {ID_ResetElapsed,   bm::pnl::pnlClockElapsedReset_id,  bm::pnl::pnlClockElapsedReset_RC, [&](void) { ResetElapsed(); }},
        {ID_ResetTimer,     bm::pnl::pnlClockTimerReset_id,    bm::pnl::pnlClockTimerReset_RC,   [&](void) { ResetTimer(); }}
    };
};