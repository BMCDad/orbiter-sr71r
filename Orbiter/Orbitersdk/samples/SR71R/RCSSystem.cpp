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
#include "bc_orbiter/vessel.h"
#include "bc_orbiter/Tools.h"

#include "RCSSystem.h"
#include "SR71r_mesh.h"


RCSSystem::RCSSystem(bco::vessel& vessel, bco::power_provider& pwr) :
	vessel_(vessel),
	power_(pwr)
{ 
	vessel_.AddControl(&btnLinear_);
	vessel_.AddControl(&btnRotate_);
	vessel_.AddControl(&lightLinear_);
	vessel_.AddControl(&lightRotate_);

	btnLinear_.attach([&]() { OnChanged(RCS_LIN); });
	btnRotate_.attach([&]() { OnChanged(RCS_ROT); });
}

void RCSSystem::ActiveChanged(bool isActive)
{
	if (!isActive) {
		if (vessel_.IsCreated())
		{
			vessel_.SetAttitudeMode(RCS_NONE);
		}
	}
}


// Callback:
void RCSSystem::OnRCSMode(int mode)
{
	if ((RCS_NONE != mode) && (!IsPowered()))
	{
		vessel_.SetAttitudeMode(RCS_NONE);
		lightLinear_.set_state(false);
		lightRotate_.set_state(false);
	}
	else {
		lightLinear_.set_state(mode == RCS_LIN);
		lightRotate_.set_state(mode == RCS_ROT);
	}
}

void RCSSystem::OnChanged(int mode)
{
	if (!vessel_.IsCreated()) return;

	auto currentMode = vessel_.GetAttitudeMode();
	auto newMode = (mode == currentMode) ? RCS_NONE : mode;
	vessel_.SetAttitudeMode(newMode);
}
