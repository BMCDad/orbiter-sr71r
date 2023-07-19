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
#include "bc_orbiter/Animation.h"
#include "bc_orbiter/BaseVessel.h"
#include "bc_orbiter/on_off_input.h"
#include "bc_orbiter/status_display.h"

#include "PropulsionController.h"
#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "SR71r_common.h"

namespace bco = bc_orbiter;

class RetroEngines :
    public bco::vessel_component,
    public bco::power_consumer,
    public bco::post_step,
    public bco::set_class_caps,
    public bco::draw_hud
{
public:
    RetroEngines(bco::power_provider& pwr, bco::BaseVessel& vessel);

    // set_class_caps
    void handle_set_class_caps(bco::BaseVessel& vessel) override;

    // power_consumer
    double amp_draw() const override { return IsMoving() ? 4.0 : 0.0; }

    // post_step
    void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override;

    void handle_draw_hud(bco::BaseVessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;

private:
    const double MIN_VOLTS = 20.0;

    bco::power_provider& power_;
    bco::BaseVessel& vessel_;

    bool IsPowered() const {
        return
            power_.volts_available() > MIN_VOLTS;
    }

    bool IsMoving() const {
        return 
            IsPowered() &&
            (animRetroDoors_.GetState() > 0.0) && 
            (animRetroDoors_.GetState() < 1.0); 
    }

    void EnableRetros(bool isEnabled);

    THRUSTER_HANDLE         retroThrustHandles_[2];

    bco::Animation          animRetroDoors_ {   0.2};

    bco::AnimationGroup     gpDoors_        {   {bm::main::EngineCone_id },
                                                _V(0, 0, -1.2), 
                                                0.0, 1.0 
                                            };

    bco::on_off_input		switchDoors_    { { bm::vc::swRetroDoors_id },
                                                bm::vc::swRetroDoors_location, bm::vc::DoorsRightAxis_location,
                                                toggleOnOff,
                                                bm::pnl::pnlDoorRetro_id,
                                                bm::pnl::pnlDoorRetro_verts,
                                                bm::pnl::pnlDoorRetro_RC
                                            };

    bco::status_display     status_ {           bm::vc::MsgLightRetro_id,
                                                bm::vc::MsgLightRetro_verts,
                                                bm::pnl::pnlMsgLightRetro_id,
                                                bm::pnl::pnlMsgLightRetro_verts,
                                                0.0361
                                            };

};
