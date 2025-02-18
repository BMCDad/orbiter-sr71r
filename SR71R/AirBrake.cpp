//	AirBrake - SR-71r Orbiter Addon
//	Copyright(C) 2015  Blake Christensen
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

#include "AirBrake.h"
#include "SR71r_mesh.h"

AirBrake::AirBrake(bco::Vessel& vessel, bco::HydraulicProvider& apu) :
	dragFactor_(0.0),
	apu_(apu)
{
	vessel.AddControl(&btnDecreaseAirbrake_);
	vessel.AddControl(&btnIncreaseAirbrake_);
	vessel.AddControl(&status_);

	btnIncreaseAirbrake_.attach([&]() { IncreaseDrag(); });
	btnDecreaseAirbrake_.attach([&]() { DecreaseDrag(); });
}

void AirBrake::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd)
{
    // Note:  The animAirBrake can only move if there is hydraulic power, that
    // is the actual air brake Animation.  The animAirBrakeHandle_ is the air brake
    // handle in the cockpit and can move regardless of power, therefore it must
    // always get a piece of the time step.

    if (apu_.Level() > 0.8)
    {
        animAirBrake_.Step(position_, simdt);
    }

    animBrakeSwitch_.Step(position_, simdt);

    // Update drag.
    dragFactor_ = animAirBrake_.GetState();
    //	sprintf(oapiDebugString(), "air brake: %+4.2f", dragFactor_);

       // This needs to be put into a switch statement eventually
    bco::TranslateMesh(vessel.GetpanelMeshHandle(0), bm::pnl::pnlAirBrake_id, bm::pnl::pnlAirBrake_vrt, sTrans * animBrakeSwitch_.GetState());

    status_.set_state(vessel,
        dragFactor_ > 0.05
        ? cmn::status::warn
        : cmn::status::off);
}

bool AirBrake::HandleLoadState(bco::Vessel& vessel, const std::string& line)
{
	// [a b] : a: air brake switch position.   b: air brake actual position (they can differ)

	std::istringstream in(line);
	in >> position_ >> animAirBrake_;
	
	vessel.SetAnimationState(animAirBrake_);
	return true;
}

std::string AirBrake::HandleSaveState(bco::Vessel& vessel)
{
	std::ostringstream os;
	os << position_ << " " << animAirBrake_;
	return os.str();
}

void AirBrake::HandleSetClassCaps(bco::Vessel& vessel)
{
    // Setup VC Animation
    auto vcIdx = vessel.GetVCMeshIndex();
    auto aid = vessel.CreateVesselAnimation(animBrakeSwitch_);
    animBrakeSurface_.VesselId(aid);
    vessel.AddVesselAnimationComponent(aid, vcIdx, &gpBrakeHandle_);

    // Setup external Animation   
    auto exIdx = vessel.GetMainMeshIndex();
    aid = vessel.CreateVesselAnimation(animAirBrake_);
    vessel.AddVesselAnimationComponent(aid, exIdx, &gpLeftTop_);
    vessel.AddVesselAnimationComponent(aid, exIdx, &gpLeftBottom_);
    vessel.AddVesselAnimationComponent(aid, exIdx, &gpRightTop_);
    vessel.AddVesselAnimationComponent(aid, exIdx, &gpRightBottom_);
    animAirBrake_.VesselId(aid);

    // Setup drag.
    vessel.CreateVariableDragElement(animAirBrake_.GetStatePtr(), 10.0, bm::main::BrakeDragPoint_loc);
}
