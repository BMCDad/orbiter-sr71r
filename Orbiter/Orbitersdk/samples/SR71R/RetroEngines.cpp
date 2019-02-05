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


RetroEngines::RetroEngines(bco::BaseVessel* vessel, double amps) : 
    PoweredComponent(vessel, amps, 10)
{
    swRetroDoors_.OnFunction([this] {});
    swRetroDoors_.OffFunction([this] {EnableRetros(false); });
    animRetroDoors_.SetTargetFunction([this] {EnableRetros(true); });
}


void RetroEngines::Step(double simt, double simdt, double mjd)
{
    if (HasPower())
    {
        animRetroDoors_.Step(swRetroDoors_.GetState(), simdt);
    }

	SetSound();
}

double RetroEngines::CurrentDraw()
{
    return (HasPower() && ((animRetroDoors_.GetState() > 0.0) && (animRetroDoors_.GetState() < 1.0)))
        ? PoweredComponent::CurrentDraw()
        : 0.0;
}

void RetroEngines::SetClassCaps()
{
    auto vessel = GetBaseVessel();
    auto main = vessel->GetMainMeshIndex();

    swRetroDoors_.Setup(vessel);

    auto aid = vessel->CreateVesselAnimation(&animRetroDoors_, 0.2);
    vessel->AddVesselAnimationComponent(aid, main, &gpDoors_);


    retroThrustHandles_[0] = vessel->CreateThruster(
        bt_mesh::SR71r::ThrustRetroP_location,
        _V(0, 0, -1),
        RETRO_THRUST,
        vessel->MainPropellant(),
        THRUST_ISP);

    retroThrustHandles_[1] = vessel->CreateThruster(
        bt_mesh::SR71r::ThrustRetroS_location,
        _V(0, 0, -1),
        RETRO_THRUST,
        vessel->MainPropellant(),
        THRUST_ISP);

    vessel->CreateThrusterGroup(retroThrustHandles_, 2, THGROUP_RETRO);

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

    for (auto i = 0; i < 2; i++) vessel->AddExhaust(es_retro + i);
    vessel->AddExhaustStream(retroThrustHandles_[0], _V(-4.38, 0, 3), &exhaust_retro);
    vessel->AddExhaustStream(retroThrustHandles_[1], _V(4.38, 0, 3), &exhaust_retro);
}

bool RetroEngines::LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
    if (_strnicmp(key, ConfigKey, 5) != 0)
    {
        return false;
    }

    int hover;
    double hoveranim;

    sscanf_s(configLine + 5, "%i%lf", &hover, &hoveranim);

    swRetroDoors_.SetState((hover == 0) ? 0.0 : 1.0);
    animRetroDoors_.SetState(hoveranim);

    EnableRetros(swRetroDoors_.IsOn());

    return true;
}

void RetroEngines::SaveConfiguration(FILEHANDLE scn) const
{
    char cbuf[256];
    auto val = (swRetroDoors_.GetState() == 0.0) ? 0 : 1;

    auto hover = animRetroDoors_.GetState();

    sprintf_s(cbuf, "%i %0.3lf", val, hover);
    oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}

void RetroEngines::EnableRetros(bool isEnabled)
{
    if (isEnabled)
    {
        // Enable retros if doors are open.
        if (animRetroDoors_.GetState() == 1.0)
        {
            GetBaseVessel()->SetThrusterResource(retroThrustHandles_[0], GetBaseVessel()->MainPropellant());
            GetBaseVessel()->SetThrusterResource(retroThrustHandles_[1], GetBaseVessel()->MainPropellant());
        }
    }
    else
    {
        // Disable retros, regardless of door position.
        GetBaseVessel()->SetThrusterResource(retroThrustHandles_[0], nullptr);
        GetBaseVessel()->SetThrusterResource(retroThrustHandles_[1], nullptr);
    }
}

void RetroEngines::SetSound()
{
	if (animRetroDoors_.GetState() > 0.0 && animRetroDoors_.GetState() < 1.0) {
		if (!GetBaseVessel()->IsSoundRunning(RETRO_DOOR_ID)) GetBaseVessel()->PlaySound(RETRO_DOOR_ID, true, false);
	}
	else {
		if (GetBaseVessel()->IsSoundRunning(RETRO_DOOR_ID)) GetBaseVessel()->StopSound(RETRO_DOOR_ID);
	}
}

bool RetroEngines::DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return false;

    if (animRetroDoors_.GetState() == 0.0) return false;

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

        return true;
    }

    return false;
}