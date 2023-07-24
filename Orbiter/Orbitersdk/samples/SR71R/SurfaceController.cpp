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

SurfaceController::SurfaceController(bco::vessel& vessel) :
	prevHydraulicState_(0.0),
	ctrlSurfLeftAileron_(nullptr),
	ctrlSurfRightAileron_(nullptr),
	ctrlSurfLeftElevator_(nullptr),
	ctrlSurfRightElevator_(nullptr),
	ctrlSurfLeftRudder_(nullptr),
	ctrlSurfRightRudder_(nullptr),
	apu_(nullptr)
{
}

void SurfaceController::handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd)
{
	if (nullptr == apu_)
	{
		return;
	}

	//// Determine if our hydro state has change, enable or disable controls.
	//auto hydPress = apu_->GetHydraulicLevel();
	//if (hydPress != prevHydraulicState_)
	//{
	//	if (prevHydraulicState_ == 0.0)
	//	{
	//		EnableControls();
	//	}
	//	else
	//	{
	//		DisableControls();
	//	}

	//	prevHydraulicState_ = hydPress;
	//}
}

void SurfaceController::handle_set_class_caps(bco::vessel& vessel)
{
	//// Left Aileron Animation
	//static UINT groupLeftAileron = bt_mesh::SR71r::LeftOuterElevon_id;
	//static VECTOR3 leftAileronAxis = bt_mesh::SR71r::AileronAxisPO_location - bt_mesh::SR71r::AileronAxisPI_location;
	//static VECTOR3 leftElevatorAxis = bt_mesh::SR71r::AileronAxisPI_location - bt_mesh::SR71r::AileronAxisPO_location;

	//normalise(leftAileronAxis);
	//static MGROUP_ROTATE animGroupLeftAileron(
	//	0,
	//	&groupLeftAileron,
	//	1,
	//	bt_mesh::SR71r::AileronAxisPI_location,
	//	leftAileronAxis,
	//	AILERON_RANGE);

	//normalise(leftElevatorAxis);
	//static MGROUP_ROTATE animGroupLeftElevator(
	//	0,
	//	&groupLeftAileron,
	//	1,
	//	bt_mesh::SR71r::AileronAxisPI_location,
	//	leftElevatorAxis,
	//	AILERON_RANGE);

	//anim_left_aileron_ = vessel.CreateAnimation(0.5);
	//anim_left_elevator_ = vessel.CreateAnimation(0.5);

	//vessel.AddAnimationComponent(anim_left_aileron_, 0, 1, &animGroupLeftAileron);
	//vessel.AddAnimationComponent(anim_left_elevator_, 0, 1, &animGroupLeftElevator);


	//// Right Aileron Animation
	//static UINT groupRightAileron[1] = { bt_mesh::SR71r::RightOuterElevon_id };
	//static VECTOR3 rightAileronAxis = bt_mesh::SR71r::AileronAxisSO_location - bt_mesh::SR71r::AileronAxisSI_location;

	//normalise(rightAileronAxis);
	//static MGROUP_ROTATE animGroupRightAileron(
	//	0,
	//	groupRightAileron,
	//	1,
	//	bt_mesh::SR71r::AileronAxisSI_location,
	//	rightAileronAxis,
	//	AILERON_RANGE);

	//static MGROUP_ROTATE animGroupRightElevator(
	//	0,
	//	groupRightAileron,
	//	1,
	//	bt_mesh::SR71r::AileronAxisSI_location,
	//	rightAileronAxis,
	//	AILERON_RANGE);

	//anim_right_aileron_ = vessel->CreateAnimation(0.5);
	//anim_right_elevator_ = vessel->CreateAnimation(0.5);

	//vessel->AddAnimationComponent(anim_right_aileron_, 0, 1, &animGroupRightAileron);
	//vessel->AddAnimationComponent(anim_right_elevator_, 0, 1, &animGroupRightElevator);


	//// Left Rudder Animation
	//static UINT groupLeftRudder = bt_mesh::SR71r::LeftRudder_id;
	//static VECTOR3 leftRudderAxis = bt_mesh::SR71r::RudderAxisPB_location - bt_mesh::SR71r::RudderAxisPT_location;

	//normalise(leftRudderAxis);
	//static MGROUP_ROTATE animGroupLeftRudder(
	//	0,
	//	&groupLeftRudder,
	//	1,
	//	bt_mesh::SR71r::RudderAxisPB_location,
	//	leftRudderAxis,
	//	AILERON_RANGE);


	//// Right Rudder Animation
	//static UINT groupRightRudder = bt_mesh::SR71r::RightRudder_id;
	//static VECTOR3 rightRudderAxis = bt_mesh::SR71r::RudderAxisSB_location - bt_mesh::SR71r::RudderAxisST_location;

	//normalise(rightRudderAxis);
	//static MGROUP_ROTATE animGroupRightRudder(
	//	0,
	//	&groupRightRudder,
	//	1,
	//	bt_mesh::SR71r::RudderAxisSB_location,
	//	rightRudderAxis,
	//	AILERON_RANGE);

	//// Control surface animation.
	//anim_left_rudder_ = vessel->CreateAnimation(0.5);
	//anim_right_rudder_ = vessel->CreateAnimation(0.5);

	//vessel->AddAnimationComponent(anim_left_rudder_, 0, 1, &animGroupLeftRudder);
	//vessel->AddAnimationComponent(anim_right_rudder_, 0, 1, &animGroupRightRudder);
}

