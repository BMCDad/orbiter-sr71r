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
#include "bc_orbiter/Animation.h"
#include "bc_orbiter/BaseVessel.h"
#include "bc_orbiter/on_off_input.h"
#include "bc_orbiter/status_display.h"

#include "PropulsionController.h"
#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "SR71r_common.h"

namespace bco = bc_orbiter;

class HoverEngines : 
      public bco::vessel_component
    , public bco::power_consumer
    , public bco::post_step
    , public bco::set_class_caps
    , public bco::draw_hud
    , public bco::manage_state
{
public:
    HoverEngines(bco::power_provider& pwr, bco::BaseVessel& vessel);

    // set_class_caps
    void handle_set_class_caps(bco::BaseVessel& vessel) override;

    // power_consumer
    double amp_draw() const override { return IsMoving() ? 4.0 : 0.0; }

    // post_step
    void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override;

    void handle_draw_hud(bco::BaseVessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;

    // manage_state
    bool handle_load_state(const std::string& line) override;
    std::string handle_save_state() override;

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
            (animHoverDoors_.GetState() > 0.0) && 
            (animHoverDoors_.GetState() < 1.0); 
    }

    void EnableHover(bool isEnabled);

    THRUSTER_HANDLE     hoverThrustHandles_[3];

    bco::Animation          animHoverDoors_ {   0.2};

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

    bco::on_off_input		switchOpen_     { { bm::vc::swHoverDoor_id },
                                                bm::vc::swHoverDoor_location, bm::vc::DoorsRightAxis_location,
                                                toggleOnOff,
                                                bm::pnl::pnlDoorHover_id,
                                                bm::pnl::pnlDoorHover_verts,
                                                bm::pnl::pnlDoorHover_RC
                                        };

    bco::status_display     status_ {           bm::vc::MsgLightHover_id,
                                                bm::vc::MsgLightHover_verts,
                                                bm::pnl::pnlMsgLightHover_id,
                                                bm::pnl::pnlMsgLightHover_verts,
                                                0.0361
                                            };
};
