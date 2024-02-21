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

#include "StdAfx.h"

#include "HoverEngines.h"
#include "Orbitersdk.h"
#include "ShipMets.h"
#include "SR71r_mesh.h"

HoverEngines::HoverEngines(bco::PowerProvider& pwr, bco::Vessel& Vessel) :
    power_(pwr),
    vessel_(Vessel)
{
    power_.AttachConsumer(this);
    animHoverDoors_.SetTargetFunction([this] { EnableHover(true); });
    Vessel.AddControl(&switchOpen_);
    Vessel.AddControl(&status_);
}

void HoverEngines::HandlePostStep(bco::Vessel& Vessel, double simt, double simdt, double mjd)
{
    if (IsPowered()) {
        animHoverDoors_.Step(switchOpen_.IsOn() ? 1.0 : 0.0, simdt);
    }

    auto status = bco::status_display::status::off;
    if (power_.VoltsAvailable() > MIN_VOLTS) {
        if ((animHoverDoors_.GetState() > 0.0) && (animHoverDoors_.GetState() < 1.0)) {
            status = bco::status_display::status::warn;
        }
        else {
            if (animHoverDoors_.GetState() == 1.0) {
                status = bco::status_display::status::on;
            }
        }
    }
    status_.set_state(status);
}

void HoverEngines::HandleSetClassCaps(bco::Vessel& Vessel)
{
    auto vcIdx = Vessel.GetVCMeshIndex();
    auto mIdx = Vessel.GetMainMeshIndex();

    //  Hover engines : positions are simplified.
    hoverThrustHandles_[0] = Vessel.CreateThruster(
        _V(0.0, 0.0, 5.0), 
        _V(0, 1, 0), 
        HOVER_THRUST, 
        Vessel.MainPropellant(),
        THRUST_ISP);

    hoverThrustHandles_[1] = Vessel.CreateThruster(
        _V(-3.0, 0.0, -5.0), 
        _V(0, 1, 0), 
        HOVER_THRUST * 0.5, 
        Vessel.MainPropellant(),
        THRUST_ISP);

    hoverThrustHandles_[2] = Vessel.CreateThruster(
        _V(3.0, 0.0, -5.0), 
        _V(0, 1, 0), 
        HOVER_THRUST * 0.5, 
        Vessel.MainPropellant(),
        THRUST_ISP);

    Vessel.CreateThrusterGroup(hoverThrustHandles_, 3, THGROUP_HOVER);

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

    for (auto i = 0; i < 3; i++) Vessel.AddExhaust(es_hover + i);
    Vessel.AddExhaustStream(hoverThrustHandles_[0], bm::main::ThrustHoverF_loc, &exhaust_hover);
    Vessel.AddExhaustStream(hoverThrustHandles_[1], bm::main::ThrustHoverP_loc, &exhaust_hover);
    Vessel.AddExhaustStream(hoverThrustHandles_[2], bm::main::ThrustHoverS_loc, &exhaust_hover);


//    swOpen_.Setup(vessel);

    auto aid = Vessel.CreateVesselAnimation(&animHoverDoors_, 0.12);
    Vessel.AddVesselAnimationComponent(aid, mIdx, &gpFrontLeft_);
    Vessel.AddVesselAnimationComponent(aid, mIdx, &gpFrontRight_);
    Vessel.AddVesselAnimationComponent(aid, mIdx, &gpLeft_);
    Vessel.AddVesselAnimationComponent(aid, mIdx, &gpRight_);
}

bool HoverEngines::HandleLoadState(bco::Vessel& Vessel, const std::string& line)
{
    std::istringstream in(line);
    in >> switchOpen_ >> animHoverDoors_;
    Vessel.SetAnimationState(animHoverDoors_);
    return true;
}

std::string HoverEngines::HandleSaveState(bco::Vessel& Vessel)
{
    std::ostringstream os;
    os << switchOpen_ << " " << animHoverDoors_;
    return os.str();
}

void HoverEngines::EnableHover(bool isEnabled)
{
    if (isEnabled)
    {
        if (animHoverDoors_.GetState() == 1.0)
        {
            vessel_.SetThrusterResource(hoverThrustHandles_[0], vessel_.MainPropellant());
            vessel_.SetThrusterResource(hoverThrustHandles_[1], vessel_.MainPropellant());
            vessel_.SetThrusterResource(hoverThrustHandles_[2], vessel_.MainPropellant());
        }
    }
    else
    {
        vessel_.SetThrusterResource(hoverThrustHandles_[0], nullptr);
        vessel_.SetThrusterResource(hoverThrustHandles_[1], nullptr);
        vessel_.SetThrusterResource(hoverThrustHandles_[2], nullptr);
    }
}

void HoverEngines::HandleDrawHUD(bco::Vessel& Vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return;

    if (animHoverDoors_.GetState() == 0.0) return;

    if ((animHoverDoors_.GetState() == 1.0) || (fmod(oapiGetSimTime(), 1.0) < 0.5))
    {
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
