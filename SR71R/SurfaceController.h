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

#pragma once

#include "Orbitersdk.h"

#include "APU.h"

// REFACTOR NOTE:
// The settings (InboardElevonArea, etc.) should be moved so that this class 
// can be more general in nature and moved into bco.

class VESSEL3;

const float AILERON_RANGE = (float)(20.0 * RAD);
/*	Control settings.
	These settings Control the responsiveness of the flight Control surfaces.  The area
	settings are from the SR-71 manual and should probably be kept as is.  The dCl... settings
	Control the effectiveness of each Control.  These can be tweaked to adjust the handling of the
	Vessel.  To see how these are applied look at the 'EnableControls' method of this class.
*/
const double InboardElevonArea =    3.6;    // From manual, inboard (39sf/3.6ms).
const double dClInboard =           0.6;    // Adjust to impact mostly elevator function.
const double OutboardElevonArea =   4.8;    // From manual, outboard (52.5sf/4.8ms).
const double dClOutboard =          0.2;    // Adjust to impact both elevator and aileron function.
const double RudderArea =           6.4;    // From manual, movable rudder area (70sf)
const double dClRudder =            0.2;    // Adjust to implact rudder function.

namespace bco = bc_orbiter;

/**	SurfaceController
	Manages the Control surface for the airplane.
	Checks for hydraulic pressure and enables Control if present.  If not then all Control
	surfaces are disabled.
*/
class SurfaceController : public bco::VesselComponent
{
public:
    SurfaceController(bco::Vessel& vessel, bco::HydraulicProvider& apu);

    // SetClassCaps
    void HandleSetClassCaps(bco::Vessel& vessel) override;

    // PostStep
    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override;

    void SetRudderLevel(double level);

private:
    bco::Vessel& vessel_;
    void EnableControls();
    void DisableControls();

    double		prevHydraulicState_{ 0.0 };

    int         anim_left_aileron_{ 0 };
    int         anim_left_elevator_{ 0 };
    int         anim_right_aileron_{ 0 };
    int         anim_right_elevator_{ 0 };
    int         anim_left_rudder_{ 0 };
    int         anim_right_rudder_{ 0 };

    CTRLSURFHANDLE  ctrlSurfLeftAileron_{ nullptr };
    CTRLSURFHANDLE  ctrlSurfRightAileron_{ nullptr };
    CTRLSURFHANDLE  ctrlSurfLeftElevator_{ nullptr };
    CTRLSURFHANDLE  ctrlSurfRightElevator_{ nullptr };
    CTRLSURFHANDLE  ctrlSurfLeftRudder_{ nullptr };
    CTRLSURFHANDLE  ctrlSurfRightRudder_{ nullptr };
    CTRLSURFHANDLE  ctrlSurfLeftTrim_{ nullptr };
    CTRLSURFHANDLE  ctrlSurfRightTrim_{ nullptr };

    bco::HydraulicProvider& apu_;
};

inline SurfaceController::SurfaceController(bco::Vessel& vessel, bco::HydraulicProvider& apu)
  : vessel_(vessel),
    apu_(apu)
{}

inline void SurfaceController::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd)
{
    // Determine if our hydro state has change, enable or disable controls.
    auto hydPress = apu_.Level();
    if (hydPress != prevHydraulicState_)
    {
        if (prevHydraulicState_ == 0.0) {
            EnableControls();
        }
        else {
            DisableControls();
        }

        prevHydraulicState_ = hydPress;
    }
}

