/*
SurfaceController - SR-71r Orbiter Addon
Copyright(C) 2015  Blake Christensen

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Orbitersdk.h"

#include "..\bc_orbiter\Vessel.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "IHydraulicProvider.h"

const float AILERON_RANGE = (float)(20.0 * RAD);
/*	Control settings.
	These settings control the responsiveness of the flight control surfaces.  The area
	settings are from the SR-71 manual and should probably be kept as is.  The dCl... settings
	control the effectiveness of each control.  These can be tweaked to adjust the handling of the
	vessel.  To see how these are applied look at the 'EnableControls' method of this class.
*/
const double InboardElevonArea  = 3.6;  // From manual, inboard (39sf/3.6ms).
const double dClInboard         = 0.6;  // Adjust to impact mostly elevator function.
const double OutboardElevonArea = 4.8;  // From manual, outboard (52.5sf/4.8ms).
const double dClOutboard        = 0.2;  // Adjust to impact both elevator and aileron function.
const double RudderArea         = 6.4;  // From manual, movable rudder area (70sf)
const double dClRudder          = 0.2;  // Adjust to implact rudder function.

namespace bco = bc_orbiter;

/**	SurfaceController
	Manages the control surface for the airplane.
	Checks for hydraulic pressure and enables control if present.  If not then all control
	surfaces are disabled.
*/
class SurfaceController
{
public:
    SurfaceController(bco::Vessel& vessel);

    void Setup(bco::Vessel& vessel);

    void UpdateState(bco::Vessel& vessel, double simdt, IHydraulicProvider& power);

    void SetRudderLevel(double level);

private:
    void EnableControls(bco::Vessel& vessel);
    void DisableControls(bco::Vessel& vessel);

    double  prevHydraulicState_{ 0.0 };

    UINT     aidLeftAileron_{ 0 };
    UINT     aidLeftElevator_{ 0 };
    UINT     aidRightAileron_{ 0 };
    UINT     aidRightElevator_{ 0 };
    UINT     aidLeftRudder_{ 0 };
    UINT     aidRightRudder_{ 0 };

    CTRLSURFHANDLE  ctrlSurfLeftAileron_{ nullptr };
    CTRLSURFHANDLE  ctrlSurfRightAileron_{ nullptr };
    CTRLSURFHANDLE  ctrlSurfLeftElevator_{ nullptr };
    CTRLSURFHANDLE  ctrlSurfRightElevator_{ nullptr };
    CTRLSURFHANDLE  ctrlSurfLeftRudder_{ nullptr };
    CTRLSURFHANDLE  ctrlSurfRightRudder_{ nullptr };
    CTRLSURFHANDLE  ctrlSurfLeftTrim_{ nullptr };
    CTRLSURFHANDLE  ctrlSurfRightTrim_{ nullptr };
};

inline SurfaceController::SurfaceController(bco::Vessel& vessel)
{ }

inline void SurfaceController::Setup(bco::Vessel& vessel)
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

    aidLeftAileron_ = vessel.CreateAnimation(0.5);
    aidLeftElevator_ = vessel.CreateAnimation(0.5);

    vessel.AddAnimationComponent(aidLeftAileron_, 0, 1, &animGroupLeftAileron);
    vessel.AddAnimationComponent(aidLeftElevator_, 0, 1, &animGroupLeftElevator);


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

    aidRightAileron_ = vessel.CreateAnimation(0.5);
    aidRightElevator_ = vessel.CreateAnimation(0.5);

    vessel.AddAnimationComponent(aidRightAileron_, 0, 1, &animGroupRightAileron);
    vessel.AddAnimationComponent(aidRightElevator_, 0, 1, &animGroupRightElevator);


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

    // Control surface animation.
    aidLeftRudder_ = vessel.CreateAnimation(0.5);
    aidRightRudder_ = vessel.CreateAnimation(0.5);

    vessel.AddAnimationComponent(aidLeftRudder_, 0, 1, &animGroupLeftRudder);
    vessel.AddAnimationComponent(aidRightRudder_, 0, 1, &animGroupRightRudder);
}

