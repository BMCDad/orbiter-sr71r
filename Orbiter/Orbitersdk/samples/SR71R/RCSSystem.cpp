//	RCSSystem - SR-71r Orbiter Addon
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

#include "Orbitersdk.h"
#include "bc_orbiter\bco.h"
#include "bc_orbiter\BaseVessel.h"
#include "bc_orbiter\Tools.h"

#include "RCSSystem.h"
#include "SR71r_mesh.h"


RCSSystem::RCSSystem(bco::BaseVessel* vessel, double amps) :
PoweredComponent(vessel, amps, 20.0),
slotToggleLinear_([&](bool v) {OnChanged(RCS_LIN); slotToggleLinear_.set(); }),
slotToggleRotate_([&](bool v) {OnChanged(RCS_ROT); slotToggleRotate_.set(); })
{
}

double RCSSystem::CurrentDraw()
{
    return (GetBaseVessel()->GetAttitudeMode() == RCS_NONE) ? 0.0 : PoweredComponent::CurrentDraw();
}


void RCSSystem::ChangePowerLevel(double newLevel)
{
	PoweredComponent::ChangePowerLevel(newLevel);
	if (!HasPower())
	{
		if (GetBaseVessel()->IsCreated())
		{
			GetBaseVessel()->SetAttitudeMode(RCS_NONE);
		}
	}
}


// Callback:
void RCSSystem::OnRCSMode(int mode)
{
	if ((RCS_NONE != mode) && (!HasPower()))
	{
		GetBaseVessel()->SetAttitudeMode(RCS_NONE);
		sigIsLinear_.fire(false);
		sigIsRotate_.fire(false);
	}
	else {
		sigIsLinear_.fire(mode == RCS_LIN);
		sigIsRotate_.fire(mode == RCS_ROT);
	}
}

void RCSSystem::OnChanged(int mode)
{
	auto currentMode = GetBaseVessel()->GetAttitudeMode();

	if (GetBaseVessel()->IsCreated())
	{
		auto newMode = ((mode == currentMode) || !HasPower()) ? RCS_NONE : mode;
		GetBaseVessel()->SetAttitudeMode(newMode);
	}
}
