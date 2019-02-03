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

#pragma once

#include "bc_orbiter\Component.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\OnOffSwitch.h"
#include "bc_orbiter\PushButtonSwitch.h"
#include "bc_orbiter\VCGauge.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class Clock : public bco::Component
{
public:
	Clock(bco::BaseVessel* vessel);

	// Component
	void SetClassCaps() override;
	bool VCRedrawEvent(int id, int event, SURFHANDLE surf) override;
	bool LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	void SaveConfiguration(FILEHANDLE scn) const override;


	void Step(double simt, double simdt, double mjd);
	
private:
    void ResetElapsed();
    void ResetTimer();

	const char*				ConfigKey = "CLOCK";

    bco::PushButtonSwitch	switchResetElapsed_ { bt_mesh::SR71rVC::ClockElapsedReset_location,  0.01,  GetBaseVessel() };
    bco::PushButtonSwitch	switchStopWatch_    { bt_mesh::SR71rVC::ClockTimerReset_location,    0.01,  GetBaseVessel() };

	double					startElapsedTime_;
	double					startTimerTime_;
	double					currentTimerTime_;
	bool					isTimerRunning_;

	int						eidResetElapsed_;
	int						eidResetTimer_;

    bco::VCGaugeWrap        gaSecondHand_{ {bt_mesh::SR71rVC::ClockSecond_id },
                                                    bt_mesh::SR71rVC::ClockSecond_location,
                                                    bt_mesh::SR71rVC::ClockAxisFront_location,
                                                    (360*RAD),
                                                    0.4
                                                };

    bco::VCGaugeWrap        gaTimerMinute_{ {bt_mesh::SR71rVC::ClockTimerMinute_id },
                                                    bt_mesh::SR71rVC::ClockTimerMinute_location,
                                                    bt_mesh::SR71rVC::ClockAxisFront_location, 
                                                    (360 * RAD), 
                                                    0.4 
                                                };

    bco::VCGaugeWrap        gaHourHand_{ {bt_mesh::SR71rVC::ClockHour_id },
                                                    bt_mesh::SR71rVC::ClockHour_location,
                                                    bt_mesh::SR71rVC::ClockAxisFront_location,
                                                    (360 * RAD),
                                                    0.4
                                                };

    bco::VCGaugeWrap        gaMinuteHand_{ {bt_mesh::SR71rVC::ClockMinute_id },
                                                    bt_mesh::SR71rVC::ClockMinute_location,
                                                    bt_mesh::SR71rVC::ClockAxisFront_location,
                                                    (360 * RAD),
                                                    0.4
                                                };

};