inline void SurfaceController::HandleSetClassCaps(bco::Vessel& vessel)
{
    // Left Aileron Animation
    static UINT groupLeftAileron = bm::main::ElevonPO_id;
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

    anim_left_aileron_ = vessel.CreateAnimation(0.5);
    anim_left_elevator_ = vessel.CreateAnimation(0.5);

    vessel.AddAnimationComponent(anim_left_aileron_, 0, 1, &animGroupLeftAileron);
    vessel.AddAnimationComponent(anim_left_elevator_, 0, 1, &animGroupLeftElevator);


    // Right Aileron Animation
    static UINT groupRightAileron[1] = { bm::main::ElevonSO_id };
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

    anim_right_aileron_ = vessel.CreateAnimation(0.5);
    anim_right_elevator_ = vessel.CreateAnimation(0.5);

    vessel.AddAnimationComponent(anim_right_aileron_, 0, 1, &animGroupRightAileron);
    vessel.AddAnimationComponent(anim_right_elevator_, 0, 1, &animGroupRightElevator);


    // Left Rudder Animation
    static UINT groupLeftRudder = bm::main::RudderP_id;
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
    static UINT groupRightRudder = bm::main::RudderS_id;
    static VECTOR3 rightRudderAxis = bm::main::RudderAxisSB_loc - bm::main::RudderAxisST_loc;

    normalise(rightRudderAxis);
    static MGROUP_ROTATE animGroupRightRudder(
        0,
        &groupRightRudder,
        1,
        bm::main::RudderAxisSB_loc,
        rightRudderAxis,
        AILERON_RANGE);

    // Control surface Animation.
    anim_left_rudder_ = vessel.CreateAnimation(0.5);
    anim_right_rudder_ = vessel.CreateAnimation(0.5);

    vessel.AddAnimationComponent(anim_left_rudder_, 0, 1, &animGroupLeftRudder);
    vessel.AddAnimationComponent(anim_right_rudder_, 0, 1, &animGroupRightRudder);
}

inline void SurfaceController::SetRudderLevel(double level)
{
    vessel_.SetControlSurfaceLevel(AIRCTRL_RUDDER, level);
}

inline void SurfaceController::EnableControls()
{
    // Aileron Control : bank left/right.  For this we will just use the outer elevon area.
    ctrlSurfLeftAileron_ = vessel_.CreateControlSurface3(AIRCTRL_AILERON, OutboardElevonArea / 2, dClOutboard, _V(-5.0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_left_aileron_);
    ctrlSurfRightAileron_ = vessel_.CreateControlSurface3(AIRCTRL_AILERON, OutboardElevonArea / 2, dClOutboard, _V(5.0, 0, -8.0), AIRCTRL_AXIS_AUTO, 2.0, anim_right_aileron_);

    // Elevator Control : pitch up/down.  For this we use the combined inner and outer area.
    auto fullArea = OutboardElevonArea + InboardElevonArea;
    auto fulldc = dClInboard + dClOutboard;
    ctrlSurfLeftElevator_ = vessel_.CreateControlSurface3(AIRCTRL_ELEVATOR, fullArea, fulldc, _V(-7.0, 0, -10.0), AIRCTRL_AXIS_AUTO, 1.0, anim_left_elevator_);
    ctrlSurfRightElevator_ = vessel_.CreateControlSurface3(AIRCTRL_ELEVATOR, fullArea, fulldc, _V(7.0, 0, -10.0), AIRCTRL_AXIS_AUTO, 1.0, anim_right_elevator_);

    // Rudder contro : yaw.
    ctrlSurfLeftRudder_ = vessel_.CreateControlSurface3(AIRCTRL_RUDDER, RudderArea, dClRudder, _V(0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_left_rudder_);
    ctrlSurfRightRudder_ = vessel_.CreateControlSurface3(AIRCTRL_RUDDER, RudderArea, dClRudder, _V(0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, anim_right_rudder_);

    // Trim : SR-71 does not have trim tabs, or flaps.  We will use the inBoard area and generall as smaller dCl to provide finer Control.
    ctrlSurfLeftTrim_ = vessel_.CreateControlSurface3(AIRCTRL_ELEVATORTRIM, InboardElevonArea, dClInboard, _V(-7.0, 0, -10.0), AIRCTRL_AXIS_XPOS, 1.0, anim_left_elevator_);
    ctrlSurfRightTrim_ = vessel_.CreateControlSurface3(AIRCTRL_ELEVATORTRIM, InboardElevonArea, dClInboard, _V(7.0, 0, -10.0), AIRCTRL_AXIS_XPOS, 1.0, anim_right_elevator_);
}

inline void SurfaceController::DisableControls()
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
