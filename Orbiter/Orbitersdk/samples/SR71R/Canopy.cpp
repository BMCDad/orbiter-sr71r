//	Canopy - SR-71r Orbiter Addon
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

#include "bc_orbiter/Tools.h"
#include "Orbitersdk.h"
#include "Canopy.h"
#include "SR71r_mesh.h"

//bco::OnOffSwitch& Canopy::CanopyPowerSwitch() { return swCanopyPower_; }
//bco::OnOffSwitch& Canopy::CanopyOpenSwitch() { return swCanopyOpen_; }

double Canopy::GetCanopyState()
{
    return 0.0;// TODO animCanopy_.GetState();
}

bool Canopy::CanopyHasPower()
{
    return HasPower() && powerSwitchOn_;
}

Canopy::Canopy(bco::BaseVessel* vessel, double amps) :
    PoweredComponent(vessel, amps, 26.0),
    slotCanopyPowered_([&](bool v) { powerSwitchOn_ = v; }),
    slotCanopyOpenClose_([&](bool v) {})
{
}

double Canopy::CurrentDraw()
{
    return ( CanopyHasPower() && CanopyIsMoving())
        ? PoweredComponent::CurrentDraw() 
        : 0.0;
}

void Canopy::Step(double simt, double simdt, double mjd)
{
    if (CanopyHasPower())
    {
        animCanopy_.Step(slotCanopyOpenClose_.value() ? 1.0 : 0.0, simdt);
    }
}

bool Canopy::OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
    if (_strnicmp(key, ConfigKeyCanopy, 6) != 0) return false;

    int isOn;
    int isOpen;
    double state;

    sscanf_s(configLine + 6, "%d%d%lf", &isOn, &isOpen, &state);

    // TODO
    //swCanopyPower_.SetState((isOn == 0) ? 0.0 : 1.0);
    //swCanopyOpen_.SetState((isOpen == 0) ? 0.0 : 1.0);
    //
    //animCanopy_.SetState(state);
    GetBaseVessel()->SetAnimationState(idAnim_, state);

    return true;
}

void Canopy::OnSaveConfiguration(FILEHANDLE scn) const
{
    char cbuf[256];

    // TODO
    //sprintf_s(cbuf, "%d %d %lf",
    //    ((swCanopyPower_.GetState() == 0) ? 0 : 1),
    //    ((swCanopyOpen_.GetState() == 0) ? 0 : 1),
    //    animCanopy_.GetState());

    oapiWriteScenario_string(scn, (char*)ConfigKeyCanopy, cbuf);
}
void Canopy::OnSetClassCaps()
{
    auto vessel = GetBaseVessel();
    auto vcIdx = vessel->GetVCMeshIndex();
    auto mIdx = vessel->GetMainMeshIndex();

    idAnim_ = vessel->CreateVesselAnimation(&animCanopy_, 0.2);
    vessel->AddVesselAnimationComponent(idAnim_, vcIdx, &gpCanopyVC_);
    vessel->AddVesselAnimationComponent(idAnim_, mIdx, &gpCanopy_);
}