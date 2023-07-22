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

HoverEngines::HoverEngines(bco::power_provider& pwr, bco::vessel& vessel) :
    power_(pwr),
    vessel_(vessel)
{
    power_.attach_consumer(this);
    animHoverDoors_.SetTargetFunction([this] { EnableHover(true); });
}

void HoverEngines::handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd)
{
    if (IsPowered()) {
        animHoverDoors_.Step(switchOpen_.is_on() ? 1.0 : 0.0, simdt);
    }

    status_.set_state(
        IsPowered()
        ?   IsMoving()
            ?   bco::status_display::status::warn
            :   switchOpen_.is_on()
                ?   bco::status_display::status::on
                :   bco::status_display::status::off
        :   bco::status_display::status::off);
}

void HoverEngines::handle_set_class_caps(bco::vessel& vessel)
{
    vessel.AddControl(&switchOpen_);
    vessel.AddControl(&status_);

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
    vessel.AddExhaustStream(hoverThrustHandles_[0], bm::main::ThrustHoverF_location, &exhaust_hover);
    vessel.AddExhaustStream(hoverThrustHandles_[1], bm::main::ThrustHoverP_location, &exhaust_hover);
    vessel.AddExhaustStream(hoverThrustHandles_[2], bm::main::ThrustHoverS_location, &exhaust_hover);


//    swOpen_.Setup(vessel);

    auto aid = vessel.CreateVesselAnimation(&animHoverDoors_, 0.12);
    vessel.AddVesselAnimationComponent(aid, mIdx, &gpFrontLeft_);
    vessel.AddVesselAnimationComponent(aid, mIdx, &gpFrontRight_);
    vessel.AddVesselAnimationComponent(aid, mIdx, &gpLeft_);
    vessel.AddVesselAnimationComponent(aid, mIdx, &gpRight_);
}

bool HoverEngines::handle_load_state(bco::vessel& vessel, const std::string& line)
{
    std::istringstream in(line);
    in >> switchOpen_ >> animHoverDoors_;
    vessel.SetAnimationState(animHoverDoors_);
    return true;
}

std::string HoverEngines::handle_save_state(bco::vessel& vessel)
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

void HoverEngines::handle_draw_hud(bco::vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
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
//    bco::DrawPanelOnOff(GetBaseVessel()->GetpanelMeshHandle0(), bm::pnl::pnlDoorHover_id, bm::pnl::pnlDoorHover_verts, swOpen_.IsOn(), 0.0148);
//    return true;
//}
