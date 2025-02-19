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

class HoverEngines : public bco::VesselComponent, public bco::PowerConsumer
{
public:
    HoverEngines(bco::PowerProvider& pwr, bco::Vessel& vessel);

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
            (animHoverDoors_.GetState() > 0.0) && 
            (animHoverDoors_.GetState() < 1.0); 
    }

    void EnableHover(bool isEnabled);

    THRUSTER_HANDLE         hoverThrustHandles_[3];

    bco::AnimationTarget   animHoverDoors_{ 0.2 };

    bco::AnimationGroup    gpFrontLeft_{
        { bm::main::HoverDoorPF_id },
        bm::main::HoverDoorAxisPFF_loc, bm::main::HoverDoorAxisPFA_loc,
        (140 * RAD),
        0, 1
    };
        
    bco::AnimationGroup    gpFrontRight_{
        { bm::main::HoverDoorSF_id },
        bm::main::HoverDoorAxisSFA_loc, bm::main::HoverDoorAxisSFF_loc,
        (140 * RAD),
        0, 1
    };

    bco::AnimationGroup    gpLeft_{
        { bm::main::HoverDoorPA_id } ,
        bm::main::HoverDoorAxisPF_loc, bm::main::HoverDoorAxisPA_loc,
        (100 * RAD),
        0, 1
    };

    bco::AnimationGroup    gpRight_{ 
        { bm::main::HoverDoorSA_id } ,
        bm::main::HoverDoorAxisSA_loc, bm::main::HoverDoorAxisSF_loc,
        (100 * RAD),
        0, 1
    };

    bco::OnOffInput       switchOpen_{
        { bm::vc::swHoverDoor_id },
        bm::vc::swHoverDoor_loc, bm::vc::DoorsRightAxis_loc,
        toggleOnOff,
        bm::pnlright::pnlHoverDoor_id,
        bm::pnlright::pnlHoverDoor_vrt,
        bm::pnlright::pnlHoverDoor_RC,
        1
    };

    bco::VesselTextureElement       status_ {
        bm::vc::MsgLightHover_id,
        bm::vc::MsgLightHover_vrt,
        cmn::vc::main,
        bm::pnl::pnlMsgLightHover_id,
        bm::pnl::pnlMsgLightHover_vrt,
        cmn::panel::main
    };
};

inline HoverEngines::HoverEngines(bco::PowerProvider& pwr, bco::Vessel& vessel) :
    power_(pwr),
    vessel_(vessel)
{
    power_.AttachConsumer(this);
    animHoverDoors_.SetTargetFunction([this] { EnableHover(true); });
    vessel.AddControl(&switchOpen_);
    vessel.AddControl(&status_);
}

inline void HoverEngines::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd)
{
    if (IsPowered()) {
        animHoverDoors_.Step(switchOpen_.IsOn() ? 1.0 : 0.0, simdt);
    }

    auto status = cmn::status::off;
    if (power_.VoltsAvailable() > MIN_VOLTS) {
        if ((animHoverDoors_.GetState() > 0.0) && (animHoverDoors_.GetState() < 1.0)) {
            status = cmn::status::warn;
        }
        else {
            if (animHoverDoors_.GetState() == 1.0) {
                status = cmn::status::on;
            }
        }
    }
    status_.set_state(vessel, status);
}

inline void HoverEngines::HandleSetClassCaps(bco::Vessel& vessel)
{
    auto vcIdx = vessel.GetVCMeshIndex();
    auto mIdx = vessel.GetMainMeshIndex();

    //  Hover engines : positions are simplified.
    hoverThrustHandles_[0] = vessel.CreateThruster(
        _V(0.0, 0.0, 5.0),
        _V(0, 1, 0),
        HOVER_THRUST,
        vessel.MainPropellant(),
        THRUST_ISP);

    hoverThrustHandles_[1] = vessel.CreateThruster(
        _V(-3.0, 0.0, -5.0),
        _V(0, 1, 0),
        HOVER_THRUST * 0.5,
        vessel.MainPropellant(),
        THRUST_ISP);

    hoverThrustHandles_[2] = vessel.CreateThruster(
        _V(3.0, 0.0, -5.0),
        _V(0, 1, 0),
        HOVER_THRUST * 0.5,
        vessel.MainPropellant(),
        THRUST_ISP);

    vessel.CreateThrusterGroup(hoverThrustHandles_, 3, THGROUP_HOVER);

    EXHAUSTSPEC es_hover[3] =
    {
        { hoverThrustHandles_[0], NULL, NULL, NULL, 4, .25, 0, 0.1, NULL },
        { hoverThrustHandles_[1], NULL, NULL, NULL, 4, .25, 0, 0.1, NULL },
        { hoverThrustHandles_[2], NULL, NULL, NULL, 4, .25, 0, 0.1, NULL }
    };

    PARTICLESTREAMSPEC exhaust_hover = {
        0, 2.0, 13, 150, 0.1, 0.2, 16, 1.0, PARTICLESTREAMSPEC::EMISSIVE,
        PARTICLESTREAMSPEC::LVL_SQRT, 0, 1,
        PARTICLESTREAMSPEC::ATM_PLOG, 1e-5, 0.1
    };

    for (auto i = 0; i < 3; i++) vessel.AddExhaust(es_hover + i);
    vessel.AddExhaustStream(hoverThrustHandles_[0], bm::main::ThrustHoverF_loc, &exhaust_hover);
    vessel.AddExhaustStream(hoverThrustHandles_[1], bm::main::ThrustHoverP_loc, &exhaust_hover);
    vessel.AddExhaustStream(hoverThrustHandles_[2], bm::main::ThrustHoverS_loc, &exhaust_hover);


    //    swOpen_.Setup(Vessel);

    auto aid = vessel.CreateVesselAnimation(animHoverDoors_);
    vessel.AddVesselAnimationComponent(aid, mIdx, &gpFrontLeft_);
    vessel.AddVesselAnimationComponent(aid, mIdx, &gpFrontRight_);
    vessel.AddVesselAnimationComponent(aid, mIdx, &gpLeft_);
    vessel.AddVesselAnimationComponent(aid, mIdx, &gpRight_);
}

