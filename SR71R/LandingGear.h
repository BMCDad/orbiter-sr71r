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

#include "../bc_orbiter/Animation.h"
#include "../bc_orbiter/PanelTextureElement.h"
#include "../bc_orbiter/VesselEvent.h"
#include "../bc_orbiter/Vessel.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"
#include "SR71rPanelRight_mesh.h"

#include "Common.h"

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

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
class LandingGear : public bco::VesselComponent
{
public:
	LandingGear(bco::Vessel& vessel, bco::HydraulicProvider& apu);

    // SetClassCaps
    void HandleSetClassCaps(bco::Vessel& vessel) override;

    // PostStep
    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override;

    // ManageState
    bool HandleLoadState(bco::Vessel& vessel, const std::string& line) override;
    std::string HandleSaveState(bco::Vessel& vessel) override;

    void HandleDrawHud(bco::Vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;

    void Toggle() { position_ = (position_ == 0.0) ? 1.0 : 0.0; }
private:

    bco::HydraulicProvider& apu_;

    double                  position_{ 0.0 };

                                                /*  animLandingGear_ has no visual.  We use it
                                                    as a target provider for the main landing gear
                                                    Animation.  By doing this we can Control the
                                                    Animation of the gear based on available APU
                                                    power.  See the Step method in the cpp file. 
                                                */
    bco::AnimationTarget    animLandingSwitch_  { 2.0 };
    bco::AnimationTarget    animLandingGear_    { 0.15 };
    UINT                    idAnim_             { 0 };

    // VC Animations
    bco::AnimationGroup     lgHandle_ {
        {bm::vc::LGHandle_id },
        bm::vc::LGHandle_loc, bm::vc::LGHandleAxis_loc, 
        (40 * RAD),
        0.0, 1.0
    };

    // External:
    bco::AnimationGroup     gpRightFrontDoor_ {
        { bm::main::LGDoorSF_id },
        bm::main::LGFRBackPivot_loc, bm::main::LGFRFrontPivot_loc,
        (90 * RAD),
        0.0, 0.3 
    };

    bco::AnimationGroup     gpLeftFrontDoor_ {
        { bm::main::LGDoorPF_id },
        bm::main::LGFLFrontPivot_loc, bm::main::LGFLBackPivot_loc, 		
        (90 * RAD),
        0.0, 0.3 
    };

    bco::AnimationGroup     gpFrontGear_ {
        { bm::main::LGFrontArm_id,
            bm::main::LGFrontAxel_id,
            bm::main::LGCrossF_id,
            bm::main::LGFrontLeftWheel_id,
            bm::main::LGFrontRightWheel_id },
        bm::main::LGFrontRightPivot_loc, bm::main::LGFrontLeftPivot_loc, 
        (90 * RAD),
        0.2, 1.0 
    };

    bco::AnimationGroup     gpLeftOuterDoor_ {
        { bm::main::LGDoorPO_id },
        bm::main::LGDoorAxisPOA_loc, bm::main::LGDoorAxisPOF_loc,
        (-100 * RAD),
        0.1, 1.0
    };

    bco::AnimationGroup     gpLeftInnerDoor_ {
        { bm::main::LGDoorPI_id },
        bm::main::LGDoorAxisPIA_loc, bm::main::LGDoorAxisPIF_loc,
        (100 * RAD),
        0.0, 0.3
    };

    bco::AnimationGroup     gpLeftGearUpper_ {
        { bm::main::LGLArm_id },
        bm::main::LGCrossAxisPA_loc, bm::main::LGCrossAxisPF_loc,
        (-90 * RAD),
        0.2, 1.0
    };

    bco::AnimationGroup     gpLeftGearLower_ {
        { bm::main::LGLFork_id, bm::main::LGLWheel1_id, bm::main::LGLWheel2_id, bm::main::LGLWheel3_id },
        _V(-0.39, 0, 0),
        0.5, 0.8
    };

    bco::AnimationGroup     gpRightOuterDoor_ {
        { bm::main::LGDoorSO_id },
        bm::main::LGDoorAxisSOA_loc, bm::main::LGDoorAxisSOF_loc,
        (100 * RAD),
        0.2, 1.0
     };

    bco::AnimationGroup     gpRightInnerDoor_ {
        { bm::main::LGDoorSI_id },
        bm::main::LGDoorAxisSIA_loc, bm::main::LGDoorAxisSIF_loc,
        (-100 * RAD),
        0.0, 0.3
    };

    bco::AnimationGroup     gpRightGearUpper_ {
        { bm::main::LGRArm_id },
        bm::main::LGCrossAxisSA_loc, bm::main::LGCrossAxisSF_loc,
        (90 * RAD),
        0.2, 1.0
    };

    bco::AnimationGroup     gpRightGearLower_ {
        { bm::main::LGRFork_id, bm::main::LGRWheel1_id, bm::main::LGRWheel2_id, bm::main::LGRWheel3_id },
        _V(0.39, 0, 0),
        0.5, 0.8
    };

    const VECTOR3 sTrans{ bm::pnl::pnlLandingGearKnobDown_loc - bm::pnl::pnlLandingGearKnobUp_loc };

    // *** LANDING GEAR *** //
    bco::VesselEvent     btnRaiseGear_ {
        bm::vc::GearLeverUpTarget_loc,
        0.01,
        0,
        bm::pnl::pnlLandingGearUp_RC,
        0
    };

    bco::VesselEvent     btnLowerGear_ {
        bm::vc::GearLeverDownTarget_loc,
        0.01,
        0,
        bm::pnl::pnlLandingGearDown_RC,
        0
    };

    bco::PanelTextureControl  pnlHudGear_ {
        bm::pnl::pnlHUDGear_id,     /* 0-DOWN, 1-UP, 2-Transition*/
        bm::pnl::pnlHUDGear_vrt, 
        cmn::panel::main
    };
};

inline LandingGear::LandingGear(bco::Vessel& vessel, bco::HydraulicProvider& apu) :
    apu_(apu)
{
    vessel.AddControl(&btnLowerGear_);
    vessel.AddControl(&btnRaiseGear_);
    vessel.AddControl(&pnlHudGear_);

    btnLowerGear_.Attach([&](VESSEL4&) { position_ = 1.0; });
    btnRaiseGear_.Attach([&](VESSEL4&) { position_ = 0.0; });
}

inline void LandingGear::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd)
{
    // Note:  The animLandingGear can only move if there is hydraulic power, that
    // is the actual landing gear Animation.  The animLGHandle_ is the landing gear
    // handle in the cockpit and can move regardless of power, therefore it must
    // always get a piece of the time step.

    if (apu_.Level() > 0.8) {
        animLandingGear_.Step(position_, simdt);
    }

    animLandingSwitch_.Step(position_, simdt);

    bco::TranslateMesh(vessel.GetpanelMeshHandle(0), bm::pnl::pnlLandingGear_id, bm::pnl::pnlLandingGear_vrt, sTrans * animLandingSwitch_.GetState());

    auto hudState = 1; // Gear up
    if (animLandingGear_.GetState() > 0.0) {
        if ((animLandingGear_.GetState() == 1.0) || (fmod(oapiGetSimTime(), 1.0) < 0.5)) {
            hudState = 0;
        }
    }

    pnlHudGear_.setState(vessel, hudState);
}