void SurfaceController::SetAileronLevel(double level)
{
//	GetBaseVessel()->SetControlSurfaceLevel(AIRCTRL_AILERON, level);
}

void SurfaceController::SetRudderLevel(double level)
{
//	GetBaseVessel()->SetControlSurfaceLevel(AIRCTRL_RUDDER, level);
}

void SurfaceController::SetElevatorLevel(double level)
{
//	GetBaseVessel()->SetControlSurfaceLevel(AIRCTRL_ELEVATOR, level);
}

void SurfaceController::EnableControls()
{
	//auto vessel = GetBaseVessel();
	//// Aileron control : bank left/right.  For this we will just use the outer elevon area.
	//ctrlSurfLeftAileron_ = vessel->CreateControlSurface3(AIRCTRL_AILERON, OutboardElevonArea / 2, dClOutboard, _V(-5.0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_left_aileron_);
	//ctrlSurfRightAileron_ = vessel->CreateControlSurface3(AIRCTRL_AILERON, OutboardElevonArea / 2, dClOutboard, _V(5.0, 0, -8.0), AIRCTRL_AXIS_AUTO, 2.0, anim_right_aileron_);

	//// Elevator control : pitch up/down.  For this we use the combined inner and outer area.
	//auto fullArea = OutboardElevonArea + InboardElevonArea;
	//auto fulldc = dClInboard + dClOutboard;
	//ctrlSurfLeftElevator_ = vessel->CreateControlSurface3(AIRCTRL_ELEVATOR, fullArea, fulldc, _V(-5.0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_left_elevator_);
	//ctrlSurfRightElevator_ = vessel->CreateControlSurface3(AIRCTRL_ELEVATOR, fullArea, fulldc, _V(5.0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_right_elevator_);

	//// Rudder contro : yaw.
	//ctrlSurfLeftRudder_ = vessel->CreateControlSurface3(AIRCTRL_RUDDER, RudderArea, dClRudder, _V(0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_left_rudder_);
	//ctrlSurfRightRudder_ = vessel->CreateControlSurface3(AIRCTRL_RUDDER, RudderArea, dClRudder, _V(0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_right_rudder_);

	//// Trim : SR-71 does not have trim tabs, or flaps.  We will use the inBoard area and generall as smaller dCl to provide finer control.
	//ctrlSurfLeftTrim_ = vessel->CreateControlSurface3(AIRCTRL_ELEVATORTRIM, InboardElevonArea, dClInboard, _V(-5.0, 0, -8.0), AIRCTRL_AXIS_XPOS, 1.0, anim_left_elevator_);
	//ctrlSurfRightTrim_ = vessel->CreateControlSurface3(AIRCTRL_ELEVATORTRIM, InboardElevonArea, dClInboard, _V(5.0, 0, -8.0), AIRCTRL_AXIS_XPOS, 1.0, anim_right_elevator_);
}

void SurfaceController::DisableControls()
{
	//auto vessel = GetBaseVessel();
	//vessel->DelControlSurface(ctrlSurfLeftAileron_);
	//vessel->DelControlSurface(ctrlSurfRightAileron_);
	//vessel->DelControlSurface(ctrlSurfLeftElevator_);
	//vessel->DelControlSurface(ctrlSurfRightElevator_);
	//vessel->DelControlSurface(ctrlSurfLeftRudder_);
	//vessel->DelControlSurface(ctrlSurfRightRudder_);
	//vessel->DelControlSurface(ctrlSurfLeftTrim_);
	//vessel->DelControlSurface(ctrlSurfRightTrim_);
}