inline bool HoverEngines::HandleLoadState(bco::Vessel& vessel, const std::string& line)
{
    std::istringstream in(line);
    in >> switchOpen_ >> animHoverDoors_;
    vessel.SetAnimationState(animHoverDoors_);
    return true;
}

inline std::string HoverEngines::HandleSaveState(bco::Vessel& vessel)
{
    std::ostringstream os;
    os << switchOpen_ << " " << animHoverDoors_;
    return os.str();
}

inline void HoverEngines::EnableHover(bool isEnabled)
{
    if (isEnabled) {
        if (animHoverDoors_.GetState() == 1.0) {
            vessel_.SetThrusterResource(hoverThrustHandles_[0], vessel_.MainPropellant());
            vessel_.SetThrusterResource(hoverThrustHandles_[1], vessel_.MainPropellant());
            vessel_.SetThrusterResource(hoverThrustHandles_[2], vessel_.MainPropellant());
        }
    }
    else {
        vessel_.SetThrusterResource(hoverThrustHandles_[0], nullptr);
        vessel_.SetThrusterResource(hoverThrustHandles_[1], nullptr);
        vessel_.SetThrusterResource(hoverThrustHandles_[2], nullptr);
    }
}

inline void HoverEngines::HandleDrawHud(bco::Vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return;

    if (animHoverDoors_.GetState() == 0.0) return;

    if ((animHoverDoors_.GetState() == 1.0) || (fmod(oapiGetSimTime(), 1.0) < 0.5)) {
        int halfFoot = 8;
        int ySep = 10;
        int xSep = 15;

        int xCenter = hps->W / 2;
        int yCenter = hps->H / 2;

        // Front
        skp->Ellipse(xCenter - halfFoot,
            yCenter - ySep - halfFoot,
            xCenter + halfFoot,
            yCenter - ySep + halfFoot);

        // Back left
        skp->Ellipse(xCenter - xSep - halfFoot,
            yCenter + ySep - halfFoot,
            xCenter - xSep + halfFoot,
            yCenter + ySep + halfFoot);

        // Back right
        skp->Ellipse(xCenter + xSep - halfFoot,
            yCenter + ySep - halfFoot,
            xCenter + xSep + halfFoot,
            yCenter + ySep + halfFoot);
    }
}

//bool HoverEngines::OnLoadPanel2D(int id, PANELHANDLE hPanel)
//{
//    oapiRegisterPanelArea(ID_DOOR, bm::pnl::pnlDoorHover_RC, PANEL_REDRAW_USER);
//    GetBaseVessel()->RegisterPanelArea(hPanel, ID_DOOR, bm::pnl::pnlDoorHover_RC, PANEL_REDRAW_MOUSE, PANEL_MOUSE_LBDOWN);
//
//    return true;
//}
//
//bool HoverEngines::OnPanelMouseEvent(int id, int event)
//{
//    swOpen_.Toggle();
//    return true;
//}
//
//bool HoverEngines::OnPanelRedrawEvent(int id, int event, SURFHANDLE surf)
//{
//    bco::DrawPanelOnOff(GetBaseVessel()->GetpanelMeshHandle0(), bm::pnl::pnlDoorHover_id, bm::pnl::pnlDoorHover_vrt, swOpen_.IsOn(), 0.0148);
//    return true;
//}
