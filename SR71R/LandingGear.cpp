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

#include "StdAfx.h"

#include "LandingGear.h"
#include "SR71r_mesh.h"

LandingGear::LandingGear(bco::vessel& vessel, bco::hydraulic_provider& apu) :
    apu_(apu)
{
    vessel.AddControl(&btnLowerGear_);
    vessel.AddControl(&btnRaiseGear_);
    vessel.AddControl(&pnlHudGear_);

    btnLowerGear_.attach([&]() { position_ = 1.0; });
    btnRaiseGear_.attach([&]() { position_ = 0.0; });
}

void LandingGear::handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd)
{
	// Note:  The animLandingGear can only move if there is hydraulic power, that
	// is the actual landing gear animation.  The animLGHandle_ is the landing gear
	// handle in the cockpit and can move regardless of power, therefore it must
	// always get a piece of the time step.

	if (apu_.level() > 0.8)
	{
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

    pnlHudGear_.set_position(hudState);
}

bool LandingGear::handle_load_state(bco::vessel& vessel, const std::string& line)
{
    // [a b] : a: position, 1 (down) 0 (up)   b: anim state
    std::istringstream in(line);
    in >> position_ >> animLandingGear_;
    if (position_ < 0.0) position_ = 0.0;
    if (position_ > 1.0) position_ = 1.0;
    vessel.SetAnimationState(animLandingGear_);
    return true;
}

std::string LandingGear::handle_save_state(bco::vessel& vessel)
{
    std::ostringstream os;

    os << position_ << " " << animLandingGear_;
    return os.str();
}

void LandingGear::handle_set_class_caps(bco::vessel& vessel)
{
    auto vcMeshIdx = vessel.GetVCMeshIndex();
    auto meshIdx = vessel.GetMainMeshIndex();

    // VC
    auto aid = vessel.CreateVesselAnimation(&animLandingSwitch_, 2.0);
    animLandingGear_.VesselId(aid);
    vessel.AddVesselAnimationComponent(aid, vcMeshIdx, &lgHandle_);

    // External
    idAnim_ = vessel.CreateVesselAnimation(&animLandingGear_, 0.15);
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

void LandingGear::handle_draw_hud(bco::vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return;
    
    if (animLandingGear_.GetState() == 0.0) return;

    if ((animLandingGear_.GetState() == 1.0) || (fmod(oapiGetSimTime(), 1.0) < 0.5))
    {
        int halfFoot    = 8;
        int ySep        = 30;
        int xSep        = 35;

        int xCenter     = hps->W / 2;
        int yCenter     = hps->H / 2;

        // Front
        skp->Rectangle( xCenter - halfFoot,
                        yCenter - ySep - halfFoot,
                        xCenter + halfFoot,
                        yCenter - ySep + halfFoot);

        // Back left
        skp->Rectangle( xCenter - xSep - halfFoot,
                        yCenter + ySep - halfFoot,
                        xCenter - xSep + halfFoot,
                        yCenter + ySep + halfFoot);

        // Back right
        skp->Rectangle( xCenter + xSep - halfFoot,
                        yCenter + ySep - halfFoot,
                        xCenter + xSep + halfFoot,
                        yCenter + ySep + halfFoot);
    }
}