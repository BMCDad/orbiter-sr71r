//	SurfaceController - SR-71r Orbiter Addon
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

#include "Orbitersdk.h"

#include "APU.h"

// REFACTOR NOTE:
// The settings (InboardElevonArea, etc.) should be moved so that this class 
// can be more general in nature and moved into bco.

class VESSEL3;

const float AILERON_RANGE = (float)(20.0 * RAD);
/*	Control settings.
	These settings control the responsiveness of the flight control surfaces.  The area
	settings are from the SR-71 manual and should probably be kept as is.  The dCl... settings
	control the effectiveness of each control.  These can be tweaked to adjust the handling of the
	vessel.  To see how these are applied look at the 'EnableControls' method of this class.
*/
const double InboardElevonArea =	3.6;		// From manual, inboard (39sf/3.6ms).
const double dClInboard =			0.2;		// Adjust to impact mostly elevator function.
const double OutboardElevonArea =	4.8;		// From manual, outboard (52.5sf/4.8ms).
const double dClOutboard =			0.2;		// Adjust to impact both elevator and aileron function.
const double RudderArea =			6.4;		// From manual, movable rudder area (70sf)
const double dClRudder =			0.2;		// Adjust to implact rudder function.

namespace bco = bc_orbiter;

/**	SurfaceController
	Manages the control surface for the airplane.
	Checks for hydraulic pressure and enables control if present.  If not then all control
	surfaces are disabled.
*/
class SurfaceController : 
	  public bco::vessel_component
	, public bco::set_class_caps
	, public bco::post_step
{
public:
	SurfaceController(bco::vessel& vessel, bco::hydraulic_provider& apu);

	// set_class_caps
	void handle_set_class_caps(bco::vessel& vessel) override;

	// post_step
	void handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd) override;

	void SetAileronLevel(double level);
	void SetRudderLevel(double level);
	void SetElevatorLevel(double level);

private:
	bco::vessel&	vessel_;
	void EnableControls();
	void DisableControls();

	double		prevHydraulicState_			{ 0.0 };

	int			anim_left_aileron_			{ 0 };
	int			anim_left_elevator_			{ 0 };
	int			anim_right_aileron_			{ 0 };
	int			anim_right_elevator_		{ 0 };
	int			anim_left_rudder_			{ 0 };
	int			anim_right_rudder_			{ 0 };

	CTRLSURFHANDLE	ctrlSurfLeftAileron_	{ nullptr };
	CTRLSURFHANDLE	ctrlSurfRightAileron_	{ nullptr };
	CTRLSURFHANDLE	ctrlSurfLeftElevator_	{ nullptr };
	CTRLSURFHANDLE	ctrlSurfRightElevator_	{ nullptr };
	CTRLSURFHANDLE	ctrlSurfLeftRudder_		{ nullptr };
	CTRLSURFHANDLE	ctrlSurfRightRudder_	{ nullptr };
	CTRLSURFHANDLE	ctrlSurfLeftTrim_		{ nullptr };
	CTRLSURFHANDLE	ctrlSurfRightTrim_		{ nullptr };

	bco::hydraulic_provider& apu_;
};
