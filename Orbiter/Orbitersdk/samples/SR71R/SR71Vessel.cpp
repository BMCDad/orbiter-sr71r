//	SR-71r Orbiter Addon
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

#include "SR71Vessel.h"
#include "LiftCoeff.h"
#include "ShipMets.h"

SR71Vessel::SR71Vessel(OBJHANDLE hvessel, int flightmodel) : 
	bco::BaseVessel(hvessel, flightmodel),
	meshVirtualCockpit_(nullptr),
	navModes_(              *this),
	slotHydraulicLevel_([&](double v) { UpdateHydraulicLevel(v); }),
	ctrlSurfLeftAileron_(nullptr),
	ctrlSurfRightAileron_(nullptr),
	ctrlSurfLeftElevator_(nullptr),
	ctrlSurfRightElevator_(nullptr),
	ctrlSurfLeftRudder_(nullptr),
	ctrlSurfRightRudder_(nullptr)
{
	AddComponent(&avionics_);
	AddComponent(&airBrake_);
	AddComponent(&airspeed_);
	AddComponent(&altimeter_);
	AddComponent(&apu_);
	AddComponent(&beacon_);
	AddComponent(&canopy_);
	AddComponent(&cargobay_);
	AddComponent(&clock_);
	AddComponent(&fuelCell_);
	AddComponent(&headsUpDisplay_);
	AddComponent(&hoverEngines_);
	AddComponent(&hydrogenTank_);
	AddComponent(&hsi_);
	AddComponent(&landingGear_);
	AddComponent(&navLight_);
	AddComponent(&oxygenTank_);
	AddComponent(&propulsion_);
	AddComponent(&powerSystem_);
	AddComponent(&retroEngines_);
	AddComponent(&strobe_);

	AddComponent(&mfdLeft_);
	AddComponent(&mfdRight_);


	bco::connect(avionics_.IsAeroActiveSignal(), altimeter_.EnabledSlot());
	bco::connect(avionics_.IsAeroModeAtmoSignal(), altimeter_.AvionicsModeSlot());
	bco::connect(avionics_.IsAeroActiveSignal(), airspeed_.EnabledSlot());
	bco::connect(avionics_.IsAeroModeAtmoSignal(), airspeed_.AvionicsModeSlot());
	bco::connect(avionics_.IsAeroActiveSignal(), hsi_.EnabledSlot());
	bco::connect(avionics_.IsAeroModeAtmoSignal(), hsi_.AvionicsModeSlot());

	// Fuel cell					// A signal can drive more then one slot
	bco::connect( fuelCell_.AvailablePowerSignal(),			powerSystem_.FuelCellAvailablePowerSlot());

	// Airbrake
	bco::connect( apu_.HydroPressSignal(),					airBrake_.HydraulicPressSlot());	// still need this

	// Landing Gear
	bco::connect( apu_.HydroPressSignal(),					landingGear_.HydraulicPressSlot());	// still need this

	bco::connect( propulsion_.MainFuelLevelSignal(),		apu_.FuelLevelSlot());

	// RCS
	bco::connect( avionics_.IsAeroActiveSignal(),			rcs_.IsAeroActiveSlot());

	// ...which in turn drive the HSI course and heading
	bco::connect( avionics_.SetCourseSignal(),				hsi_.SetCourseSlot());
	bco::connect( avionics_.SetHeadingSignal(),				hsi_.SetHeadingSlot());
			// ...which drives the course and heading needle and wheels.

	// Internal vessel stuff:
	bco::connect( apu_.HydroPressSignal(),					slotHydraulicLevel_);	 // Get our hydraulic level for surface controls.
}


SR71Vessel::~SR71Vessel()
{
}

void SR71Vessel::SetupAerodynamics()
{
	// Aerodynamics - see notes in ShipMets file.
	CreateAirfoil3(
		LIFT_VERTICAL, 
		_V(0, 0, -0.3), 
		VLiftCoeff, 
		0, 
		VERT_WING_CHORD, 
		VERT_WING_AREA, 
		VERT_WING_AR);

	CreateAirfoil3(
		LIFT_HORIZONTAL, 
		_V(0, 0, -4), 
		HLiftCoeff, 
		0, 
		HORZ_WING_CHORD,
		HORZ_WING_AREA,
		HORZ_WING_AR);
}

