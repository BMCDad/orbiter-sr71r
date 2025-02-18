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
#include "../bc_orbiter/Animation.h"
#include "../bc_orbiter/Vessel.h"
#include "../bc_orbiter/OnOffInput.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "PropulsionController.h"
#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "SR71r_common.h"
#include "Common.h"

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

class RetroEngines :
    public bco::VesselComponent,
    public bco::PowerConsumer,
    public bco::PostStep,
    public bco::SetClassCaps,
    public bco::DrawHud,
    public bco::ManageState
{
public:
    RetroEngines(bco::PowerProvider& pwr, bco::Vessel& vessel);

    // SetClassCaps
    void HandleSetClassCaps(bco::Vessel& vessel) override;

    // PowerConsumer
    double AmpDraw() const override { return IsMoving() ? 4.0 : 0.0; }

    // PostStep
    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override;

    void HandleDrawHud(bco::Vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;

    // ManageState
    bool HandleLoadState(bco::Vessel& vessel, const std::string& line) override;
    std::string HandleSaveState(bco::Vessel& vessel) override;

private:
    const double MIN_VOLTS = 20.0;

    bco::PowerProvider& power_;
    bco::Vessel& vessel_;

    bool IsPowered() const {
        return
            power_.VoltsAvailable() > MIN_VOLTS;
    }

    bool IsMoving() const {
        return 
            IsPowered() &&
            (animRetroDoors_.GetState() > 0.0) && 
            (animRetroDoors_.GetState() < 1.0); 
    }

    void EnableRetros(bool isEnabled);

    THRUSTER_HANDLE         retroThrustHandles_[2];

    bco::AnimationTarget   animRetroDoors_{ 0.2 };

    bco::AnimationGroup    gpDoors_ {
        {bm::main::EngineCone_id },
        _V(0, 0, -1.2), 
        0.0, 1.0 
    };

    bco::OnOffInput       switchDoors_ {
        { bm::vc::swRetroDoors_id },
        bm::vc::swRetroDoors_loc, bm::vc::DoorsRightAxis_loc,
        toggleOnOff,
        bm::pnlright::pnlDoorRetro_id,
        bm::pnlright::pnlDoorRetro_vrt,
        bm::pnlright::pnlDoorRetro_RC,
        1
    };

    bco::VesselTextureElement       status_ {
        bm::vc::MsgLightRetro_id,
        bm::vc::MsgLightRetro_vrt,
        cmn::vc::main,
        bm::pnl::pnlMsgLightRetro_id,
        bm::pnl::pnlMsgLightRetro_vrt,
        cmn::panel::main
    };
};
