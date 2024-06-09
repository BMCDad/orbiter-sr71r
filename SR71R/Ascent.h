//	AutoPilot - SR-71r Orbiter Addon
//	Copyright(C) 2024  Blake Christensen
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

#include "../bc_orbiter/vessel.h"

namespace bco = bc_orbiter;

class Ascent :
    public bco::vessel_component,
    public bco::post_step,
    public bco::manage_state
{
public:
    Ascent(bco::vessel& vessel);

    // post_step
    void handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd) override;

    // manage_state
    bool handle_load_state(bco::vessel& vessel, const std::string& line) override { return false; };
    std::string handle_save_state(bco::vessel& vessel) override { return ""; };

    double vessel_longitude() const { return vessel_longitude_; }
    double vessel_latitude() const { return vessel_latitude_; }
    double vessel_radius() const { return vessel_radius_; }

    double target_inclination() const { return target_inclination_; }
    double target_lan() const { return target_lan_; }
    double target_altitude() const { return target_altitude_; }
    double target_lst() const { return target_local_sidereal_time_; }

    double launch_heading() const { return launch_heading_; }
    double launch_alt_heading() const { return launch_alt_heading_; }
private:

    double vessel_longitude_{ 0.0 };
    double vessel_latitude_{ 0.0 };
    double vessel_radius_{ 0.0 };
    double launch_heading_{ 0.0 };
    double launch_alt_heading_{ 0.0 };

    double target_inclination_{ 0.0 };
    double target_lan_{ 0.0 };
    double target_altitude_{ 0.0 };
    double target_local_sidereal_time_{ 0.0 };
};
