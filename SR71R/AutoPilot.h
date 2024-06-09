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
#include "../bc_orbiter/Component.h"
#include "../bc_orbiter/control.h"
#include "../bc_orbiter/simple_event.h"
#include "../bc_orbiter/status_display.h"

#include "VesselControl.h"

#include "SR71r_mesh.h"

class VESSEL;

namespace bco = bc_orbiter;

namespace AP {

    enum class Progs
    {
        None = 0,
        HoldHeading = 1
    };

    // Logical OR - combine flags.
    constexpr Progs operator|(Progs l, Progs r)
    {
        return static_cast<Progs>(static_cast<int>(l) | static_cast<int>(r));
    }

    // Logical AND - test for the existance of a flag.
    constexpr Progs operator&(Progs l, Progs r)
    {
        return static_cast<Progs>(static_cast<int>(l) & static_cast<int>(r));
    }

    // Logical NOT - remove flag.
    constexpr Progs operator~(const Progs l)
    {
        return static_cast<Progs>(~static_cast<int>(l));
    }


    constexpr bool IsActive(Progs a, Progs b) { return (bool)(a & b); }

    class AutoPilot :
        public bco::vessel_component,
        public bco::set_class_caps,
        public bco::post_step,
        public bco::manage_state
    {
    public:
        AutoPilot(bco::vessel& vessel);

        // set_class_caps
        void handle_set_class_caps(bco::vessel& vessel) override;

        // post_step
        void handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd) override;

        // manage_state
        bool handle_load_state(bco::vessel& vessel, const std::string& line) override;
        std::string handle_save_state(bco::vessel& vessel) override;

        void set_autopilot(Progs apid, bool state);

    private:

        Progs active_{ Progs::None };
        Progs prev_active_{ Progs::None };

        HoldHeadingProgram  heading_prog_;
    };
}