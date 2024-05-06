//	Avionics - SR-71r Orbiter Addon
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

// REFACTOR : Move into bco.

class IAvionics
{
public:
    virtual double GetPitch() const = 0;								// Angle(rad) in reference to local horizon.
    virtual double GetBank() const = 0;									// Angle(rad) in reference to local horizon.
    virtual double GetAltitudeFeet(AltitudeMode mode) const = 0;		// In reference to current surface.
    virtual double GetRawAltitude() const = 0;							// Altitude(m) reference to current surface mean radius.
    virtual double GetVertSpeedFPM() const = 0;							// Vert. speed(f) reference to local horizon.
    virtual double GetVertSpeedRaw() const = 0;							// Vert. speed(m) reference to local horizon.
    virtual double GetAirSpeedKias() const = 0;							// Airspeed KIAS
    virtual double GetAtmPressure() const = 0;							// Static air pressure at vessel.
    virtual double GetDynamicPressure() const = 0;						// Dynamic air pressure at vessel.
    virtual double GetAngleOfAttack() const = 0;						// Angle of attack(RAD)
    virtual double GetAirSpeedKeas() const = 0;							// Airspeed KEAS
    virtual double GetHeading() const = 0;								// Yaw(rad) in reference to the local horizon.
    virtual double GetGForces() const = 0;								// current G forces on vessel.
    virtual double GetSetHeading()const = 0;							// current setting of the 'heading' bug.
    virtual double GetTrimLevel() const = 0;							// current setting of trim level.
    virtual void GetMachNumbers(double& mach, double& maxMach) = 0;		// current MACH number.
    virtual void GetAngularVel(VECTOR3& v) = 0;							// Rotation around axis (rad/s)
	virtual double SetHeading(double hdg) = 0;							// Sets the 'set' heading. Returns actual.
};