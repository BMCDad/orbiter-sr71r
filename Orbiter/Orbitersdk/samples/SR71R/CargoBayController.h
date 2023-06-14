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
	c - 0.0-1.0 current door position.
*/
class CargoBayController : public bco::PoweredComponent
{
public:
	CargoBayController(bco::BaseVessel* vessel, double amps);

	virtual void OnSetClassCaps() override;
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

//    double				GetCargoBayState();

    bco::slot<bool>& PowerEnabledSlot() { return slotCargoPowered_; }
    bco::slot<bool>& CargoOpenSlot() { return slotCargoOpenClose_; }

private:
	bool CargoBayHasPower();

    const char*			    ConfigKeyCargo = "CARGOBAY";

    bco::slot<bool>         slotCargoPowered_;
    bco::slot<bool>         slotCargoOpenClose_;

    bool                    powerSwitchOn_{ false };

    bco::Animation		    animCargoBayDoors_{ 0.01 };

    UINT idCargoAnim_{ 0 };

    bco::AnimationGroup     gpCargoLeftFront_   {   { bm::main::BayDoorPF_id },
                                                    bm::main::Bay1AxisPA_location, bm::main::Bay1AxisPF_location,
                                                    (160 * RAD),
                                                    0.51, 0.74
                                                };

    bco::AnimationGroup     gpCargoRightFront_  {   { bm::main::BayDoorSF_id },
                                                    bm::main::Bay1AxisSF_location, bm::main::Bay1AxisSA_location,
                                                    (160 * RAD),
                                                    0.76, 1.0
                                                };

    bco::AnimationGroup     gpCargoLeftMain_    {   { bm::main::BayDoorPA_id },
                                                    bm::main::Bay2AxisPA_location, bm::main::Bay2AxisPF_location,
                                                    (160 * RAD),
                                                    0.0, 0.24
                                                };

    bco::AnimationGroup     gpCargoRightMain_   {   { bm::main::BayDoorSA_id },
                                                    bm::main::Bay2AxisSF_location, bm::main::Bay2AxisSA_location,
                                                    (160 * RAD),
                                                    0.26, 0.49
                                                };
};