void SR71Vessel::SetupSurfaces()
{
	auto vessel = this;

	// Left Aileron Animation
	static UINT groupLeftAileron = bm::main::LeftOuterElevon_id;
	static VECTOR3 leftAileronAxis = bm::main::AileronAxisPO_location - bm::main::AileronAxisPI_location;
	static VECTOR3 leftElevatorAxis = bm::main::AileronAxisPI_location - bm::main::AileronAxisPO_location;

	normalise(leftAileronAxis);
	static MGROUP_ROTATE animGroupLeftAileron(
		0,
		&groupLeftAileron,
		1,
		bm::main::AileronAxisPI_location,
		leftAileronAxis,
		AILERON_RANGE);

	normalise(leftElevatorAxis);
	static MGROUP_ROTATE animGroupLeftElevator(
		0,
		&groupLeftAileron,
		1,
		bm::main::AileronAxisPI_location,
		leftElevatorAxis,
		AILERON_RANGE);

	anim_left_aileron_ = vessel->CreateAnimation(0.5);
	anim_left_elevator_ = vessel->CreateAnimation(0.5);

	vessel->AddAnimationComponent(anim_left_aileron_, 0, 1, &animGroupLeftAileron);
	vessel->AddAnimationComponent(anim_left_elevator_, 0, 1, &animGroupLeftElevator);


	// Right Aileron Animation
	static UINT groupRightAileron[1] = { bm::main::RightOuterElevon_id };
	static VECTOR3 rightAileronAxis = bm::main::AileronAxisSO_location - bm::main::AileronAxisSI_location;

	normalise(rightAileronAxis);
	static MGROUP_ROTATE animGroupRightAileron(
		0,
		groupRightAileron,
		1,
		bm::main::AileronAxisSI_location,
		rightAileronAxis,
		AILERON_RANGE);

	static MGROUP_ROTATE animGroupRightElevator(
		0,
		groupRightAileron,
		1,
		bm::main::AileronAxisSI_location,
		rightAileronAxis,
		AILERON_RANGE);

	anim_right_aileron_ = vessel->CreateAnimation(0.5);
	anim_right_elevator_ = vessel->CreateAnimation(0.5);

	vessel->AddAnimationComponent(anim_right_aileron_, 0, 1, &animGroupRightAileron);
	vessel->AddAnimationComponent(anim_right_elevator_, 0, 1, &animGroupRightElevator);


	// Left Rudder Animation
	static UINT groupLeftRudder = bm::main::LeftRudder_id;
	static VECTOR3 leftRudderAxis = bm::main::RudderAxisPB_location - bm::main::RudderAxisPT_location;

	normalise(leftRudderAxis);
	static MGROUP_ROTATE animGroupLeftRudder(
		0,
		&groupLeftRudder,
		1,
		bm::main::RudderAxisPB_location,
		leftRudderAxis,
		AILERON_RANGE);


	// Right Rudder Animation
	static UINT groupRightRudder = bm::main::RightRudder_id;
	static VECTOR3 rightRudderAxis = bm::main::RudderAxisSB_location - bm::main::RudderAxisST_location;

	normalise(rightRudderAxis);
	static MGROUP_ROTATE animGroupRightRudder(
		0,
		&groupRightRudder,
		1,
		bm::main::RudderAxisSB_location,
		rightRudderAxis,
		AILERON_RANGE);

	// control surface animation.
	anim_left_rudder_ = vessel->CreateAnimation(0.5);
	anim_right_rudder_ = vessel->CreateAnimation(0.5);

	vessel->AddAnimationComponent(anim_left_rudder_, 0, 1, &animGroupLeftRudder);
	vessel->AddAnimationComponent(anim_right_rudder_, 0, 1, &animGroupRightRudder);
}
