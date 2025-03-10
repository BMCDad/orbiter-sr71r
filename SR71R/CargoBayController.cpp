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

#include "..\bc_orbiter\Tools.h"

#include "Orbitersdk.h"
#include "CargoBayController.h"
#include "SR71r_mesh.h"

CargoBayController::CargoBayController(bco::power_provider& pwr, bco::vessel& vessel) :
    power_(pwr)
{
    power_.attach_consumer(this);
    vessel.AddControl(&switchOpen_);
    vessel.AddControl(&switchPower_);
    vessel.AddControl(&status_);
}

void CargoBayController::handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd)
{
	if (IsPowered()) {
        animCargoBayDoors_.Step(switchOpen_.is_on() ? 1.0 : 0.0, simdt);
	}

    auto status = bco::status_display::status::off;
    if (power_.volts_available() > MIN_VOLTS) {
        if ((animCargoBayDoors_.GetState() > 0.0) && (animCargoBayDoors_.GetState() < 1.0)) {
            status = bco::status_display::status::warn;
        }
        else {
            if (animCargoBayDoors_.GetState() == 1.0) {
                status = bco::status_display::status::on;
            }
        }
    }
    status_.set_state(status);
}

bool CargoBayController::handle_load_state(bco::vessel& vessel, const std::string& line)
{
    std::istringstream in(line);
    in >> switchPower_ >> switchOpen_ >> animCargoBayDoors_;
    vessel.SetAnimationState(animCargoBayDoors_);
    return true;
}

std::string CargoBayController::handle_save_state(bco::vessel& vessel)
{
    std::ostringstream os;
    os << switchPower_ << " " << switchOpen_ << " " << animCargoBayDoors_;
    return os.str();
}

void CargoBayController::handle_set_class_caps(bco::vessel& vessel)
{
    auto mIdx = vessel.GetMainMeshIndex();

    auto id = vessel.CreateVesselAnimation(&animCargoBayDoors_, 0.01);
    animCargoBayDoors_.VesselId(id);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoLeftFront_);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoRightFront_);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoLeftMain_);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoRightMain_);
}