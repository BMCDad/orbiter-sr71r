//	CargoBayController - SR-71r Orbiter Addon
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

#include "OrbiterSDK.h"

#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\OnOffSwitch.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\VCToggleSwitch.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

/**	CargoBayController
	Controls the cargo bay doors.
	Draws from the main power circuit.  The power draw only
	happens when the doors are in motion.  That draw can be fairly high so
	try to operate one or the other at a time.

	To operate the cargo bay:
	- Power the main circuit (external or fuel cell).
	- Turn 'Main' power on (up).
	- Turn 'Cargo bay' (CRGO) power switch (right panel) on (up).
	- To Open switch 'Cargo Bay' switch (left panel) to OPEN.
	- The message board [BAY] light will show orange while the cargo bay doors
	are in motion, and white when fully open.

	Configuration:
	CARGOBAY a b c
	a - 0/1 Power switch off/on.
	b - 0/1 Open close switch closed/open.
	c - 0.0-1.0 Current door position.
*/
class CargoBayController : public bco::PoweredComponent
{
public:
	CargoBayController(bco::BaseVessel* vessel, double amps);

	virtual void OnSetClassCaps() override;
	virtual bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf) override { return false; }
	virtual bool OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	virtual void OnSaveConfiguration(FILEHANDLE scn) const override;

	/**
		We override from the base class because the calculation for this
		is determined by which doors are currently in motion.
	*/
	virtual double CurrentDraw() override;

	/**
		Provide time steps for the animations.
	*/
	void Step(double simt, double simdt, double mjd);


	// ICargoBay:
	bco::OnOffSwitch&	CargoBayDoorsPowerSwitch();
	bco::OnOffSwitch&	CargoBayDoorsOpenSwitch();

    double				GetCargoBayState();

	// Callbacks

private:
	bool CargoBayHasPower();

    const char*			    ConfigKeyCargo = "CARGOBAY";

    bco::Animation		    animCargoBayDoors_{ &swCargoOpen_, 0.01 };

    UINT idCargoAnim_{ 0 };

    bco::VCToggleSwitch     swCargoPower_       {   bt_mesh::SR71rVC::SwCargoPower_id,
                                                    bt_mesh::SR71rVC::SwCargoPower_location, 
                                                    bt_mesh::SR71rVC::PowerTopRightAxis_location
                                                };

    bco::VCToggleSwitch     swCargoOpen_        {   bt_mesh::SR71rVC::SwCargoOpen_id,
                                                    bt_mesh::SR71rVC::SwCargoOpen_location,
                                                    bt_mesh::SR71rVC::DoorsRightAxis_location
                                                };

    bco::AnimationGroup     gpCargoLeftFront_   {   { bt_mesh::SR71r::BayDoorPF_id },
                                                    bt_mesh::SR71r::Bay1AxisPA_location, bt_mesh::SR71r::Bay1AxisPF_location,
                                                    (160 * RAD),
                                                    0.51, 0.74
                                                };

    bco::AnimationGroup     gpCargoRightFront_  {   { bt_mesh::SR71r::BayDoorSF_id },
                                                    bt_mesh::SR71r::Bay1AxisSF_location, bt_mesh::SR71r::Bay1AxisSA_location,
                                                    (160 * RAD),
                                                    0.76, 1.0
                                                };

    bco::AnimationGroup     gpCargoLeftMain_    {   { bt_mesh::SR71r::BayDoorPA_id },
                                                    bt_mesh::SR71r::Bay2AxisPA_location, bt_mesh::SR71r::Bay2AxisPF_location,
                                                    (160 * RAD),
                                                    0.0, 0.24
                                                };

    bco::AnimationGroup     gpCargoRightMain_   {   { bt_mesh::SR71r::BayDoorSA_id },
                                                    bt_mesh::SR71r::Bay2AxisSF_location, bt_mesh::SR71r::Bay2AxisSA_location,
                                                    (160 * RAD),
                                                    0.26, 0.49
                                                };
};