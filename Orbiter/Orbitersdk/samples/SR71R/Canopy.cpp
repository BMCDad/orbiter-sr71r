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

Canopy::Canopy(bco::power_provider& pwr) :
    power_(pwr)
{ 
    power_.attach_consumer(this);
}

void Canopy::handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd)
{
    if (IsPowered()) {
        animCanopy_.Step(switchOpen_.is_on() ? 1.0 : 0.0, simdt);
    }

    status_.set_state(
        IsPowered() 
        ?   CanopyIsMoving() 
            ?   bco::status_display::status::warn 
            :   switchOpen_.is_on() 
                ?   bco::status_display::status::on 
                :   bco::status_display::status::off 
        : bco::status_display::status::off);
}

bool Canopy::handle_load_state(const std::string& line)
{
    std::istringstream in(line);

    if (in >> switchPower_ >> switchOpen_ >> animCanopy_) {
        return true;
    }
    else {
        return false;
    }
}

std::string Canopy::handle_save_state()
{
    std::ostringstream os;
    os << switchPower_ << " " << switchOpen_ << " " << animCanopy_;
    return os.str();
}


void Canopy::handle_set_class_caps(bco::BaseVessel& vessel)
{
    auto vcIdx = vessel.GetVCMeshIndex();
    auto mIdx = vessel.GetMainMeshIndex();

    auto idAnim = vessel.CreateVesselAnimation(&animCanopy_, 0.2);
    vessel.AddVesselAnimationComponent(idAnim, vcIdx, &gpCanopyVC_);
    vessel.AddVesselAnimationComponent(idAnim, mIdx, &gpCanopy_);

    vessel.AddControl(&switchOpen_);
    vessel.AddControl(&switchPower_);
    vessel.AddControl(&status_);
}