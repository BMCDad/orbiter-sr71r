//	Lights - SR-71r Orbiter Addon
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

#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\OnOffSwitch.h"
#include "bc_orbiter\VCToggleSwitch.h"

#include "SR71r_mesh.h"

#include <map>

namespace bco = bc_orbiter;

class VESSEL;

/**	Lights

	Manage the vessel's Navigation, Beacon, and Strobe lights.

	Configuration:
	LIGHTS a b c
	a = 0/1 Navigation on/off
	b = 0/1 Beacon on/off
	c = 0/1 Strobe on/off
*/
class Lights : public bco::PoweredComponent
{
public:
	Lights(bco::BaseVessel* vessel, double amps);

	// Component
	virtual void SetClassCaps() override;
	virtual bool VCRedrawEvent(int id, int event, SURFHANDLE surf) override { return false; }
	virtual bool LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	virtual void SaveConfiguration(FILEHANDLE scn) const override;

	virtual double CurrentDraw() override;

	void Step(double simt, double simdt, double mjd);


private:
	void Update();

	const char*				ConfigKey = "LIGHTS";

    double                  ampsPerLight_;

    bco::AnimationGroup     rnavPower           {   {bt_mesh::SR71rVC::SwitchNavLights_id },
                                                    bt_mesh::SR71rVC::LightsRightAxis_location, 
													bt_mesh::SR71rVC::LightsRightAxis_location,
                                                    (90 * RAD), 0.0, 0.1 };

    bco::VCToggleSwitch     swNav_              {   bt_mesh::SR71rVC::SwitchNavLights_id, 
                                                    bt_mesh::SR71rVC::SwitchNavLights_location, 
                                                    bt_mesh::SR71rVC::LightsRightAxis_location
                                                };

    bco::VCToggleSwitch     swBeacon_           {   bt_mesh::SR71rVC::SwitchBeaconLights_id,
                                                    bt_mesh::SR71rVC::SwitchBeaconLights_location,
                                                    bt_mesh::SR71rVC::LightsRightAxis_location
                                                };

    bco::VCToggleSwitch     swStrobe_           {   bt_mesh::SR71rVC::SwitchStrobeLights_id,
                                                    bt_mesh::SR71rVC::SwitchStrobeLights_location,
                                                    bt_mesh::SR71rVC::LightsRightAxis_location
                                                };

    bco::VCToggleSwitch     swDock_             {   bt_mesh::SR71rVC::SwitchDockLights_id,
                                                    bt_mesh::SR71rVC::SwitchDockLights_location,
                                                    bt_mesh::SR71rVC::LightsRightAxis_location
                                                };


	BEACONLIGHTSPEC			specBeaconTop_;
	BEACONLIGHTSPEC			specBeaconBottom_;

	BEACONLIGHTSPEC			specNavLeft_;
	BEACONLIGHTSPEC			specNavRight_;
	BEACONLIGHTSPEC			specNavRear_;

	BEACONLIGHTSPEC			specStrobeLeft_;
	BEACONLIGHTSPEC			specStrobeRight_;
};