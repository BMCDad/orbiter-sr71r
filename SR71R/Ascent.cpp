//	Ascent - SR-71r Orbiter Addon
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

#include "StdAfx.h"
#include "Orbitersdk.h"1
#include "Ascent.h"

Ascent::Ascent(bco::vessel& vessel)
{
    // Dummy target values to get going.
    target_inclination_ = 74.51 * RAD;
    target_lan_ = 169.03 * RAD;
    target_local_sidereal_time_ = 24.0 * (target_lan_ / PI2);
};

// post_step
void Ascent::handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd)
{
    double lng, lat, rad;
    oapiGetEquPos(vessel.GetHandle(), &lng, &lat, &rad);
    vessel_longitude_ = lng;
    vessel_latitude_ = lat;
    vessel_radius_ = rad;

    double direct, alt;
    if (bc_orbiter::CalcLaunchHeading(vessel_latitude_, target_inclination_, direct, alt)) {
        launch_heading_ = direct;
        launch_alt_heading_ = alt;
    }
}
