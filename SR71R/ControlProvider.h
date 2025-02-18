//	SurfaceController - SR-71r Orbiter Addon
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

/*  Control Provider
    Virtual base for classes that will provide input control to
    the Vessel controller.  These classes will be the auto pilot
    and flight computer.
*/

#include "Orbitersdk.h"
#include "Component.h"
#include "bco.h"


namespace bco = bc_orbiter;


// Program id flags.
//const int PidHoldHeading    = 1 << 0;
//const int PidHoldAltitude   = 1 << 1;
//const int PidHoldKEAS       = 1 << 2;
//const int PidHoldMACH       = 1 << 3;
//const int PidHoldAttitude   = 1 << 4;

class ControlProvider
{
public:
    // Informs Vessel Control what the target heading should be.
    virtual double      TargetAtmoHeading()     const { return atmoHeadingTarget_; }
    virtual double      TargetAtmoAltitude()    const { return atmoAltitudeTarget_; }
    virtual double      TargetAtmoKEAS()        const { return atmoKEASTarget_; }
    virtual double      TargetAtmoMACH()        const { return atmoMACHTarget_; }
    virtual double      TargetAtmoAOA()         const { return atmoAOATarget_; }
    virtual double      TargetAtmoBank()        const { return atmoBankTarget_; }

    const FCProgFlags RunningPrograms() const { return runningPrograms_; }

protected:
    double              atmoHeadingTarget_{ 0.0 };
    double              atmoAltitudeTarget_{ 0.0 };
    double              atmoKEASTarget_{ 0.0 };
    double              atmoMACHTarget_{ 0.0 };
    double              atmoAOATarget_{ 0.0 };
    double              atmoBankTarget_{ 0.0 };

    void SetProgramState(FCProgFlags pid, bool state)
    {
        runningPrograms_ = (state) ?
            runningPrograms_ | pid : 
            runningPrograms_ & ~pid;
    }

    void ClearAllPrograms() { runningPrograms_ = FCProgFlags::None; }

    bool IsControlling() const { return isControlling_; }
private:
    FCProgFlags         runningPrograms_    { FCProgFlags::None };
    bool        isControlling_;
};
