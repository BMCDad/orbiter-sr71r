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

LandingGear::LandingGear(bco::Vessel& Vessel, bco::HydraulicProvider& apu) :
    apu_(apu)
{
    Vessel.AddControl(&btnLowerGear_);
    Vessel.AddControl(&btnRaiseGear_);
    Vessel.AddControl(&pnlHudGear_);

    btnLowerGear_.Attach([&]() { position_ = 1.0; });
    btnRaiseGear_.Attach([&]() { position_ = 0.0; });
}

void LandingGear::HandlePostStep(bco::Vessel& Vessel, double simt, double simdt, double mjd)
{
	// Note:  The animLandingGear can only move if there is hydraulic power, that
	// is the actual landing gear animation.  The animLGHandle_ is the landing gear
	// handle in the cockpit and can move regardless of power, therefore it must
	// always get a piece of the time step.

	if (apu_.Level() > 0.8)
	{
		animLandingGear_.Step(position_, simdt);
	}

    animLandingSwitch_.Step(position_, simdt);

    bco::TranslateMesh(Vessel.GetpanelMeshHandle0(), bm::pnl::pnlLandingGear_id, bm::pnl::pnlLandingGear_vrt, sTrans * animLandingSwitch_.GetState());

    auto hudState = 1; // Gear up
    if (animLandingGear_.GetState() > 0.0) {
        if ((animLandingGear_.GetState() == 1.0) || (fmod(oapiGetSimTime(), 1.0) < 0.5)) {
            hudState = 0;
        }
    }

    pnlHudGear_.SetPosition(hudState);
}

bool LandingGear::HandleLoadState(bco::Vessel& Vessel, const std::string& line)
{
    // [a b] : a: position, 1 (down) 0 (up)   b: anim state
    std::istringstream in(line);
    in >> position_ >> animLandingGear_;
    if (position_ < 0.0) position_ = 0.0;
    if (position_ > 1.0) position_ = 1.0;
    Vessel.SetAnimationState(animLandingGear_);
    return true;
}

std::string LandingGear::HandleSaveState(bco::Vessel& Vessel)
{
    std::ostringstream os;

    os << position_ << " " << animLandingGear_;
    return os.str();
}

void LandingGear::HandleSetClassCaps(bco::Vessel& Vessel)
{
    auto vcMeshIdx = Vessel.GetVCMeshIndex();
    auto meshIdx = Vessel.GetMainMeshIndex();

    // VC
    auto aid = Vessel.CreateVesselAnimation(&animLandingSwitch_, 2.0);
    animLandingGear_.VesselId(aid);
    Vessel.AddVesselAnimationComponent(aid, vcMeshIdx, &lgHandle_);

    // External
    idAnim_ = Vessel.CreateVesselAnimation(&animLandingGear_, 0.15);
    animLandingGear_.VesselId(idAnim_);

    // Front gear
    Vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightFrontDoor_);
    Vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftFrontDoor_);
    Vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpFrontGear_);

    // Left gear
    Vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftOuterDoor_);
    Vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftInnerDoor_);
    auto parent = Vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftGearUpper_);
    Vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftGearLower_, parent);

    // Right gear
    Vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightOuterDoor_);
    Vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightInnerDoor_);
    parent = Vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightGearUpper_);
    Vessel.AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightGearLower_, parent);
}

void LandingGear::HandleDrawHUD(bco::Vessel& Vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
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