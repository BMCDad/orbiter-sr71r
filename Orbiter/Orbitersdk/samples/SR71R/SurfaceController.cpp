//	SurfaceController - SR-71r Orbiter Addon
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

#include "SurfaceController.h"
#include "Orbitersdk.h"

#include "SR71r_mesh.h"

SurfaceController::SurfaceController(bco::Vessel& Vessel, bco::HydraulicProvider& apu) :
	  vessel_(Vessel)
	, apu_(apu)
{
}

void SurfaceController::HandlePostStep(bco::Vessel& Vessel, double simt, double simdt, double mjd)
{
	// Determine if our hydro state has change, enable or disable controls.
	auto hydPress = apu_.Level();
	if (hydPress != prevHydraulicState_)
	{
		if (prevHydraulicState_ == 0.0)
		{
			EnableControls();
		}
		else
		{
			DisableControls();
		}

		prevHydraulicState_ = hydPress;
	}
}

void SurfaceController::HandleSetClassCaps(bco::Vessel& Vessel)
{
	// Left Aileron Animation
	static UINT groupLeftAileron = bm::main::LeftOuterElevon_id;
	static VECTOR3 leftAileronAxis = bm::main::AileronAxisPO_loc - bm::main::AileronAxisPI_loc;
	static VECTOR3 leftElevatorAxis = bm::main::AileronAxisPI_loc - bm::main::AileronAxisPO_loc;

	normalise(leftAileronAxis);
	static MGROUP_ROTATE animGroupLeftAileron(
		0,
		&groupLeftAileron,
		1,
		bm::main::AileronAxisPI_loc,
		leftAileronAxis,
		AILERON_RANGE);

	normalise(leftElevatorAxis);
	static MGROUP_ROTATE animGroupLeftElevator(
		0,
		&groupLeftAileron,
		1,
		bm::main::AileronAxisPI_loc,
		leftElevatorAxis,
		AILERON_RANGE);

	anim_left_aileron_ = Vessel.CreateAnimation(0.5);
	anim_left_elevator_ = Vessel.CreateAnimation(0.5);

	Vessel.AddAnimationComponent(anim_left_aileron_, 0, 1, &animGroupLeftAileron);
	Vessel.AddAnimationComponent(anim_left_elevator_, 0, 1, &animGroupLeftElevator);


	// Right Aileron Animation
	static UINT groupRightAileron[1] = { bm::main::RightOuterElevon_id };
	static VECTOR3 rightAileronAxis = bm::main::AileronAxisSO_loc - bm::main::AileronAxisSI_loc;

	normalise(rightAileronAxis);
	static MGROUP_ROTATE animGroupRightAileron(
		0,
		groupRightAileron,
		1,
		bm::main::AileronAxisSI_loc,
		rightAileronAxis,
		AILERON_RANGE);

	static MGROUP_ROTATE animGroupRightElevator(
		0,
		groupRightAileron,
		1,
		bm::main::AileronAxisSI_loc,
		rightAileronAxis,
		AILERON_RANGE);

	anim_right_aileron_ = Vessel.CreateAnimation(0.5);
	anim_right_elevator_ = Vessel.CreateAnimation(0.5);

	Vessel.AddAnimationComponent(anim_right_aileron_, 0, 1, &animGroupRightAileron);
	Vessel.AddAnimationComponent(anim_right_elevator_, 0, 1, &animGroupRightElevator);


	// Left Rudder Animation
	static UINT groupLeftRudder = bm::main::LeftRudder_id;
	static VECTOR3 leftRudderAxis = bm::main::RudderAxisPB_loc - bm::main::RudderAxisPT_loc;

	normalise(leftRudderAxis);
	static MGROUP_ROTATE animGroupLeftRudder(
		0,
		&groupLeftRudder,
		1,
		bm::main::RudderAxisPB_loc,
		leftRudderAxis,
		AILERON_RANGE);


	// Right Rudder Animation
	static UINT groupRightRudder = bm::main::RightRudder_id;
	static VECTOR3 rightRudderAxis = bm::main::RudderAxisSB_loc - bm::main::RudderAxisST_loc;

	normalise(rightRudderAxis);
	static MGROUP_ROTATE animGroupRightRudder(
		0,
		&groupRightRudder,
		1,
		bm::main::RudderAxisSB_loc,
		rightRudderAxis,
		AILERON_RANGE);

	// Control surface animation.
	anim_left_rudder_ = Vessel.CreateAnimation(0.5);
	anim_right_rudder_ = Vessel.CreateAnimation(0.5);

	Vessel.AddAnimationComponent(anim_left_rudder_, 0, 1, &animGroupLeftRudder);
	Vessel.AddAnimationComponent(anim_right_rudder_, 0, 1, &animGroupRightRudder);
}

