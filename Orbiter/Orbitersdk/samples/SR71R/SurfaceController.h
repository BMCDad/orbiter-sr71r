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

#include "bc_orbiter\Component.h"
#include "bc_orbiter\bco.h"

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
const double InboardElevonArea = 3.6;		// From manual, inboard (39sf/3.6ms).
const double dClInboard = 0.2;		// Adjust to impact mostly elevator function.
const double OutboardElevonArea = 4.8;		// From manual, outboard (52.5sf/4.8ms).
const double dClOutboard = 0.2;		// Adjust to impact both elevator and aileron function.
const double RudderArea = 6.4;		// From manual, movable rudder area (70sf)
const double dClRudder = 0.2;		// Adjust to implact rudder function.

namespace bco = bc_orbiter;

/**	SurfaceController
	Manages the control surface for the airplane.
	Checks for hydraulic pressure and enables control if present.  If not then all control
	surfaces are disabled.
*/
class SurfaceController : public bco::Component
{
public:
	SurfaceController(bco::BaseVessel* vessel);

	virtual void SetClassCaps() override;

	// Temporal
	void Step(double simt, double simdt, double mjd);

	void SetAileronLevel(double level);
	void SetRudderLevel(double level);
	void SetElevatorLevel(double level);

	void SetAPU(APU* ap) { apu_ = ap; }

private:
	void EnableControls();
	void DisableControls();

	double			prevHydraulicState_;

	int			anim_left_aileron_;
	int			anim_left_elevator_;
	int			anim_right_aileron_;
	int			anim_right_elevator_;
	int			anim_left_rudder_;
	int			anim_right_rudder_;

	CTRLSURFHANDLE	ctrlSurfLeftAileron_;
	CTRLSURFHANDLE	ctrlSurfRightAileron_;
	CTRLSURFHANDLE	ctrlSurfLeftElevator_;
	CTRLSURFHANDLE	ctrlSurfRightElevator_;
	CTRLSURFHANDLE	ctrlSurfLeftRudder_;
	CTRLSURFHANDLE	ctrlSurfRightRudder_;
	CTRLSURFHANDLE	ctrlSurfLeftTrim_;
	CTRLSURFHANDLE	ctrlSurfRightTrim_;

	APU* apu_;
};
