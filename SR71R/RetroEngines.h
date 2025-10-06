/*
RetroEngines - SR-71r Orbiter Addon
Copyright(C) 2025  Blake Christensen

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "..\bc_orbiter\Vessel.h"

#include "Orbitersdk.h"
#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "SR71Toggle.h"

#include "IFuelSystem.h"
#include "IPowerProvider.h"

namespace bco = bc_orbiter;

class RetroEngines
{
public:
    RetroEngines(bco::Vessel& vessel);

    void Setup(bco::Vessel& vessel, IFuelSystem& fuel);

    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power);

    void LoadState(const std::string& line);
    std::string GetState() const;

//    void handle_draw_hud(bco::vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;

    // manage_state
    //bool handle_load_state(bco::vessel& vessel, const std::string& line) override;
    //std::string handle_save_state(bco::vessel& vessel) override;

private:
    const double MIN_VOLTS = 20.0;

    THRUSTER_HANDLE         retroThrustHandles_[2];

    UINT aidRetroDoors_{ 0 };
    bco::AnimatedValue<bco::StateUpdateTarget>  animRetroDoors_;

    SR71::Toggle togDoors_{
        bm::vc::swRetroDoors_id,
        bm::vc::swRetroDoors_loc, bm::vc::DoorsRightAxis_loc,
        bm::pnlright::pnlDoorRetro_id,
        bm::pnlright::pnlDoorRetro_vrt,
        bm::pnlright::pnlDoorRetro_RC,
        SR71R::RightPanel_ID
    };

    //bco::status_display     status_ {
    //    bm::vc::MsgLightRetro_id,
    //    bm::vc::MsgLightRetro_vrt,
    //    bm::pnl::pnlMsgLightRetro_id,
    //    bm::pnl::pnlMsgLightRetro_vrt,
    //    0.0361
    //};
};

inline RetroEngines::RetroEngines(bco::Vessel& vessel) :
    retroThrustHandles_{ nullptr, nullptr }
{
    vessel.RegisterUIControl(togDoors_);
}

inline void RetroEngines::Setup(bco::Vessel& vessel, IFuelSystem& fuel)
{
    // Animations:
    auto mainIndex = vessel.GetMeshIndex(bm::main::MESH_NAME);

    aidRetroDoors_ = vessel.CreateVesselAnimation();
    vessel.AddAnimationGroup(
        aidRetroDoors_,
        mainIndex,
        { bm::main::EngineCone_id },
        _V(0, 0, -1.2),
        0, 1);

    retroThrustHandles_[0] = vessel.CreateThruster(
        bm::main::ThrustRetroP_loc,
        _V(0, 0, -1),
        SR71R::RETRO_THRUST,
        fuel.GetMainProppelantHandle(),
        SR71R::THRUST_ISP);

    retroThrustHandles_[1] = vessel.CreateThruster(
        bm::main::ThrustRetroS_loc,
        _V(0, 0, -1),
        SR71R::RETRO_THRUST,
        fuel.GetMainProppelantHandle(),
        SR71R::THRUST_ISP);

    vessel.CreateThrusterGroup(retroThrustHandles_, 2, THGROUP_RETRO);

    EXHAUSTSPEC es_retro[2] =
    {
        { retroThrustHandles_[0], NULL, NULL, NULL, 4, .25, 0, 0.1, NULL },
        { retroThrustHandles_[1], NULL, NULL, NULL, 4, .25, 0, 0.1, NULL }
    };

    PARTICLESTREAMSPEC exhaust_retro = {
        0, 2.0, 13, 150, 0.1, 0.2, 16, 1.0, PARTICLESTREAMSPEC::EMISSIVE,
        PARTICLESTREAMSPEC::LVL_SQRT, 0, 1,
        PARTICLESTREAMSPEC::ATM_PLOG, 1e-5, 0.1
    };

    for (auto i = 0; i < 2; i++) vessel.AddExhaust(es_retro + i);
    vessel.AddExhaustStream(retroThrustHandles_[0], _V(-4.38, 0, 3), &exhaust_retro);
    vessel.AddExhaustStream(retroThrustHandles_[1], _V(4.38, 0, 3), &exhaust_retro);
}

inline void RetroEngines::UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power)
{
    auto isMoving = false;

    if (power.GetPowerLevel() > 20.0) {
        isMoving = animRetroDoors_.Update(simdt, togDoors_.IsOn() ? 1.0 : 0.0);
    }
    //auto status = bco::status_display::status::off;
    //if (power_.volts_available() > MIN_VOLTS) {
    //    if ((animRetroDoors_.GetState() > 0.0) && (animRetroDoors_.GetState() < 1.0)) {
    //        status = bco::status_display::status::warn;
    //    }
    //    else {
    //        if (animRetroDoors_.GetState() == 1.0) {
    //            status = bco::status_display::status::on;
    //        }
    //    }
    //}
}

inline void RetroEngines::LoadState(const std::string& line)
{
    std::istringstream is(line);
    int doors;
    double position;
    is >> doors >> position;
    togDoors_.SetState(doors != 0);
    animRetroDoors_.LoadState(position, togDoors_.IsOn() ? 1.0 : 0.0);
}

inline std::string RetroEngines::GetState() const
{
    std::ostringstream out;
    out << (togDoors_.IsOn() ? 1 : 0) << " " << animRetroDoors_.GetCurrent();
    return out.str();
}
