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
    virtual void OnSetClassCaps() override;
    virtual bool OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
    virtual void OnSaveConfiguration(FILEHANDLE scn) const override;

	void Step(double simt, double simdt, double mjd);

    double GetHoverDoorsState() { return animHoverDoors_.GetState(); }
    bool DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

private:

    void EnableHover(bool isEnabled);


    THRUSTER_HANDLE     hoverThrustHandles_[3];

    const char*	        ConfigKey = "HOVER";

    int                 hoverMouseId_;


    bco::VCToggleSwitch     swOpen_ {   bm::vc::swHoverDoor_id, 
                                        bm::vc::swHoverDoor_location,  
                                        bm::vc::DoorsRightAxis_location
                                    };

    bco::Animation          animHoverDoors_ {   &swOpen_, 0.2};

    bco::AnimationGroup     gpFrontLeft_    {   { bm::main::HoverDoorPF_id },
                                                bm::main::HoverDoorAxisPFF_location, bm::main::HoverDoorAxisPFA_location,
                                                (140 * RAD),
                                                0, 1
                                            };
        
    bco::AnimationGroup     gpFrontRight_   {   { bm::main::HoverDoorSF_id },
                                                bm::main::HoverDoorAxisSFA_location, bm::main::HoverDoorAxisSFF_location,
                                                (140 * RAD),
                                                0, 1
                                            };

    bco::AnimationGroup     gpLeft_         {   { bm::main::HoverDoorPA_id } ,
                                                bm::main::HoverDoorAxisPF_location, bm::main::HoverDoorAxisPA_location,
                                                (100 * RAD),
                                                0, 1
                                            };

    bco::AnimationGroup     gpRight_        {   { bm::main::HoverDoorSA_id } ,
                                                bm::main::HoverDoorAxisSA_location, bm::main::HoverDoorAxisSF_location, 
                                                (100 * RAD),
                                                0, 1
                                            };
};
