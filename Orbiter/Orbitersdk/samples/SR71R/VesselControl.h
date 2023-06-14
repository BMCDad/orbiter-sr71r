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

/*  Vessel control
    The intent here is to create a control layer between the vessel
    calls which control spacecraft control and the automation components
    such as the and flight computer.  Note:  The AutoPilot has been sub-sumed
	into the flight computer at this point.
*/

#include "Orbitersdk.h"

#include "bc_orbiter\Component.h"
#include "bc_orbiter\bco.h"

#include "IAvionics.h"
#include "PropulsionController.h"
#include "SurfaceController.h"


namespace bco = bc_orbiter;

/// AP classes, can be moved

enum class FCProgFlags
{
	None			= 0,
	HoldHeading		= 1,
	HoldAltitude	= 2,
	HoldKEAS		= 4,
	HoldMACH		= 8,
	HoldInvalid		= 12,			// << KEAS + MACH is invalid.  Use this to force a re-eval.
	HoldAttitude	= 16,
	AtmoActive		= 32
};

constexpr FCProgFlags operator|(FCProgFlags l, FCProgFlags r)
{
	return static_cast<FCProgFlags>(static_cast<int>(l) | static_cast<int>(r));
}

constexpr FCProgFlags operator&(FCProgFlags l, FCProgFlags r)
{
	return static_cast<FCProgFlags>(static_cast<int>(l) & static_cast<int>(r));
}

constexpr FCProgFlags operator~(const FCProgFlags l)
{
	return static_cast<FCProgFlags>(~static_cast<int>(l));
}


struct IVesselControl
{
    virtual IAvionics*              GetAvionics() const = 0;
    virtual PropulsionController*   GetPropulsionController() const = 0;
    virtual SurfaceController*      GetSurfaceController() const = 0;
};

/**
	Base class for any program that will control the vessel.  All
	program input and output (control) will come through the IVesselControl
	interface.

	control programs contain the logic of what the program does. For example,
	the HoldHeading program implements the logic for steering the vessel
	towards a target heading.
*/
class ControlProgram
{
public:
    ControlProgram(IVesselControl& p) : vesselControl_(p)
    {}

    virtual void Start() {}
    virtual void Stop() {}
    virtual void Step(double simt, double simdt, double mjd) {}

protected:
    IVesselControl& vesselControl_;
};

class HoldHeadingProgram : public ControlProgram
{
    const double RAD30 = 30 * RAD;
	double target_{ 0.0 };

public:
    HoldHeadingProgram(IVesselControl& v) : ControlProgram(v)
    {}

	void SetTargetHeading(double target) { target_ = target; }

    void Step(double simt, double simdt, double mjd) override
    {
		// GetHeading will return a value between 0 and PI2 (0 - 360 in degrees)
        auto currentYaw     = vesselControl_.GetAvionics()->GetHeading();

		// Bank will return between -PI2 to PI2
        auto currentBank    = vesselControl_.GetAvionics()->GetBank();

		// This is the target heading in RAD, it should also be between 0 and PI2
		target_				= vesselControl_.GetAvionics()->GetSetHeading();

        // Find the delta heading from target.  This is also called the 'error'.
		// When the target is to the left of current, the error is negative.  The
		// error value should be between -PI and PI, or 180 deg.
        auto deltaYaw = target_ - currentYaw;

		// If deltaYas is < -PI or > PI then we are 'going around the long way'
		// and need to adjust.
        if (deltaYaw < -PI) deltaYaw += PI2;
        if (deltaYaw > PI) deltaYaw -= PI2;

        // deltaYaw is -PI to PI.

        // -deltaYaw becomes targetBank
        // Target bank based on yawFactor * 1 RAD
        auto targetBank = 0.0;
        if (abs(deltaYaw) < 0.2)
        {
            targetBank = -deltaYaw;
        }
        else
        {
            targetBank = (deltaYaw > 0) ? -RAD30 : RAD30;
        }

        // Find bank delta from target.
        auto deltaBank = targetBank - currentBank;

        // Clamp
        if (deltaBank > 1) deltaBank = 1;
        if (deltaBank < -1) deltaBank = -1;

        vesselControl_.GetSurfaceController()->SetAileronLevel(-deltaBank);

//        sprintf(oapiDebugString(), "Hold heading ON - Target: %.2f, deltaBank: %.2f", target * DEG, deltaBank);
    }

    void Stop() override
    {
        vesselControl_.GetSurfaceController()->SetAileronLevel(0.0);
    }
};

class HoldAltitudeProgram : public ControlProgram
{
    // Auto pilot consts:
    const double TargetClimbRateMPS = 20.0;
    const double AltitudeHoldRange = 100.0;
    const double ClimbHoldRange = 2.0;

	double target_{ 0.0 };

public:
    HoldAltitudeProgram(IVesselControl& v) : ControlProgram(v)
    {}

	void SetTargetAltitude(double t) { target_ = t; }

