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

#include "bc_orbiter\Component.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\EventTarget.h"
#include "bc_orbiter\Animation.h"

#include "APU.h"
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
class LandingGear : public bco::Component
{
public:
	LandingGear(bco::BaseVessel* vessel);

	virtual void SetClassCaps() override;
	virtual bool VCRedrawEvent(int id, int event, SURFHANDLE surf) override { return false; }
	virtual bool LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	virtual void SaveConfiguration(FILEHANDLE scn) const override;

	void Step(double simt, double simdt, double mjd);


	void SetAPU(APU* apu) { apu_ = apu; }

    bool DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

	bco::OnOffSwitch& LandingGearSwitch();

private:
	void SetSound();

	bool gearUpSound = false, gearUpLockedSound = false, gearDownSound = false, gearDownLockedSound = false;

	bco::OnOffSwitch		landingGearSwitch_  { _V(0.0, 0.0, 0.0), 0.0, GetBaseVessel()};
    bco::EventTarget        targetGearDown_     { bt_mesh::SR71rVC::GearLeverDownTarget_location,    0.01 };
    bco::EventTarget        targetGearUp_       { bt_mesh::SR71rVC::GearLeverUpTarget_location,      0.01 };

	APU*					apu_;

	const char*				ConfigKey = "GEAR";

                                                /*  animLandingGear_ has no visual.  We use it
                                                    as a target provider for the main landing gear
                                                    animation.  By doing this we can control the
                                                    animation of the gear based on available APU
                                                    power.  See the Step method in the cpp file. 
                                                */
    bco::Animation			animLandingGear_    { &landingGearSwitch_, 0.15 };
    UINT                    idAnim_             { 0 };

    // VC Animations
    bco::AnimationGroup     lgHandle_           {   {bt_mesh::SR71rVC::LGHandle_id },
                                                    bt_mesh::SR71rVC::LGHandle_location, bt_mesh::SR71rVC::LGHandleAxis_location, 
                                                    (40 * RAD),
                                                    0.0, 1.0
                                                };

    // External:
    bco::AnimationGroup     gpRightFrontDoor_   {   { bt_mesh::SR71r::LGDoorSF_id },
                                                    bt_mesh::SR71r::LGFRBackPivot_location, bt_mesh::SR71r::LGFRFrontPivot_location,
                                                    (90 * RAD),
                                                    0.0, 0.3 
                                                };

    bco::AnimationGroup     gpLeftFrontDoor_    {   { bt_mesh::SR71r::LGDoorPF_id },
                                                    bt_mesh::SR71r::LGFLFrontPivot_location, bt_mesh::SR71r::LGFLBackPivot_location, 		
                                                    (90 * RAD),
                                                    0.0, 0.3 
                                                };

    bco::AnimationGroup     gpFrontGear_        {   { bt_mesh::SR71r::LGFrontArm_id,
                                                      bt_mesh::SR71r::LGFrontAxel_id,
                                                      bt_mesh::SR71r::LGCrossF_id,
                                                      bt_mesh::SR71r::LGFrontLeftWheel_id,
                                                      bt_mesh::SR71r::LGFrontRightWheel_id },
                                                    bt_mesh::SR71r::LGFrontRightPivot_location, bt_mesh::SR71r::LGFrontLeftPivot_location, 
                                                    (90 * RAD),
                                                    0.2, 1.0 
                                                };

    bco::AnimationGroup     gpLeftOuterDoor_    {   { bt_mesh::SR71r::LGDoorPO_id },
                                                    bt_mesh::SR71r::LGDoorAxisPOA_location, bt_mesh::SR71r::LGDoorAxisPOF_location,
                                                    (-100 * RAD),
                                                    0.1, 1.0
                                                };

    bco::AnimationGroup     gpLeftInnerDoor_    {   { bt_mesh::SR71r::LGDoorPI_id },
                                                    bt_mesh::SR71r::LGDoorAxisPIA_location, bt_mesh::SR71r::LGDoorAxisPIF_location,
                                                    (100 * RAD),
                                                    0.0, 0.3
                                                };

    bco::AnimationGroup     gpLeftGearUpper_    {   { bt_mesh::SR71r::LGLArm_id },
                                                    bt_mesh::SR71r::LGCrossAxisPA_location, bt_mesh::SR71r::LGCrossAxisPF_location,
                                                    (-90 * RAD),
                                                    0.2, 1.0
                                                };

    bco::AnimationGroup     gpLeftGearLower_    {   { bt_mesh::SR71r::LGLFork_id,
                                                      bt_mesh::SR71r::LGLWheel1_id,
                                                      bt_mesh::SR71r::LGLWheel2_id,
                                                      bt_mesh::SR71r::LGLWheel3_id },
                                                    _V(-0.39, 0, 0),
                                                    0.5, 0.8
                                                };

    bco::AnimationGroup     gpRightOuterDoor_   {   { bt_mesh::SR71r::LGDoorSO_id },
                                                    bt_mesh::SR71r::LGDoorAxisSOA_location, bt_mesh::SR71r::LGDoorAxisSOF_location,
                                                    (100 * RAD),
                                                    0.2, 1.0
                                                };

    bco::AnimationGroup     gpRightInnerDoor_   {   { bt_mesh::SR71r::LGDoorSI_id },
                                                    bt_mesh::SR71r::LGDoorAxisSIA_location, bt_mesh::SR71r::LGDoorAxisSIF_location,
                                                    (-100 * RAD),
                                                    0.0, 0.3
                                                };

    bco::AnimationGroup     gpRightGearUpper_   {   { bt_mesh::SR71r::LGRArm_id },
                                                    bt_mesh::SR71r::LGCrossAxisSA_location, bt_mesh::SR71r::LGCrossAxisSF_location,
                                                    (90 * RAD),
                                                    0.2, 1.0
                                                };

    bco::AnimationGroup     gpRightGearLower_   {   { bt_mesh::SR71r::LGRFork_id, 
                                                      bt_mesh::SR71r::LGRWheel1_id, 
                                                      bt_mesh::SR71r::LGRWheel2_id, 
                                                      bt_mesh::SR71r::LGRWheel3_id },
                                                    _V(0.39, 0, 0),
                                                    0.5, 0.8
                                                };

};