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

#include "StdAfx.h"

#include "Orbitersdk.h"
#include "AutoPilot.h"

AP::AutoPilot::AutoPilot(bco::vessel& vessel)
{
}

void AP::AutoPilot::handle_set_class_caps(bco::vessel& vessel)
{
}

void AP::AutoPilot::handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd)
{
    if ((prev_active_ & Progs::HoldHeading) != (active_ & Progs::HoldHeading)) {
        IsActive(active_, Progs::HoldHeading) ? heading_prog_.start(vessel) : heading_prog_.stop(vessel);
    }

    if (IsActive(active_, Progs::HoldHeading)) heading_prog_.step(vessel, simt, simdt, mjd);

    prev_active_ = active_;
}

bool AP::AutoPilot::handle_load_state(bco::vessel& vessel, const std::string& line)
{
    return false;
}

std::string AP::AutoPilot::handle_save_state(bco::vessel& vessel)
{
    return std::string();
}

void AP::AutoPilot::set_autopilot(Progs apid, bool state)
{
    active_ = (state) ? active_ | apid : active_ & ~apid;
}
