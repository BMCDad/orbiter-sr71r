//	CarboBayController - SR-71r Orbiter Addon
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

#include "StdAfx.h"

#include "bc_orbiter\Tools.h"

#include "Orbitersdk.h"
#include "CargoBayController.h"
#include "SR71r_mesh.h"

CargoBayController::CargoBayController(bco::power_provider& pwr) :
    power_(pwr)
{
    power_.attach_consumer(this);
}

void CargoBayController::handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd)
{
	if (IsPowered()) {
        animCargoBayDoors_.Step(switchOpen_.is_on() ? 1.0 : 0.0, simdt);
	}

    status_.set_state(
        IsPowered()
        ?   IsMoving()
            ?   bco::status_display::status::warn
            :   switchOpen_.is_on()
                ?   bco::status_display::status::on
                :   bco::status_display::status::off
        :   bco::status_display::status::off);

}

bool CargoBayController::handle_load_state(const std::string& line)
{
    std::istringstream in(line);
    in >> switchPower_ >> switchOpen_ >> animCargoBayDoors_;
    return true;
}

std::string CargoBayController::handle_save_state()
{
    std::ostringstream os;
    os << switchPower_ << " " << switchOpen_ << " " << animCargoBayDoors_;
    return os.str();
}

void CargoBayController::handle_set_class_caps(bco::BaseVessel& vessel)
{
    vessel.AddControl(&switchOpen_);
    vessel.AddControl(&switchPower_);
    vessel.AddControl(&status_);

    auto mIdx = vessel.GetMainMeshIndex();

    auto id = vessel.CreateVesselAnimation(&animCargoBayDoors_, 0.01);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoLeftFront_);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoRightFront_);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoLeftMain_);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoRightMain_);
}