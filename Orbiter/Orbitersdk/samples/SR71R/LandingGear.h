//	LandingGear - SR-71r Orbiter Addon
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

#include "bc_orbiter/vessel.h"
#include "bc_orbiter/Component.h"
#include "bc_orbiter/Animation.h"
#include "bc_orbiter/simple_event.h"
#include "bc_orbiter/panel_display.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class VESSEL;

/**
	Model the landing gear and its controls.

	The landing gear requires hydraulic pressure to function which comes from the APU.  See
	the APU instructions on how to start the APU.

	Configuration:
	GEAR a b
	a = 0/1 up/down.
	b = 0.0 position
*/
class LandingGear : 
    public bco::VesselComponent
    , public bco::HandlesSetClassCaps
    , public bco::HandlesPostStep
    , public bco::HandlesState
    , public bco::HandlesDrawHud
{
public:
	LandingGear(bco::Vessel& Vessel, bco::HydraulicProvider& apu);

    // set_class_caps
    void HandleSetClassCaps(bco::Vessel& Vessel) override;

    // post_step
    void HandlePostStep(bco::Vessel& Vessel, double simt, double simdt, double mjd) override;

    // manage_state
    bool HandleLoadState(bco::Vessel& Vessel, const std::string& line) override;
    std::string HandleSaveState(bco::Vessel& Vessel) override;

    void HandleDrawHUD(bco::Vessel& Vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;

    void Toggle() { position_ = (position_ == 0.0) ? 1.0 : 0.0; }
private:

    bco::HydraulicProvider& apu_;

    double                  position_{ 0.0 };

                                                /*  animLandingGear_ has no visual.  We use it
                                                    as a target provider for the main landing gear
                                                    animation.  By doing this we can control the
                                                    animation of the gear based on available APU
                                                    power.  See the Step method in the cpp file. 
                                                */
    bco::animation_target          animLandingSwitch_  { 2.0 };
    bco::animation_target			animLandingGear_    { 0.15 };
    UINT                    idAnim_             { 0 };

    // VC Animations
    bco::AnimationGroup     lgHandle_           {   {bm::vc::LGHandle_id },
                                                    bm::vc::LGHandle_loc, bm::vc::LGHandleAxis_loc, 
                                                    (40 * RAD),
                                                    0.0, 1.0
                                                };

    // External:
    bco::AnimationGroup     gpRightFrontDoor_   {   { bm::main::LGDoorSF_id },
                                                    bm::main::LGFRBackPivot_loc, bm::main::LGFRFrontPivot_loc,
                                                    (90 * RAD),
                                                    0.0, 0.3 
                                                };

    bco::AnimationGroup     gpLeftFrontDoor_    {   { bm::main::LGDoorPF_id },
                                                    bm::main::LGFLFrontPivot_loc, bm::main::LGFLBackPivot_loc, 		
                                                    (90 * RAD),
                                                    0.0, 0.3 
                                                };

    bco::AnimationGroup     gpFrontGear_        {   { bm::main::LGFrontArm_id,
                                                      bm::main::LGFrontAxel_id,
                                                      bm::main::LGCrossF_id,
                                                      bm::main::LGFrontLeftWheel_id,
                                                      bm::main::LGFrontRightWheel_id },
                                                    bm::main::LGFrontRightPivot_loc, bm::main::LGFrontLeftPivot_loc, 
                                                    (90 * RAD),
                                                    0.2, 1.0 
                                                };

    bco::AnimationGroup     gpLeftOuterDoor_    {   { bm::main::LGDoorPO_id },
                                                    bm::main::LGDoorAxisPOA_loc, bm::main::LGDoorAxisPOF_loc,
                                                    (-100 * RAD),
                                                    0.1, 1.0
                                                };

    bco::AnimationGroup     gpLeftInnerDoor_    {   { bm::main::LGDoorPI_id },
                                                    bm::main::LGDoorAxisPIA_loc, bm::main::LGDoorAxisPIF_loc,
                                                    (100 * RAD),
                                                    0.0, 0.3
                                                };

    bco::AnimationGroup     gpLeftGearUpper_    {   { bm::main::LGLArm_id },
                                                    bm::main::LGCrossAxisPA_loc, bm::main::LGCrossAxisPF_loc,
                                                    (-90 * RAD),
                                                    0.2, 1.0
                                                };

    bco::AnimationGroup     gpLeftGearLower_    {   { bm::main::LGLFork_id,
                                                      bm::main::LGLWheel1_id,
                                                      bm::main::LGLWheel2_id,
                                                      bm::main::LGLWheel3_id },
                                                    _V(-0.39, 0, 0),
                                                    0.5, 0.8
                                                };

    bco::AnimationGroup     gpRightOuterDoor_   {   { bm::main::LGDoorSO_id },
                                                    bm::main::LGDoorAxisSOA_loc, bm::main::LGDoorAxisSOF_loc,
                                                    (100 * RAD),
                                                    0.2, 1.0
                                                };

    bco::AnimationGroup     gpRightInnerDoor_   {   { bm::main::LGDoorSI_id },
                                                    bm::main::LGDoorAxisSIA_loc, bm::main::LGDoorAxisSIF_loc,
                                                    (-100 * RAD),
                                                    0.0, 0.3
                                                };

    bco::AnimationGroup     gpRightGearUpper_   {   { bm::main::LGRArm_id },
                                                    bm::main::LGCrossAxisSA_loc, bm::main::LGCrossAxisSF_loc,
                                                    (90 * RAD),
                                                    0.2, 1.0
                                                };

    bco::AnimationGroup     gpRightGearLower_   {   { bm::main::LGRFork_id, 
                                                      bm::main::LGRWheel1_id, 
                                                      bm::main::LGRWheel2_id, 
                                                      bm::main::LGRWheel3_id },
                                                    _V(0.39, 0, 0),
                                                    0.5, 0.8
                                                };

    const VECTOR3 sTrans{ bm::pnl::pnlLandingGearKnobDown_loc - bm::pnl::pnlLandingGearKnobUp_loc };

    // *** LANDING GEAR *** //
    bco::SimpleEvent<>		btnRaiseGear_       {   bm::vc::GearLeverUpTarget_loc,
                                                    0.01,
                                                    bm::pnl::pnlLandingGearUp_RC
                                                };

    bco::SimpleEvent<>		btnLowerGear_       {   bm::vc::GearLeverDownTarget_loc,
                                                    0.01,
                                                    bm::pnl::pnlLandingGearDown_RC
    };

    bco::panel_display      pnlHudGear_         {   bm::pnl::pnlHUDGear_id,     /* 0-DOWN, 1-UP, 2-Transition*/
                                                    bm::pnl::pnlHUDGear_vrt, 
                                                    0.0305
                                                };
};