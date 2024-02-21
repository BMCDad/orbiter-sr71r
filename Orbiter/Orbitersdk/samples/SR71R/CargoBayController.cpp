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

CargoBayController::CargoBayController(bco::PowerProvider& pwr, bco::Vessel& Vessel) :
    power_(pwr)
{
    power_.AttachConsumer(this);
    Vessel.AddControl(&switchOpen_);
    Vessel.AddControl(&switchPower_);
    Vessel.AddControl(&status_);
}

void CargoBayController::HandlePostStep(bco::Vessel& Vessel, double simt, double simdt, double mjd)
{
	if (IsPowered()) {
        animCargoBayDoors_.Step(switchOpen_.IsOn() ? 1.0 : 0.0, simdt);
	}

    auto status = bco::status_display::status::off;
    if (power_.VoltsAvailable() > MIN_VOLTS) {
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

bool CargoBayController::HandleLoadState(bco::Vessel& Vessel, const std::string& line)
{
    std::istringstream in(line);
    in >> switchPower_ >> switchOpen_ >> animCargoBayDoors_;
    Vessel.SetAnimationState(animCargoBayDoors_);
    return true;
}

std::string CargoBayController::HandleSaveState(bco::Vessel& Vessel)
{
    std::ostringstream os;
    os << switchPower_ << " " << switchOpen_ << " " << animCargoBayDoors_;
    return os.str();
}

void CargoBayController::HandleSetClassCaps(bco::Vessel& Vessel)
{
    auto mIdx = Vessel.GetMainMeshIndex();

    auto id = Vessel.CreateVesselAnimation(&animCargoBayDoors_, 0.01);
    animCargoBayDoors_.VesselId(id);
    Vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoLeftFront_);
    Vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoRightFront_);
    Vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoLeftMain_);
    Vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoRightMain_);
}