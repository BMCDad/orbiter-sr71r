//	HoverEngines - SR-71r Orbiter Addon
//	Copyright(C) 2017  Blake Christensen
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
#include "bc_orbiter\OnOffSwitch.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\VCToggleSwitch.h"
#include "bc_orbiter\PoweredComponent.h"

#include "PropulsionController.h"
#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class HoverEngines : public bco::PoweredComponent
{
public:
    HoverEngines(bco::BaseVessel* vessel, double amps);

    virtual double CurrentDraw() override;
    virtual void SetClassCaps() override;
    virtual bool LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
    virtual void SaveConfiguration(FILEHANDLE scn) const override;

	void Step(double simt, double simdt, double mjd);

    double GetHoverDoorsState() { return animHoverDoors_.GetState(); }
    bool DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

private:

    void EnableHover(bool isEnabled);
	void SetSound();


    THRUSTER_HANDLE     hoverThrustHandles_[3];

    const char*	        ConfigKey = "HOVER";

    int                 hoverMouseId_;


    bco::VCToggleSwitch     swOpen_ {   bt_mesh::SR71rVC::swHoverDoor_id,
										GetBaseVessel(), 
                                        bt_mesh::SR71rVC::swHoverDoor_location,  
                                        bt_mesh::SR71rVC::DoorsRightAxis_location
                                    };

    bco::Animation          animHoverDoors_ {   &swOpen_, 0.2};

    bco::AnimationGroup     gpFrontLeft_    {   { bt_mesh::SR71r::HoverDoorPF_id },
                                                bt_mesh::SR71r::HoverDoorAxisPFF_location, bt_mesh::SR71r::HoverDoorAxisPFA_location,
                                                (140 * RAD),
                                                0, 1
                                            };
        
    bco::AnimationGroup     gpFrontRight_   {   { bt_mesh::SR71r::HoverDoorSF_id },
                                                bt_mesh::SR71r::HoverDoorAxisSFA_location, bt_mesh::SR71r::HoverDoorAxisSFF_location,
                                                (140 * RAD),
                                                0, 1
                                            };

    bco::AnimationGroup     gpLeft_         {   { bt_mesh::SR71r::HoverDoorPA_id } ,
                                                bt_mesh::SR71r::HoverDoorAxisPF_location, bt_mesh::SR71r::HoverDoorAxisPA_location,
                                                (100 * RAD),
                                                0, 1
                                            };

    bco::AnimationGroup     gpRight_        {   { bt_mesh::SR71r::HoverDoorSA_id } ,
                                                bt_mesh::SR71r::HoverDoorAxisSA_location, bt_mesh::SR71r::HoverDoorAxisSF_location, 
                                                (100 * RAD),
                                                0, 1
                                            };
};
