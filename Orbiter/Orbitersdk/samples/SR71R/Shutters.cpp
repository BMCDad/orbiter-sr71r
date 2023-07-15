//	Shutters - SR-71r Orbiter Addon
//	Copyright(C) 2016  Blake Christensen
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

#include "Shutters.h"
#include "Orbitersdk.h"
#include "SR71r_mesh.h"

#include <assert.h>

Shutters::Shutters(bco::BaseVessel& vessel)
{
	vessel.AddControl(&switchShutters_);
	switchShutters_.attach([&]() { Update(); });
}

bool Shutters::handle_load_state(const std::string& line)
{
	//if (_strnicmp(key, ConfigKey, 8) != 0)
	//{
	//	return false;
	//}

	//int isOpen;

	//sscanf_s(configLine + 8, "%i", &isOpen);

	//// TODO (isOpen == 1) ? swShutters_.SetOn() : swShutters_.SetOff();

	return true;
}

std::string Shutters::handle_save_state()
{
	//char cbuf[256];

	//auto state = 0; // TODO = swShutters_.IsOn() ? 1 : 0;

	//sprintf_s(cbuf, "%i", state);
	//oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
	return std::string();
}

void Shutters::Update()
{
	//auto devMesh = GetBaseVessel()->GetVirtualCockpitMesh0();
	//if (devMesh == nullptr)
	//{
	//	return;
	//}

	//auto trans = swShutters_.IsOn() ? 0.22 : 0.0;
	auto trans = shuttersSlot_.value() ? 0.22 : 0.0;

	//visShuttersFrontLeft_.SetTranslate(_V(trans, 0.0, 0.0));
	//visShuttersFrontLeft_.RotateMesh(devMesh);

	//visShuttersFrontRight_.SetTranslate(_V(trans, 0.0, 0.0));
	//visShuttersFrontRight_.RotateMesh(devMesh);

	//visShuttersSideLeft_.SetTranslate(_V(trans, 0.0, 0.0));
	//visShuttersSideLeft_.RotateMesh(devMesh);

	//visShuttersSideRight_.SetTranslate(_V(0.0, trans, 0.0));
	//visShuttersSideRight_.RotateMesh(devMesh);
}