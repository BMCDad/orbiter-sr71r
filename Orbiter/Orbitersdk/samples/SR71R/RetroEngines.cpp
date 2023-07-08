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

#include "StdAfx.h"

#include "RetroEngines.h"
#include "Orbitersdk.h"
#include "ShipMets.h"
#include "SR71r_mesh.h"


RetroEngines::RetroEngines() : 
    retroDoorsSlot_([&](bool v) { if (!v) { EnableRetros(false); } })
{
    animRetroDoors_.SetTargetFunction([this] {EnableRetros(true); });
}


void RetroEngines::handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd)
{
    if (slotVoltsInput_.value() > MIN_VOLTS) {
        animRetroDoors_.Step(retroDoorsSlot_.value() ? 1.0 : 0.0, simdt);
    }
}

void RetroEngines::handle_set_class_caps(bco::BaseVessel& vessel)
{
    auto main = vessel.GetMainMeshIndex();

    auto aid = vessel.CreateVesselAnimation(&animRetroDoors_, 0.2);
    vessel.AddVesselAnimationComponent(aid, main, &gpDoors_);


    retroThrustHandles_[0] = vessel.CreateThruster(
        bm::main::ThrustRetroP_location,
        _V(0, 0, -1),
        RETRO_THRUST,
        vessel.MainPropellant(),
        THRUST_ISP);

    retroThrustHandles_[1] = vessel.CreateThruster(
        bm::main::ThrustRetroS_location,
        _V(0, 0, -1),
        RETRO_THRUST,
        vessel.MainPropellant(),
        THRUST_ISP);

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

void RetroEngines::EnableRetros(bool isEnabled)
{
    if (isEnabled)
    {
        // Enable retros if doors are open.
        if (animRetroDoors_.GetState() == 1.0)
        {
            // TODO
            //GetBaseVessel()->SetThrusterResource(retroThrustHandles_[0], GetBaseVessel()->MainPropellant());
            //GetBaseVessel()->SetThrusterResource(retroThrustHandles_[1], GetBaseVessel()->MainPropellant());
        }
    }
    else
    {
        // Disable retros, regardless of door position.
        //GetBaseVessel()->SetThrusterResource(retroThrustHandles_[0], nullptr);
        //GetBaseVessel()->SetThrusterResource(retroThrustHandles_[1], nullptr);
    }
}

void RetroEngines::handle_draw_hud(bco::BaseVessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return;

    if (animRetroDoors_.GetState() == 0.0) return;

    if ((animRetroDoors_.GetState() == 1.0) || (fmod(oapiGetSimTime(), 1.0) < 0.5))
    {
        int halfFoot = 8;
        int ySep = 10;
        int xSep = 35;

        int xCenter = hps->W / 2;
        int yCenter = hps->H / 2;

        // Right
        int ptx = xCenter + xSep;
        int ptxl = ptx - 10;
        int ptxr = ptx + 10;
        int pty = yCenter - ySep;
        int ptyt = pty - 20;

        skp->Line(ptx, pty, ptx, ptyt);
        skp->Line(ptx, pty, ptxl, ptyt);
        skp->Line(ptx, pty, ptxr, ptyt);

        // Left
        ptx = xCenter - xSep;
        ptxl = ptx - 10;
        ptxr = ptx + 10;

        skp->Line(ptx, pty, ptx, ptyt);
        skp->Line(ptx, pty, ptxl, ptyt);
        skp->Line(ptx, pty, ptxr, ptyt);
    }
}