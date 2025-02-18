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

CargoBayController::CargoBayController(bco::PowerProvider& pwr, bco::Vessel& vessel) :
    power_(pwr)
{
    power_.AttachConsumer(this);
    vessel.AddControl(&switchOpen_);
    vessel.AddControl(&switchPower_);
    vessel.AddControl(&status_);
}

void CargoBayController::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd)
{
    if (IsPowered()) {
        animCargoBayDoors_.Update(vessel, switchOpen_.IsOn() ? 1.0 : 0.0, simdt);
    }

    auto status = cmn::status::off;
    if (power_.VoltsAvailable() > MIN_VOLTS) {
        if ((animCargoBayDoors_.GetState() > 0.0) && (animCargoBayDoors_.GetState() < 1.0)) {
            status = cmn::status::warn;
        }
        else {
            if (animCargoBayDoors_.GetState() == 1.0) {
                status = cmn::status::on;
            }
        }
    }
    status_.set_state(vessel, status);
}

bool CargoBayController::HandleLoadState(bco::Vessel& vessel, const std::string& line)
{
    std::istringstream in(line);
    in >> switchPower_ >> switchOpen_ >> animCargoBayDoors_;
    vessel.SetAnimationState(animCargoBayDoors_);
    return true;
}

std::string CargoBayController::HandleSaveState(bco::Vessel& vessel)
{
    std::ostringstream os;
    os << switchPower_ << " " << switchOpen_ << " " << animCargoBayDoors_;
    return os.str();
}

void CargoBayController::HandleSetClassCaps(bco::Vessel& vessel)
{
    auto mIdx = vessel.GetMainMeshIndex();

    auto id = vessel.CreateVesselAnimation(animCargoBayDoors_);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoLeftFront_);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoRightFront_);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoLeftMain_);
    vessel.AddVesselAnimationComponent(id, mIdx, &gpCargoRightMain_);
}