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

#include "bc_orbiter\Animation.h"

#include "PropulsionController.h"
#include "SR71r_mesh.h"
#include "ShipMets.h"

namespace bco = bc_orbiter;

class RetroEngines : 
    public bco::vessel_component,
    public bco::set_class_caps,
    public bco::power_consumer,
    public bco::post_step,
    public bco::draw_hud
{
public:
    RetroEngines();

    // set_class_caps
    void handle_set_class_caps(bco::BaseVessel& vessel) override;

    // power_consumer
    double amp_load() override { return (slotVoltsInput_.value() > MIN_VOLTS) && IsMoving() ? RETRO_AMPS : 0.0; }

    // post_step
    void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override;

    void handle_draw_hud(bco::BaseVessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;


    bco::slot<bool>&    RetroDoorsSlot() { return retroDoorsSlot_; }
    bco::slot<double>& VoltsInputSlot() { return slotVoltsInput_; }

private:
    const double MIN_VOLTS = 20.0;

    bool IsMoving() { return (animRetroDoors_.GetState() > 0.0) && (animRetroDoors_.GetState() < 1.0); }

    bco::slot<bool>     retroDoorsSlot_;
    bco::slot<double>   slotVoltsInput_;

    void EnableRetros(bool isEnabled);

    THRUSTER_HANDLE     retroThrustHandles_[2];

    bco::Animation      animRetroDoors_ {   0.2};

    bco::AnimationGroup gpDoors_        {   {bm::main::EngineCone_id },
                                            _V(0, 0, -1.2), 
                                            0.0, 1.0 
                                        };
};
