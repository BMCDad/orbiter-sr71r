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

HoverEngines::HoverEngines(bco::BaseVessel* vessel, double amps) :
    PoweredComponent(vessel, amps, 10.0)
{
    // Hover
    swOpen_.OnFunction([this] {});
    swOpen_.OffFunction([this] { EnableHover(false); });
    animHoverDoors_.SetTargetFunction([this] { EnableHover(true); });
}

void HoverEngines::Step(double simt, double simdt, double mjd)
{
    if (HasPower())
    {
        animHoverDoors_.Step(swOpen_.GetState(), simdt);
    }
}

double HoverEngines::CurrentDraw()
{
    return (HasPower() && ((animHoverDoors_.GetState() > 0.0) && (animHoverDoors_.GetState() < 1.0))) 
        ? PoweredComponent::CurrentDraw() 
        : 0.0;
}

void HoverEngines::SetClassCaps()
{
    auto vessel = GetBaseVessel();
    auto vcIdx = vessel->GetVCMeshIndex();
    auto mIdx = vessel->GetMainMeshIndex();

    //  Hover engines : positions are simplified.
    hoverThrustHandles_[0] = vessel->CreateThruster(
        _V(0.0, 0.0, 5.0), 
        _V(0, 1, 0), 
        HOVER_THRUST, 
        GetBaseVessel()->MainPropellant(), 
        THRUST_ISP);

    hoverThrustHandles_[1] = vessel->CreateThruster(
        _V(-3.0, 0.0, -5.0), 
        _V(0, 1, 0), 
        HOVER_THRUST * 0.5, 
        GetBaseVessel()->MainPropellant(), 
        THRUST_ISP);

    hoverThrustHandles_[2] = vessel->CreateThruster(
        _V(3.0, 0.0, -5.0), 
        _V(0, 1, 0), 
        HOVER_THRUST * 0.5, 
        GetBaseVessel()->MainPropellant(), 
        THRUST_ISP);

    vessel->CreateThrusterGroup(hoverThrustHandles_, 3, THGROUP_HOVER);

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

    for (auto i = 0; i < 3; i++) vessel->AddExhaust(es_hover + i);
    vessel->AddExhaustStream(hoverThrustHandles_[0], bt_mesh::SR71r::ThrustHoverF_location, &exhaust_hover);
    vessel->AddExhaustStream(hoverThrustHandles_[1], bt_mesh::SR71r::ThrustHoverP_location, &exhaust_hover);
    vessel->AddExhaustStream(hoverThrustHandles_[2], bt_mesh::SR71r::ThrustHoverS_location, &exhaust_hover);


    swOpen_.Setup(vessel);

    auto aid = vessel->CreateVesselAnimation(&animHoverDoors_, 0.12);
    vessel->AddVesselAnimationComponent(aid, mIdx, &gpFrontLeft_);
    vessel->AddVesselAnimationComponent(aid, mIdx, &gpFrontRight_);
    vessel->AddVesselAnimationComponent(aid, mIdx, &gpLeft_);
    vessel->AddVesselAnimationComponent(aid, mIdx, &gpRight_);
}

bool HoverEngines::LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
    if (_strnicmp(key, ConfigKey, 5) != 0)
    {
        return false;
    }

    int hover;
    double hoveranim;

    sscanf_s(configLine + 5, "%i%lf", &hover, &hoveranim);

    // Hover
    swOpen_.SetState((hover == 0) ? 0.0 : 1.0);
    animHoverDoors_.SetState(hoveranim);

    EnableHover(swOpen_.IsOn());

    return true;
}

void HoverEngines::SaveConfiguration(FILEHANDLE scn) const
{
    char cbuf[256];
    auto val = (swOpen_.GetState() == 0.0) ? 0 : 1;

    auto hover = animHoverDoors_.GetState();

    sprintf_s(cbuf, "%i %0.3lf", val, hover);
    oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}

void HoverEngines::EnableHover(bool isEnabled)
{
    if (isEnabled)
    {
        if (animHoverDoors_.GetState() == 1.0)
        {
            GetBaseVessel()->SetThrusterResource(hoverThrustHandles_[0], GetBaseVessel()->MainPropellant());
            GetBaseVessel()->SetThrusterResource(hoverThrustHandles_[1], GetBaseVessel()->MainPropellant());
            GetBaseVessel()->SetThrusterResource(hoverThrustHandles_[2], GetBaseVessel()->MainPropellant());
        }
    }
    else
    {
        GetBaseVessel()->SetThrusterResource(hoverThrustHandles_[0], nullptr);
        GetBaseVessel()->SetThrusterResource(hoverThrustHandles_[1], nullptr);
        GetBaseVessel()->SetThrusterResource(hoverThrustHandles_[2], nullptr);
    }
}

bool HoverEngines::DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return false;

    if (animHoverDoors_.GetState() == 0.0) return false;

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

        return true;
    }

    return false;
}