//	HydrogenSupply - SR-71r Orbiter Addon
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

namespace bco = bc_orbiter;

/** Hydrogen Supply
	Models the hydrogen supply which is used by the fuel cell.
	This is a cryogenic tank and requires power from the main circuit to function.  The
	amp draw is fixed as long as there is power in the main circuit.  There is no power 
	switch.  Hydrogen will be available to the fuel cell as long as the tank has power
	and the supply of hydrogen has not been exhuasted.

	The hydrogen level can ge seen on the HYDRO gauge on the left panel.

	Capacity is 10 liters.

	Configuration:
	HYDROGEN a
	a - level 0.0 - 1.0 empty - full.
*/
class HydrogenSupply : 
	public CryogenicTank,
	public bco::IAnimationState
{
public:
	HydrogenSupply(bco::BaseVessel* vessel, double capacity);

	virtual void SetClassCaps() override;
	virtual bool LoadVC(int id) override;
	virtual bool VCRedrawEvent(int id, int event, SURFHANDLE surf) override;

	void Step(double simt, double simdt, double mjd);

	// Animation state
	virtual double GetState() const override { return GetLevel(); }
private:
	void					Update();

	int						areaId_;
    double					prevTime_               { 0.0 };

	bco::TextureVisual		lightSupplyAvailable_;
	bco::TextureVisual		lightValveOpen_;
    bco::PushButtonSwitch	switchValveOpen_        { bt_mesh::SR71rVC::LH2ValveOpenSwitch_location,     0.01 };

    bco::VCGauge            gaugeHydro_{ {bt_mesh::SR71rVC::gaHydrogenLevel_id },
                                                        bt_mesh::SR71rVC::gaHydrogenLevel_location, 
                                                        bt_mesh::SR71rVC::axisHydrogenLevel_location,
                                                        (300 * RAD), 
                                                        0.2
                                                    };
};