void SurfaceController::SetRudderLevel(double Level)
{
	vessel_.SetControlSurfaceLevel(AIRCTRL_RUDDER, Level);
}

void SurfaceController::EnableControls()
{
	// Aileron control : bank left/right.  For this we will just use the outer elevon area.
	ctrlSurfLeftAileron_ = vessel_.CreateControlSurface3(AIRCTRL_AILERON, OutboardElevonArea / 2, dClOutboard, _V(-5.0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_left_aileron_);
	ctrlSurfRightAileron_ = vessel_.CreateControlSurface3(AIRCTRL_AILERON, OutboardElevonArea / 2, dClOutboard, _V(5.0, 0, -8.0), AIRCTRL_AXIS_AUTO, 2.0, anim_right_aileron_);

	// Elevator control : pitch up/down.  For this we use the combined inner and outer area.
	auto fullArea = OutboardElevonArea + InboardElevonArea;
	auto fulldc = dClInboard + dClOutboard;
	ctrlSurfLeftElevator_ = vessel_.CreateControlSurface3(AIRCTRL_ELEVATOR, fullArea, fulldc, _V(-7.0, 0, -10.0), AIRCTRL_AXIS_AUTO, 1.0, anim_left_elevator_);
	ctrlSurfRightElevator_ = vessel_.CreateControlSurface3(AIRCTRL_ELEVATOR, fullArea, fulldc, _V(7.0, 0, -10.0), AIRCTRL_AXIS_AUTO, 1.0, anim_right_elevator_);

	// Rudder contro : yaw.
	ctrlSurfLeftRudder_ = vessel_.CreateControlSurface3(AIRCTRL_RUDDER, RudderArea, dClRudder, _V(0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_left_rudder_);
	ctrlSurfRightRudder_ = vessel_.CreateControlSurface3(AIRCTRL_RUDDER, RudderArea, dClRudder, _V(0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_right_rudder_);

	// Trim : SR-71 does not have trim tabs, or flaps.  We will use the inBoard area and generall as smaller dCl to provide finer control.
	ctrlSurfLeftTrim_ = vessel_.CreateControlSurface3(AIRCTRL_ELEVATORTRIM, InboardElevonArea, dClInboard, _V(-7.0, 0, -10.0), AIRCTRL_AXIS_XPOS, 1.0, anim_left_elevator_);
	ctrlSurfRightTrim_ = vessel_.CreateControlSurface3(AIRCTRL_ELEVATORTRIM, InboardElevonArea, dClInboard, _V(7.0, 0, -10.0), AIRCTRL_AXIS_XPOS, 1.0, anim_right_elevator_);
}

void SurfaceController::DisableControls()
{
	vessel_.DelControlSurface(ctrlSurfLeftAileron_);
	vessel_.DelControlSurface(ctrlSurfRightAileron_);
	vessel_.DelControlSurface(ctrlSurfLeftElevator_);
	vessel_.DelControlSurface(ctrlSurfRightElevator_);
	vessel_.DelControlSurface(ctrlSurfLeftRudder_);
	vessel_.DelControlSurface(ctrlSurfRightRudder_);
	vessel_.DelControlSurface(ctrlSurfLeftTrim_);
	vessel_.DelControlSurface(ctrlSurfRightTrim_);
}
