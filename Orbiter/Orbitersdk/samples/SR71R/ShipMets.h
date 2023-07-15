//	ShipMets - SR-71r Orbiter Addon
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
#include "orbitersdk.h"

// kg  -> lbs : (kb  * 2.20462) = lbs
// lbs -> kg  : (lbs * 0.45359) = kg
// ft  -> m   : (ft  * 0.3048)  = m

const double SHIP_SIZE          =     20        ;
const double EMPTY_MASS         =  26762        ; // kg, 59,000 lbs -> 26762
const double MAX_FUEL           =  36287        ; // kg, 80,000 lbs
const double MAX_RCS_FUEL       =   1000        ; // 2204.6 lbs
const double THRUST_ISP         =  40000        ; // Same as DG.
const double ENGINE_THRUST      = 486000        ; // 34000 lbs, per engine, or in [N] 151240 * 2.
const double ENGINE_THRUST_AB   = 302464        ; // Thrust with limiter active.
const double RETRO_THRUST       = 121500        ; // 1/4 ENGINE * 2 engines
const double HOVER_THRUST       = 100000        ; // * 3
const double RCS_THRUST         =   5000        ; // (DG is 2500)
const double ELEV_AREA          =      0.2      ;

const double FUEL_FILL_RATE		=	  40.0		; // kg per sec.
const double FUEL_TRANFER_RATE	=     20.0		;		// Transfers between tanks will be slow.
const double FUEL_DUMP_RATE		=     80.0		;
const double FUEL_MINIMUM_DUMP	=   8400.0 * 0.4536;	// LBS. -> kg.


const VECTOR3 PMI               = _V(52.72,  59.60,  7.36);		// From Shipedit
const VECTOR3 CROSSSECTIONS     = _V(51.78, 233.47, 10.20);     // From Shipedit
const VECTOR3 ROTDRAG           = _V( 0.20,   0.26,  0.04);		// From DG (for now).

const float GEARDOOR_RANGE      = (float)(-90 * RAD);

// Aerodynamics:
											        // From NASA:
											        // Wing span 56.60ft -> 17.25m
												    // Mean chord 37.70ft -> 11.49m
const double VERT_WING_AREA         = 198.2     ;   // chord * span = 198.2
const double VERT_WING_AR           =   1.50    ;   // AR 17.25^2 / 198.2  (wingspan^2 / area)
const double VERT_WING_CHORD        =  11.49    ;   // area / span
const double VERT_WIND_EFFICIENCY   =   0.7     ;   // A guess.

const double HORZ_WING_AREA         =  12.0     ;   // Rudders * 2
const double HORZ_WING_AR           =   1.2     ;   // 3.8^2 (height of both rudders) / 12 (area) - treat two rudders as one big wing.
const double HORZ_WING_CHORD        =   3.15    ;   // area / span
const double HORZ_WING_EFFICIENCY   =   0.6     ;   // A guess.

// Consumables:

const double O2_SUPPLY              =  20.0     ;   // Liters - Liquid oxygen
const double O2_NER                 =   0.0021  ;   // Evaporation per hour
const double OXYGEN_FILL_RATE       =   0.07    ;   // About 4 pounds per minute

const double HYDRO_SUPPLY           =  10.0     ;   // Liters - Hydrogen
const double HYDRO_NER              =   0.001   ;   // Evaporation per hour
const double HYDROGEN_FILL_RATE     =   0.07    ;   // About 4 pounds per minute.


// Power usage
const double AVIONICS_AMPS          =   5.0     ;
const double HUD_AMPS               =   3.0     ;
const double RCS_AMPS               =   2.0     ;
const double NAV_AMPS               =   2.0     ;
const double MFD_AMPS               =   5.0     ;
const double AUTOP_AMPS             =   5.0     ;
const double CARGO_AMPS             =  20.0     ;   // Amps draw will cargo doors in operation
const double CANOPY_AMPS            =  10.0     ;   // Draw will canopy in motion
const double APU_AMPS               =   2.0     ;
const double FUELCELL_AMPS          =   3.0     ;
const double STATUS_AMPS            =   1.0     ;
const double PROPULS_AMPS           =   2.0     ;
const double LIGHTS_AMPS            =   5.0     ;   // Amps per light
const double COMPUTER_AMPS          =   8.0     ;
const double HOVER_AMPS             =  10.0     ;   // Draw when hover doors open.
const double RETRO_AMPS             =  18.0     ;   // Retro doors

const float AILERON_RANGE = (float)(20.0 * RAD);
/*	control settings.
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


/*	Some stats from the SR-71 manual
	Wing area 1605 sq feet.
	Wing span 55 ft.
	Dry weight 56,600 to 60,000 lbs.
	Fuel JP-7 (fuel and coolant)
	Engines - Pratt & Whitney JT11D-20

*/