//	CarboBayController - SR-71r Orbiter Addon
//	Copyright(C) 2023  Blake Christensen
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

#include "bc_orbiter\Tools.h"

#include "Orbitersdk.h"
#include "CargoBayController.h"
#include "SR71r_mesh.h"

bool CargoBayController::CargoBayHasPower() 
{ 
    return HasPower() && slotCargoPowered_.value();
}

CargoBayController::CargoBayController(bco::BaseVessel* vessel, double amps) :
    PoweredComponent(vessel, amps, 26.0),
    slotCargoPowered_([&](bool v) { powerSwitchOn_ = v; }),
    slotCargoOpenClose_([&](bool v) {})
{
}

double CargoBayController::CurrentDraw()
{
    return ((animCargoBayDoors_.GetState() > 0.0) && (animCargoBayDoors_.GetState() < 1.0))
        ? PoweredComponent::CurrentDraw()
        : 0.0;
}

void CargoBayController::Step(double simt, double simdt, double mjd)
{
	if (CargoBayHasPower())
	{
        animCargoBayDoors_.Step(slotCargoOpenClose_.value() ? 1.0 : 0.0, simdt);
	}
}

bool CargoBayController::OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
    if (_strnicmp(key, ConfigKeyCargo, 8) != 0) return false;

    // TODO
 //   int isOn;
	//int isOpen;
	//double state;

	//sscanf_s(configLine + 8, "%d%d%lf", &isOn, &isOpen, &state);

	//swCargoPower_.SetState((isOn == 0) ? 0.0 : 1.0);
	//swCargoOpen_.SetState((isOpen == 0) ? 0.0 : 1.0);
	//animCargoBayDoors_.SetState(state);
 //   
 //   // Set the actual animation to the starting state.
 //   GetBaseVessel()->SetAnimationState(idCargoAnim_, state);

	return true;
}

void CargoBayController::OnSaveConfiguration(FILEHANDLE scn) const
{
    // TODO
	//char cbuf[256];

	//sprintf_s(cbuf, "%d %d %lf",
	//	((swCargoPower_.GetState() == 0) ? 0 : 1),
	//	((swCargoOpen_.GetState() == 0) ? 0 : 1),
	//	animCargoBayDoors_.GetState());

	//oapiWriteScenario_string(scn, (char*)ConfigKeyCargo, cbuf);
}

void CargoBayController::OnSetClassCaps()
{
    auto vessel = GetBaseVessel();
    auto mIdx = vessel->GetMainMeshIndex();

    idCargoAnim_ = vessel->CreateVesselAnimation(&animCargoBayDoors_, 0.01);
    vessel->AddVesselAnimationComponent(idCargoAnim_, mIdx, &gpCargoLeftFront_);
    vessel->AddVesselAnimationComponent(idCargoAnim_, mIdx, &gpCargoRightFront_);
    vessel->AddVesselAnimationComponent(idCargoAnim_, mIdx, &gpCargoLeftMain_);
    vessel->AddVesselAnimationComponent(idCargoAnim_, mIdx, &gpCargoRightMain_);
}