//	O2Supply - SR-71r Orbiter Addon
//	Copyright(C) 2015  Blake Christensen
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

#include "bc_orbiter\Animation.h"
#include "bc_orbiter\IAnimationState.h"
#include "bc_orbiter\OnOffSwitch.h"
#include "bc_orbiter\TextureVisual.h"
#include "bc_orbiter\PushButtonSwitch.h"
#include "bc_orbiter\VCGauge.h"

#include "CryogenicTank.h"
#include "SR71r_mesh.h"

#include <string>


namespace bco = bc_orbiter;

/**
	O2Supply models the liquid oxygen supply for the vessel.  Consumers of O2
	should call the 'Draw' method on O2Supply.  Draw will return 0.0 when the
	tank is empty, or if the O2 tank does not have power.

	The O2 tank can be refueled when the O2 'AVAIL' light is on.  This will come
	on when the vessel is at rest, or docked and recieving power.  To refill the
	O2 tank, press the 'FILL' button when the 'AVAIL' light is on.
	
	Configuration:
	OXYGEN a b
	a = Tank level 0.0-empty, 1.0-full
	b = Fill pump 1-on, 0-off.

	Notes:
	This will eventually supply both the
	fuel cell and the pilot oxygen.  The supply is liquid oxygen. According to
	SR71 manual, 1 person will consume about 1/2 liter per hour.  So that would 
	be a consumption rate of 0.000138 liters per second.
*/
class O2Supply : public CryogenicTank,
	public bco::IAnimationState
{
public:
	O2Supply(bco::BaseVessel* vessel, double capacity);

	virtual void SetClassCaps() override;
	virtual bool LoadVC(int id) override;
	virtual bool VCRedrawEvent(int id, int event, SURFHANDLE surf) override;

	void Step(double simt, double simdt, double mjd);

	// Animation state
	virtual double GetState() const override { return GetLevel(); }
private:
	void					Update();

	int						areaId_;
	int						pumpMouseId_;
	double					prevTime_;
	
	const char*				ConfigKey = "OXYGEN";

	bco::TextureVisual		lightSupplyAvailable_;
	bco::TextureVisual		lightValveOpen_;
    bco::PushButtonSwitch	switchValveOpen_    { bt_mesh::SR71rVC::LOXValveOpenSwitch_location, 0.01 };

    bco::VCGauge            gaugeOxygenLevel_   { {bt_mesh::SR71rVC::gaugeOxygenLevel_id },
                                                    bt_mesh::SR71rVC::gaugeOxygenLevel_location,     
                                                    bt_mesh::SR71rVC::axisOxygenLevel_location,
                                                    (300 * RAD), 
                                                    0.2
                                                };
};