inline bool LandingGear::HandleLoadState(bco::Vessel& vessel, const std::string& line)
{
    // [a b] : a: position, 1 (down) 0 (up)   b: anim state
    std::istringstream in(line);
    in >> position_ >> animLandingGear_;
    if (position_ < 0.0) position_ = 0.0;
    if (position_ > 1.0) position_ = 1.0;
    vessel.SetAnimationState(animLandingGear_);
    return true;
}

inline std::string LandingGear::HandleSaveState(bco::Vessel& vessel)
{
    std::ostringstream os;

    os << position_ << " " << animLandingGear_;
    return os.str();
}

inline void LandingGear::HandleSetClassCaps(bco::Vessel& vessel)
{
    auto vcMeshIdx = vessel.GetVCMeshIndex();
    auto meshIdx = vessel.GetMainMeshIndex();

    // VC
    auto aid = vessel.CreateVesselAnimation(animLandingSwitch_);
    animLandingGear_.VesselId(aid);
    vessel.AddVesselAnimationComponent(aid, vcMeshIdx, &lgHandle_);

    // External
    idAnim_ = vessel.CreateVesselAnimation(animLandingGear_);
    animLandingGear_.VesselId(idAnim_);

    // Front gear
    vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightFrontDoor_);
    vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftFrontDoor_);
    vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpFrontGear_);

    // Left gear
    vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftOuterDoor_);
    vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftInnerDoor_);
    auto parent = vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftGearUpper_);
    vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftGearLower_, parent);

    // Right gear
    vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightOuterDoor_);
    vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightInnerDoor_);
    parent = vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightGearUpper_);
    vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightGearLower_, parent);
}

inline void LandingGear::HandleDrawHud(bco::Vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return;

    if (animLandingGear_.GetState() == 0.0) return;

    if ((animLandingGear_.GetState() == 1.0) || (fmod(oapiGetSimTime(), 1.0) < 0.5))
    {
        int halfFoot = 8;
        int ySep = 30;
        int xSep = 35;

        int xCenter = hps->W / 2;
        int yCenter = hps->H / 2;

        // Front
        skp->Rectangle(xCenter - halfFoot,
            yCenter - ySep - halfFoot,
            xCenter + halfFoot,
            yCenter - ySep + halfFoot);

        // Back left
        skp->Rectangle(xCenter - xSep - halfFoot,
            yCenter + ySep - halfFoot,
            xCenter - xSep + halfFoot,
            yCenter + ySep + halfFoot);

        // Back right
        skp->Rectangle(xCenter + xSep - halfFoot,
            yCenter + ySep - halfFoot,
            xCenter + xSep + halfFoot,
            yCenter + ySep + halfFoot);
    }
}