    void Step(double simt, double simdt, double mjd) override
    {
        auto currentAltitude    = vesselControl_.GetAvionics()->GetRawAltitude();
        auto altError           = target_ - currentAltitude;

        auto vertSpeed          = vesselControl_.GetAvionics()->GetVertSpeedRaw();

        auto targetClimb = 0.0;

        if (fabs(altError) < AltitudeHoldRange)	// within AltRange
        {
            targetClimb = TargetClimbRateMPS * (altError / AltitudeHoldRange);
        }
        else
        {
            targetClimb = (altError > 0) ? TargetClimbRateMPS : -TargetClimbRateMPS;
        }

        auto climbError = targetClimb - vertSpeed;

        auto ctrlLevel = 0.0;

        if (fabs(climbError) < TargetClimbRateMPS)
        {
            ctrlLevel = climbError / TargetClimbRateMPS;
        }
        else
        {
            ctrlLevel = (climbError > 0) ? 1.0 : -1.0;
        }

        vesselControl_.GetSurfaceController()->SetElevatorLevel(ctrlLevel / 2);
    }

    void Stop() override
    {
        vesselControl_.GetSurfaceController()->SetElevatorLevel(0.0);
    }

	void Start() override
	{
		target_ = vesselControl_.GetAvionics()->GetRawAltitude();
	}
};

class HoldKeasProgram : public ControlProgram
{
    double prevAtmoKeas_{ 0.0 };
    double target_{ 0.0 };

public:
    HoldKeasProgram(IVesselControl& v) : ControlProgram(v)
    {}

	void SetTargetKEAS(double t) { target_ = t; }

    void Step(double simt, double simdt, double mjd) override
    {
        auto currentKeas    = vesselControl_.GetAvionics()->GetAirSpeedKeas();
        auto thLevel        = vesselControl_.GetPropulsionController()->GetVesselMainThrustLevel();

        auto accel          = (currentKeas - prevAtmoKeas_) / simdt;
        prevAtmoKeas_       = currentKeas;

        auto dspd           = target_ - currentKeas;
        auto dThrot         = (dspd - accel) * simdt;

        vesselControl_.GetPropulsionController()->SetVesselMainThrustLevel(thLevel + dThrot);
    }

    void Stop() override
    {
        prevAtmoKeas_ = -1.0;
    }

	void Start() override
	{
		target_ = vesselControl_.GetAvionics()->GetAirSpeedKeas();
	}
};

class HoldMachProgram : public ControlProgram
{
    double prevAtmoMach_{ 0.0 };
	double target_{ 0.0 };

public:
    HoldMachProgram(IVesselControl& v) : ControlProgram(v)
    {}

	void SetTargetMach(double t) { target_ = t; }

    void Step(double simt, double simdt, double mjd) override
    {
        double mach, maxMach;
        vesselControl_.GetAvionics()->GetMachNumbers(mach, maxMach);
        
        auto thLevel        = vesselControl_.GetPropulsionController()->GetVesselMainThrustLevel();

        auto currentMach    = mach;
        auto accel          = (currentMach - prevAtmoMach_) / simdt;
        prevAtmoMach_       = currentMach;

		auto dspd			= target_ -currentMach;
        auto dThrot         = (dspd - accel) * simdt;

        vesselControl_.GetPropulsionController()->SetVesselMainThrustLevel(thLevel + dThrot);
    }

    void Stop() override
    {
        prevAtmoMach_ = -1.0;
    }

	void Start() override
	{
		double mach, maxMach;
		vesselControl_.GetAvionics()->GetMachNumbers(mach, maxMach);
		target_ = mach;
	}
};

class HoldAttitude : public ControlProgram
{
    const double DEAD_ZONE      = 0.1   * RAD;
    const double DEAD_ZONE_MAX  = 5     * RAD;
    const double MAX_ROT_RATE   = 5     * RAD;		// Degrees per second.

	double targetAOA_{ 0.0 };
public:
    HoldAttitude(IVesselControl& v) : ControlProgram(v)
    {}

	void SetTargetAOA(double t) { targetAOA_ = t; }

