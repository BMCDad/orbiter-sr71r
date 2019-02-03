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

#pragma once

#include "bc_orbiter\Component.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\OnOffSwitch.h"
#include "bc_orbiter\TextureVisual.h"
#include "bc_orbiter\VCToggleSwitch.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class Shutters : public bco::Component
{
public:
	Shutters(bco::BaseVessel* vessel);

	// Component
	void SetClassCaps() override;
	bool VCRedrawEvent(int id, int event, SURFHANDLE surf) override;
	bool LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	void SaveConfiguration(FILEHANDLE scn) const override;

private:
	void Update();

	const char*				ConfigKey = "SHUTTERS";

    bco::VCToggleSwitch     swShutters_     {   bt_mesh::SR71rVC::swShutter_id,
												GetBaseVessel(),
                                                bt_mesh::SR71rVC::swShutter_location, 
                                                bt_mesh::SR71rVC::DoorsRightAxis_location
                                            };

	bco::TextureVisual		visShuttersSideLeft_;
	bco::TextureVisual		visShuttersSideRight_;

	bco::TextureVisual		visShuttersFrontLeft_;
	bco::TextureVisual		visShuttersFrontRight_;
};