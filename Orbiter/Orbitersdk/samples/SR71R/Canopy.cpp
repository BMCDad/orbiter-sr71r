//	Canopy - SR-71r Orbiter Addon
//	Copyright(C) 2017  Blake Christensen
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

#include "bc_orbiter/Tools.h"
#include "bc_orbiter/status_display.h"

#include "Orbitersdk.h"
#include "Canopy.h"
#include "SR71r_mesh.h"

Canopy::Canopy(bco::power_provider& pwr, bco::vessel& vessel) :
    power_(pwr)
{ 
    power_.attach_consumer(this);
    vessel.AddControl(&switchOpen_);
    vessel.AddControl(&switchPower_);
    vessel.AddControl(&status_);
}

void Canopy::handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd)
{
    if (IsPowered()) {
        animCanopy_.Step(switchOpen_.is_on() ? 1.0 : 0.0, simdt);
    }
    /*
        off     - no power OR closed
        warn    - yes power AND is moving
        on      - yes power AND open
    */
    auto status = bco::status_display::status::off;
    if (power_.volts_available() > MIN_VOLTS) {
        if ((animCanopy_.GetState() > 0.0) && (animCanopy_.GetState() < 1.0)) {
            status = bco::status_display::status::warn;
        }
        else {
            if (animCanopy_.GetState() == 1.0) {
                status = bco::status_display::status::on;
            }
        }
    }
    status_.set_state(status);
}

bool Canopy::handle_load_state(bco::vessel& vessel, const std::string& line)
{
    std::istringstream in(line);
    in >> switchPower_;
    in >> switchOpen_;
    in >> animCanopy_;
    vessel.SetAnimationState(animCanopy_);
    return true;
}

std::string Canopy::handle_save_state(bco::vessel& vessel)
{
    std::ostringstream os;
    os << switchPower_ << " " << switchOpen_ << " " << animCanopy_;
    return os.str();
}


void Canopy::handle_set_class_caps(bco::vessel& vessel)
{
    auto vcIdx = vessel.GetVCMeshIndex();
    auto mIdx = vessel.GetMainMeshIndex();

    auto idAnim = vessel.CreateVesselAnimation(&animCanopy_, 0.2);
    animCanopy_.VesselId(idAnim);
    vessel.AddVesselAnimationComponent(idAnim, vcIdx, &gpCanopyVC_);
    vessel.AddVesselAnimationComponent(idAnim, mIdx, &gpCanopy_);
}