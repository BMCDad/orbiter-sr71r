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

Canopy::Canopy(bco::PowerProvider& pwr, bco::Vessel& Vessel) :
    power_(pwr)
{ 
    power_.AttachConsumer(this);
    Vessel.AddControl(&switchOpen_);
    Vessel.AddControl(&switchPower_);
    Vessel.AddControl(&status_);
}

void Canopy::HandlePostStep(bco::Vessel& Vessel, double simt, double simdt, double mjd)
{
    if (IsPowered()) {
        animCanopy_.Step(switchOpen_.IsOn() ? 1.0 : 0.0, simdt);
    }
    /*
        off     - no power OR closed
        warn    - yes power AND is moving
        on      - yes power AND open
    */
    auto status = bco::status_display::status::off;
    if (power_.VoltsAvailable() > MIN_VOLTS) {
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

bool Canopy::HandleLoadState(bco::Vessel& Vessel, const std::string& line)
{
    std::istringstream in(line);
    in >> switchPower_;
    in >> switchOpen_;
    in >> animCanopy_;
    Vessel.SetAnimationState(animCanopy_);
    return true;
}

std::string Canopy::HandleSaveState(bco::Vessel& Vessel)
{
    std::ostringstream os;
    os << switchPower_ << " " << switchOpen_ << " " << animCanopy_;
    return os.str();
}


void Canopy::HandleSetClassCaps(bco::Vessel& Vessel)
{
    auto vcIdx = Vessel.GetVCMeshIndex();
    auto mIdx = Vessel.GetMainMeshIndex();

    auto idAnim = Vessel.CreateVesselAnimation(&animCanopy_, 0.2);
    animCanopy_.VesselId(idAnim);
    Vessel.AddVesselAnimationComponent(idAnim, vcIdx, &gpCanopyVC_);
    Vessel.AddVesselAnimationComponent(idAnim, mIdx, &gpCanopy_);
}