    void Step(double simt, double simdt, double mjd) override
    {
        // Pitch range is between 90 (up) and -90 (down).  A positive pitch rate
        // pitches up, and a negative rate down unless we are upside down (abs(bank) > 90),
        // then it reverses.

        // Start by finding the error (eP) by subtracting the actual from the 
        // target pitch.
        auto pitch = vesselControl_.GetAvionics()->GetPitch();
        auto eP = targetAOA_ - pitch;

        // Example: target = -10, actual = -20:  -10 - -20 = eP of 10.

        // Assuming 'up' a postive eP needs a positive pitch rate and vic-versa.
        // Note this is opposite the 'roll' logic.

        // We should never get a value outside of abs(180) so we don't need to 
        // check for rollover errors.
        auto bank = vesselControl_.GetAvionics()->GetBank();

        // Determine if we are 'up' which is bank < 90.
        auto updn = (fabs(bank) < (90 * RAD)) ? 1 : -1;

        double tRate = 0.0;

        // If we are in the dead zone, then assume 0.0 target roll.
        // We also assume we are 'up' and will adjust later if needed.
        if (fabs(eP) > DEAD_ZONE)
        {
        	if (fabs(eP) > DEAD_ZONE_MAX)
        	{
        		tRate = (eP > 0.0) ? MAX_ROT_RATE : -MAX_ROT_RATE;
        	}
        	else
        	{
        		tRate = MAX_ROT_RATE * (eP / DEAD_ZONE_MAX);
        	}
        }

        // Adjust the rate for 'down'.
        tRate = tRate * updn;

        VECTOR3 v;
        vesselControl_.GetAvionics()->GetAngularVel(v);

        // Find the rate error.
        auto eRate = tRate - v.x;

        auto level = (eRate / MAX_ROT_RATE);

        vesselControl_.GetPropulsionController()->SetAttitudeRotLevel(PropulsionController::Axis::Pitch, level);
    }

    void Stop() override
    {
        vesselControl_.GetPropulsionController()->SetAttitudeRotLevel(PropulsionController::Axis::Pitch, 0.0);
    }
};

/**
    VesselControl provides a control interface between the low level
    vessel control calls and the various classes that automate vessel
    control such as the auto pilot and flight computer.  The automation
    class must implement ControlProvider which provides the input for
    the vessel automation.

    VesselControl itself is a 'ControlProvider' with default settings.
    Setting the active controller to 'self' effectively disables all
    vassel automation.

    The algorithms for vessel control also, for the most part, live
    here in VesselControl.  That way the higher-level implmentation
    of autopilot and flight computer can utilize the same methods.

    Events:
    EnableHeading
    DisableHeading
    UpdateHeading
*/
//class VesselControl : public IVesselControl
//{
//
//public:
//    VesselControl();
//
//    void Step(double simt, double simdt, double mjd);
//
//    // Input:
//    void SetAvionics(IAvionics* av)                     { avionics_ = av; }
//    void SetPropulsionControl(PropulsionController* p)  { propulsionControl_ = p; }
//    void SetSurfaceControl(SurfaceController* s)        { surfaceController_ = s; }
//
//
//    // IVesselControl
//    IAvionics*              GetAvionics()               const override { return avionics_; }
//    PropulsionController*   GetPropulsionController()   const override { return propulsionControl_; }
//    SurfaceController*      GetSurfaceController()      const override { return surfaceController_; }
//
//	const FCProgFlags RunningPrograms() const { return runningPrograms_; }
//
//	constexpr bool IsRunning(FCProgFlags pid) const { return (runningPrograms_ & pid) == pid; }
//
//	void ToggleAtmoProgram(FCProgFlags pid);
//
//
//	void SetProgramState(FCProgFlags pid, bool state)
//	{
//		runningPrograms_ = (state) ?
//			runningPrograms_ | pid :
//			runningPrograms_ & ~pid;
//	}
//
//protected:
//
//	void ClearAllPrograms() { runningPrograms_ = FCProgFlags::None; }
//
//	bool CanRunAtmo() const { return true; }
//private:
//    PropulsionController*		propulsionControl_;
//    IAvionics*					avionics_;
//    SurfaceController*			surfaceController_;
//
//	FCProgFlags         runningPrograms_{ FCProgFlags::None };
//
//
//
//
//    HoldAltitudeProgram         prgHoldAltitude_;
//    HoldHeadingProgram          prgHoldHeading_;
//    HoldKeasProgram             prgHoldKeas_;
//    HoldMachProgram             prgHoldMach_;
//
//	FCProgFlags prevRunningProgs { FCProgFlags::None };
//
//    void UpdateProg(FCProgFlags current, FCProgFlags pid)
//    {
//        if ((current & pid) != (prevRunningProgs & pid))
//        {
//            auto prog = mapPrograms_[pid];
//            (current & pid) != FCProgFlags::None ? prog->Start() : prog->Stop();
//        }
//    }
//
//    void UpdateProgs(FCProgFlags current)
//    {
//        UpdateProg(current, FCProgFlags::HoldAltitude);
//        UpdateProg(current, FCProgFlags::HoldHeading);
//        UpdateProg(current, FCProgFlags::HoldKEAS);
//        UpdateProg(current, FCProgFlags::HoldMACH);
//    }
//
//    std::map<FCProgFlags, ControlProgram*>     mapPrograms_
//    {
//        {FCProgFlags::HoldAltitude,    &prgHoldAltitude_},
//        {FCProgFlags::HoldHeading,     &prgHoldHeading_},
//        {FCProgFlags::HoldKEAS,        &prgHoldKeas_ },
//        {FCProgFlags::HoldMACH,        &prgHoldMach_ }
//    };
//};