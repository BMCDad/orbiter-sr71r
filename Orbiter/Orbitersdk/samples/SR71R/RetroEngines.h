//	RetroEngines - SR-71r Orbiter Addon
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

#include "PropulsionController.h"
#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class RetroEngines : public bco::PoweredComponent
{
public:
    RetroEngines(bco::BaseVessel* vessel, double amps);

    virtual double CurrentDraw() override;
    virtual void OnSetClassCaps() override;
    virtual bool OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
    virtual void OnSaveConfiguration(FILEHANDLE scn) const override;

	void Step(double simt, double simdt, double mjd);

    double GetRetroDoorsState() { return animRetroDoors_.GetState(); }
    bool DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

private:

    void EnableRetros(bool isEnabled);


    THRUSTER_HANDLE     retroThrustHandles_[2];

    const char*	        ConfigKey = "RETRO";

    bco::VCToggleSwitch swRetroDoors_   {   bm::vc::swRetroDoors_id, 
                                            bm::vc::swRetroDoors_location, 
                                            bm::vc::DoorsRightAxis_location
                                        };

    bco::Animation      animRetroDoors_ {   &swRetroDoors_, 0.2};

    bco::AnimationGroup gpDoors_        {   {bm::main::EngineCone_id },
                                            _V(0, 0, -1.2), 
                                            0.0, 1.0 
                                        };
};
