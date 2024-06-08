//	SR-71r Orbiter Addon
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

#include "StdAfx.h"

#include "SR71Vessel.h"
#include "LiftCoeff.h"
#include "ShipMets.h"

SR71Vessel::SR71Vessel(OBJHANDLE hvessel, int flightmodel) : 
	bco::vessel(hvessel, flightmodel),
	meshVirtualCockpit_(nullptr)
{
    apMFDId_ = RegisterAPMFD();

	AddComponent(&avionics_);
	AddComponent(&airBrake_);
	AddComponent(&airspeed_);
	AddComponent(&altimeter_);
	AddComponent(&apu_);
	AddComponent(&canopy_);
	AddComponent(&cargobay_);
	AddComponent(&clock_);
	AddComponent(&fuelCell_);
	AddComponent(&headsUpDisplay_);
	AddComponent(&hoverEngines_);
	AddComponent(&hydrogenTank_);
	AddComponent(&hsi_);
	AddComponent(&landingGear_);
	AddComponent(&navModes_);
	AddComponent(&oxygenTank_);
	AddComponent(&propulsion_);
	AddComponent(&powerSystem_);
	AddComponent(&retroEngines_);
	AddComponent(&lights_);

	AddComponent(&mfdLeft_);
	AddComponent(&mfdRight_);
//	AddComponent(&computer_);
	AddComponent(&surfaceCtrl_);

	// Fuel cell					// A signal can drive more then one slot
	bco::connect( fuelCell_.AvailablePowerSignal(),			powerSystem_.FuelCellAvailablePowerSlot());

	bco::connect( propulsion_.MainFuelLevelSignal(),		apu_.FuelLevelSlot());

	// RCS

	// ...which in turn drive the HSI course and heading
	bco::connect( avionics_.SetCourseSignal(),				hsi_.SetCourseSlot());
	bco::connect( avionics_.SetHeadingSignal(),				hsi_.SetHeadingSlot());
	//bco::connect( avionics_.SetHeadingSignal(),				computer_.HeadingSlot());
	//bco::connect( computer_.HeadingSignal(),				avionics_.SetHeadingSlot());
			// ...which drives the course and heading needle and wheels.
}

SR71Vessel::~SR71Vessel()
{
    UnregisterMFDMode(apMFDId_);
}

void SR71Vessel::SetupAerodynamics()
{
	// Aerodynamics - see notes in ShipMets file.
	CreateAirfoil3(
		LIFT_VERTICAL, 
		_V(0, 0, -0.3), 
		VLiftCoeff, 
		0, 
		VERT_WING_CHORD, 
		VERT_WING_AREA, 
		VERT_WING_AR);

	CreateAirfoil3(
		LIFT_HORIZONTAL, 
		_V(0, 0, -4), 
		HLiftCoeff, 
		0, 
		HORZ_WING_CHORD,
		HORZ_WING_AREA,
		HORZ_WING_AR);
}

int SR71Vessel::RegisterAPMFD()
{
    static char* name = "SR71AP";
    MFDMODESPECEX spec;
    spec.name = name;
    spec.key = OAPI_KEY_B;
    spec.context = NULL;
    spec.msgproc = APMFD::MsgProc;
    return RegisterMFDMode(spec);
}
