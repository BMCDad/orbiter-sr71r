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
	virtual void OnSetClassCaps() override;
	virtual bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf) override { return false; }

	bool OnLoadPanel2D(int id, PANELHANDLE hPanel) override;
	bool OnPanelMouseEvent(int id, int event) override;
	bool OnPanelRedrawEvent(int id, int event, SURFHANDLE surf) override;

	virtual bool OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	virtual void OnSaveConfiguration(FILEHANDLE scn) const override;

	virtual double CurrentDraw() override;

	void Step(double simt, double simdt, double mjd);


private:
	void Update();

	const char* ConfigKey = "LIGHTS";

	double                  ampsPerLight_;

	//bco::VCToggleSwitch     swNav_              {   bm::vc::SwitchNavLights_id, 
	//                                                bm::vc::SwitchNavLights_location, 
	//                                                bm::vc::LightsRightAxis_location
	//                                            };

	bco::VCToggleSwitch     swBeacon_{ bm::vc::SwitchBeaconLights_id,
													bm::vc::SwitchBeaconLights_location,
													bm::vc::LightsRightAxis_location
	};

	bco::VCToggleSwitch     swStrobe_{ bm::vc::SwitchStrobeLights_id,
													bm::vc::SwitchStrobeLights_location,
													bm::vc::LightsRightAxis_location
	};

	bco::VCToggleSwitch     swDock_{ bm::vc::SwitchDockLights_id,
													bm::vc::SwitchDockLights_location,
													bm::vc::LightsRightAxis_location
	};


	BEACONLIGHTSPEC			specBeaconTop_;
	BEACONLIGHTSPEC			specBeaconBottom_;

	BEACONLIGHTSPEC			specNavLeft_;
	BEACONLIGHTSPEC			specNavRight_;
	BEACONLIGHTSPEC			specNavRear_;

	BEACONLIGHTSPEC			specStrobeLeft_;
	BEACONLIGHTSPEC			specStrobeRight_;

	struct PnlData
	{
		const UINT group;
		const RECT rc;
		const NTVERTEX* verts;
		std::function<void(void)> update;
		std::function<bool(void)> isActive;
	};

	const int ID_NAV = GetBaseVessel()->GetIdForComponent(this);
	const int ID_BEACON = GetBaseVessel()->GetIdForComponent(this);
	const int ID_STROBE = GetBaseVessel()->GetIdForComponent(this);
	const int ID_DOCK = GetBaseVessel()->GetIdForComponent(this);

	std::map<int, PnlData> pnlData_
	{
		//		{ID_NAV,	{bm::pnl::pnlLightNav_id,	 bm::pnl::pnlLightNav_RC,	 bm::pnl::pnlLightNav_verts,	[&]() {swNav_.Toggle(); },	  [&]() {return swNav_.IsOn(); }}},
				{ID_BEACON,	{bm::pnl::pnlLightBeacon_id, bm::pnl::pnlLightBeacon_RC, bm::pnl::pnlLightBeacon_verts, [&]() {swBeacon_.Toggle(); }, [&]() {return swBeacon_.IsOn(); }}},
				{ID_STROBE, {bm::pnl::pnlLightStrobe_id, bm::pnl::pnlLightStrobe_RC, bm::pnl::pnlLightStrobe_verts,	[&]() {swStrobe_.Toggle(); }, [&]() {return swStrobe_.IsOn(); }}},
				{ID_DOCK,	{bm::pnl::pnlLightDock_id,	 bm::pnl::pnlLightDock_RC,	 bm::pnl::pnlLightDock_verts,	[&]() {swDock_.Toggle(); },	  [&]() {return swDock_.IsOn(); }}}
	};
};

class NavLight : public bco::IInit {
public:
	NavLight(bco::OnOffToggle& ctrl) : control_(ctrl)
	{
		control_.Subscribe([&](double d) {
			SetActive(d);
			});

		SetActive(control_.GetState());
	}

	// IInit
	void Init(bco::BaseVessel& vessel) {
		vessel.AddBeacon(&specNavLeft_);
		vessel.AddBeacon(&specNavRight_);
		vessel.AddBeacon(&specNavRear_);
	}

private:

	void SetActive(double d) {
		auto active = (d != 0.0);
		specNavLeft_.active = active;
		specNavRear_.active = active;
		specNavRight_.active = active;
	}

	bco::OnOffToggle& control_;

	// Set light specs:
	VECTOR3 colRed{ 1.0, 0.5, 0.5 };
	VECTOR3 colGreen{ 0.5, 1.0, 0.5 };
	VECTOR3 colWhite{ 1.0, 1.0, 1.0 };

	BEACONLIGHTSPEC			specNavLeft_{
		BEACONSHAPE_DIFFUSE,
		const_cast<VECTOR3*>(&bm::main::NavLightP_location),
		&colRed,		// color
		0.3,			// size
		0.4,			// falloff
		0.0,			// period
		0.1,			// duration
		0.2,			// tofs
		false,			// active
	};

	BEACONLIGHTSPEC			specNavRight_{
		BEACONSHAPE_DIFFUSE,
		const_cast<VECTOR3*>(&bm::main::NavLightS_location),
		&colGreen,		// color
		0.3,			// size
		0.4,			// falloff
		0.0,			// period
		0.1,			// duration
		0.2,			// tofs
		false,			// active
	};

	BEACONLIGHTSPEC			specNavRear_{
		BEACONSHAPE_DIFFUSE,
		const_cast<VECTOR3*>(&bm::main::NavLightTail_location),
		&colWhite,		// color
		0.3,			// size
		0.4,			// falloff
		0.0,			// period
		0.1,			// duration
		0.2,			// tofs
		false,			// active
	};
};