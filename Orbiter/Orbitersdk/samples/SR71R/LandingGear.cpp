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

LandingGear::LandingGear(bco::BaseVessel* vessel) :
Component(vessel),
gearDownSlot_([&](bool v) { position_ = 1.0;  gearDownSlot_.set(); }),
gearUpSlot_([&](bool v) { position_ = 0.0;  gearUpSlot_.set(); }),
hydraulicPressSlot_([&](double v) {})
{
}

void LandingGear::Step(double simt, double simdt, double mjd)
{
	// Note:  The animLandingGear can only move if there is hydraulic power, that
	// is the actual landing gear animation.  The animLGHandle_ is the landing gear
	// handle in the cockpit and can move regardless of power, therefore it must
	// always get a piece of the time step.

	if (hydraulicPressSlot_.value() > 0.8)
	{
		animLandingGear_.Step(position_, simdt);
	}

    animLandingSwitch_.Step(position_, simdt);

    bco::RotateMesh(GetBaseVessel()->GetpanelMeshHandle0(), bm::pnl::pnlLandingGear_id, bm::pnl::pnlLandingGear_verts, sTrans * animLandingSwitch_.GetState());
}

bool LandingGear::OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
	if (_strnicmp(key, ConfigKey, 4) != 0)
	{
		return false;
	}

    // TODO
	//int isOn;
	//double state;

	//sscanf_s(configLine + 4, "%i%lf", &isOn, &state);

	//landingGearSwitch_.SetState((isOn == 0) ? 0.0 : 1.0);
	//
 //   animLandingGear_.SetState(state);
 //   GetBaseVessel()->SetAnimationState(idAnim_, state);

	return true;
}

void LandingGear::OnSaveConfiguration(FILEHANDLE scn) const
{
    // TODO
	//char cbuf[256];
	//auto val = (landingGearSwitch_.GetState() == 0.0) ? 0 : 1;

	//sprintf_s(cbuf, "%i %lf", val, landingGearSwitch_.GetState());
	//oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}

void LandingGear::OnSetClassCaps()
{
    auto vessel = GetBaseVessel();
    auto vcMeshIdx = vessel->GetVCMeshIndex();
    auto meshIdx = vessel->GetMainMeshIndex();

    // VC
    auto aid = vessel->CreateVesselAnimation(&animLandingSwitch_, 2.0);
    vessel->AddVesselAnimationComponent(aid, vcMeshIdx, &lgHandle_);

    // External
    idAnim_ = vessel->CreateVesselAnimation(&animLandingGear_, 0.15);

    // Front gear
    vessel->AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightFrontDoor_);
    vessel->AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftFrontDoor_);
    vessel->AddVesselAnimationComponent(idAnim_, meshIdx, &gpFrontGear_);

    // Left gear
    vessel->AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftOuterDoor_);
    vessel->AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftInnerDoor_);
    auto parent = vessel->AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftGearUpper_);
    vessel->AddVesselAnimationComponent(idAnim_, meshIdx, &gpLeftGearLower_, parent);

    // Right gear
    vessel->AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightOuterDoor_);
    vessel->AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightInnerDoor_);
    parent = vessel->AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightGearUpper_);
    vessel->AddVesselAnimationComponent(idAnim_, meshIdx, &gpRightGearLower_, parent);
}

bool LandingGear::DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return false;
    
    if (animLandingGear_.GetState() == 0.0) return false;

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

        return true;
    }

    return false;
}