inline void SurfaceController::UpdateState(bco::Vessel& vessel, double simdt, IHydraulicProvider& hydraulic)
{
    // Determine if our hydro state has change, enable or disable controls.
    auto hydPress = hydraulic.Level();
    if (hydPress != prevHydraulicState_)
    {
        if (prevHydraulicState_ == 0.0)
        {
            EnableControls(vessel);
        }
        else
        {
            DisableControls(vessel);
        }

        prevHydraulicState_ = hydPress;
    }
}

inline void SurfaceController::EnableControls(bco::Vessel& vessel)
{
    // Aileron control : bank left/right.  For this we will just use the outer elevon area.
    ctrlSurfLeftAileron_ = vessel.CreateControlSurface3(AIRCTRL_AILERON, OutboardElevonArea / 2, dClOutboard, _V(-5.0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, aidLeftAileron_);
    ctrlSurfRightAileron_ = vessel.CreateControlSurface3(AIRCTRL_AILERON, OutboardElevonArea / 2, dClOutboard, _V(5.0, 0, -8.0), AIRCTRL_AXIS_AUTO, 2.0, aidRightAileron_);

    // Elevator control : pitch up/down.  For this we use the combined inner and outer area.
    auto fullArea = OutboardElevonArea + InboardElevonArea;
    auto fulldc = dClInboard + dClOutboard;
    ctrlSurfLeftElevator_ = vessel.CreateControlSurface3(AIRCTRL_ELEVATOR, fullArea, fulldc, _V(-7.0, 0, -10.0), AIRCTRL_AXIS_AUTO, 1.0, aidLeftElevator_);
    ctrlSurfRightElevator_ = vessel.CreateControlSurface3(AIRCTRL_ELEVATOR, fullArea, fulldc, _V(7.0, 0, -10.0), AIRCTRL_AXIS_AUTO, 1.0, aidRightElevator_);

    // Rudder contro : yaw.
    ctrlSurfLeftRudder_ = vessel.CreateControlSurface3(AIRCTRL_RUDDER, RudderArea, dClRudder, _V(0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, aidLeftRudder_);
    ctrlSurfRightRudder_ = vessel.CreateControlSurface3(AIRCTRL_RUDDER, RudderArea, dClRudder, _V(0, 0, -8.0), AIRCTRL_AXIS_AUTO, 1.0, aidRightRudder_);

    // Trim : SR-71 does not have trim tabs, or flaps.  We will use the inBoard area and generall as smaller dCl to provide finer control.
    ctrlSurfLeftTrim_ = vessel.CreateControlSurface3(AIRCTRL_ELEVATORTRIM, InboardElevonArea, dClInboard, _V(-7.0, 0, -10.0), AIRCTRL_AXIS_XPOS, 1.0, aidLeftElevator_);
    ctrlSurfRightTrim_ = vessel.CreateControlSurface3(AIRCTRL_ELEVATORTRIM, InboardElevonArea, dClInboard, _V(7.0, 0, -10.0), AIRCTRL_AXIS_XPOS, 1.0, aidRightElevator_);
}

inline void SurfaceController::DisableControls(bco::Vessel& vessel)
{
    vessel.DelControlSurface(ctrlSurfLeftAileron_);
    vessel.DelControlSurface(ctrlSurfRightAileron_);
    vessel.DelControlSurface(ctrlSurfLeftElevator_);
    vessel.DelControlSurface(ctrlSurfRightElevator_);
    vessel.DelControlSurface(ctrlSurfLeftRudder_);
    vessel.DelControlSurface(ctrlSurfRightRudder_);
    vessel.DelControlSurface(ctrlSurfLeftTrim_);
    vessel.DelControlSurface(ctrlSurfRightTrim_);
}
