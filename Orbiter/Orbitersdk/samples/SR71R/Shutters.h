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
	void OnSetClassCaps() override;
	bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf) override;
	bool OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	void OnSaveConfiguration(FILEHANDLE scn) const override;

private:
	void Update();

	const char*				ConfigKey = "SHUTTERS";

    bco::VCToggleSwitch     swShutters_     {   bm::vc::swShutter_id, 
                                                bm::vc::swShutter_location, 
                                                bm::vc::DoorsRightAxis_location
                                            };

	bco::TextureVisual		visShuttersSideLeft_;
	bco::TextureVisual		visShuttersSideRight_;

	bco::TextureVisual		visShuttersFrontLeft_;
	bco::TextureVisual		visShuttersFrontRight_;
};