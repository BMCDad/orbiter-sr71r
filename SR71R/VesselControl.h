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

/*  Vessel Control
    The intent here is to create a Control layer between the Vessel
    calls which Control spacecraft Control and the automation components
    such as the and flight computer.  Note:  The AutoPilot has been sub-sumed
	into the flight computer at this point.
*/

#include "Orbitersdk.h"
#include "..\bc_orbiter\PIDAltitude.h"

#include "IAvionics.h"
#include "PropulsionController.h"
#include "SurfaceController.h"


namespace bco = bc_orbiter;
//
///// AP classes, can be moved
//
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
	Base class for any program that will Control the Vessel.  All
	program input and output (Control) will come through the IVesselControl
	interface.

	Control programs contain the logic of what the program does. For example,
	the HoldHeading program implements the logic for steering the Vessel
	towards a target heading.
*/
struct control_program
{
    virtual void set_target(double t) = 0;
    virtual void start(bco::Vessel& vessel) = 0;
    virtual void stop(bco::Vessel& vessel) = 0;
    virtual void step(bco::Vessel& vessel, double simt, double simdt, double mjd) = 0;
};

class HoldHeadingProgram : public control_program
{
    const double RAD30 = 30 * RAD;
	double target_{ 0.0 };

public:
    HoldHeadingProgram() 
    {}

	void set_target(double target) override { target_ = target; }

    void step(bco::Vessel& vessel, double simt, double simdt, double mjd) override
    {
		// GetHeading will return a value between 0 and PI2 (0 - 360 in degrees)
        auto currentYaw     = vessel.GetHeading();

		// Bank will return between -PI2 to PI2
        auto currentBank    = vessel.GetBank();

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

        vessel.set_aileron_level(-deltaBank);
//        sprintf(oapiDebugString(), "Hold heading ON - Target: %.2f, deltaBank: %.2f", target * DEG, deltaBank);
    }

    void start(bco::Vessel& vessel) override {}

    void stop(bco::Vessel& vessel) override
    {
        vessel.set_aileron_level(0.0);
    }
};

class HoldAltitudeProgram : public control_program
{
    /*
        PID - Proportion  -  Integration  -  Derivative
        Each of these parts is tuned using a gain function, which is generally a value between
        0 and 1.  Adjust these to adjust the function.

        Proportion:
        This is simply the Kp (Proportion gain) times the error.  In the case of altitude the error
        is the delta between the current altitude and the target.  For example:

        Target = 20000
        Current = 19500
        Error = -500  -  Current - Target.  We are 500 below our target.

        If your Kp value was .5, the result would be -250.

        In a simple controller a negative value from the function would call for an increase in
        elevator Control.

    */
    // Auto pilot consts:
    const double TargetClimbRateMPS = 20.0;
    const double AltitudeHoldRange = 100.0;
    const double ClimbHoldRange = 2.0;

	double target_{ 0.0 };

    bco::PIDAltitude pid_;
    double kp = 0.1;  // .5 start .2 no change .8 no change
    double ki = 0.4;    // .2 start .1 nothing
    double kd = 0.1;

    double ALT_ERROR_RANGE  = 200.0;      // 20 meters.
    double MAX_VS           = 100.0;
    double VS_STEP_PS       =   0.01;       // Trim steps per second.

    double prev_error       = 0.0;

public:
    HoldAltitudeProgram() 
    {}

	void set_target(double t) override { target_ = t; }

    void step(bco::Vessel& vessel, double simt, double simdt, double mjd) override
    {
        auto currentAltitude = vessel.GetAvAltitude();
        auto altError = target_ - currentAltitude;

        auto vertSpeed = vessel.GetVerticalSpeed();

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

        vessel.set_aileron_level(ctrlLevel / 2);
    }

    void stop(bco::Vessel& vessel) override
    {
        vessel.set_elevator_level(0.0);
    }

	void start(bco::Vessel& vessel) override
	{
        vessel.set_elevator_level(0.0);
        target_ = vessel.GetAvAltitude();
        pid_.Reset(target_, kp, ki, kd);
	}
};

class HoldKeasProgram : public control_program
{
    double prevAtmoKeas_{ 0.0 };
    double target_{ 0.0 };

public:
    HoldKeasProgram() 
    {}

	void set_target(double t) override { target_ = t; }

    void step(bco::Vessel& vessel, double simt, double simdt, double mjd) override
    {
        auto currentKeas    = vessel.GetKEAS();
        auto thLevel        = vessel.GetMainThrustLevel();

        auto accel          = (currentKeas - prevAtmoKeas_) / simdt;
        prevAtmoKeas_       = currentKeas;

        auto dspd           = target_ - currentKeas;
        auto dThrot         = (dspd - accel) * simdt;

        vessel.SetMainThrustLevel(thLevel + dThrot);
    }

    void stop(bco::Vessel& vessel) override
    {
        prevAtmoKeas_ = -1.0;
    }

	void start(bco::Vessel& vessel) override
	{
        target_ = vessel.GetKEAS();
	}
};

class HoldMachProgram : public control_program
{
    double prevAtmoMach_{ 0.0 };
	double target_{ 0.0 };

public:
    HoldMachProgram() 
    {}

	void set_target(double t) override { target_ = t; }

    void step(bco::Vessel& vessel, double simt, double simdt, double mjd) override
    {
        double mach         = vessel.GetMACH();
        
        auto thLevel        = vessel.GetMainThrustLevel();

        auto currentMach    = mach;
        auto accel          = (currentMach - prevAtmoMach_) / simdt;
        prevAtmoMach_       = currentMach;

		auto dspd			= target_ -currentMach;
        auto dThrot         = (dspd - accel) * simdt;

        vessel.SetMainThrustLevel(thLevel + dThrot);
    }

    void stop(bco::Vessel& vessel) override
    {
        prevAtmoMach_ = -1.0;
    }

	void start(bco::Vessel& vessel) override
	{
        double mach = vessel.GetMACH();
		target_ = mach;
	}
};

class HoldAttitude : public control_program
{
    const double DEAD_ZONE      = 0.1   * RAD;
    const double DEAD_ZONE_MAX  = 5     * RAD;
    const double MAX_ROT_RATE   = 5     * RAD;		// Degrees per second.

	double targetAOA_{ 0.0 };
public:
    HoldAttitude() 
    {}

	void set_target(double t) override { targetAOA_ = t; }

    void step(bco::Vessel& vessel, double simt, double simdt, double mjd) override
    {
        // Pitch range is between 90 (up) and -90 (down).  A positive pitch rate
        // pitches up, and a negative rate down unless we are upside down (abs(bank) > 90),
        // then it reverses.

        // Start by finding the error (eP) by subtracting the actual from the 
        // target pitch.
        auto pitch = vessel.GetPitch();
        auto eP = targetAOA_ - pitch;

        // Example: target = -10, actual = -20:  -10 - -20 = eP of 10.

        // Assuming 'up' a postive eP needs a positive pitch rate and vic-versa.
        // Note this is opposite the 'roll' logic.

        // We should never get a value outside of abs(180) so we don't need to 
        // check for rollover errors.
        auto bank = vessel.GetBank();

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
        vessel.GetAngularVelocity(v);

        // Find the rate error.
        auto eRate = tRate - v.x;

        auto level = (eRate / MAX_ROT_RATE);

        vessel.SetAttitudeRotation(bco::Axis::Pitch, level);
    }

    void start(bco::Vessel& vessel) override {}

    void stop(bco::Vessel& vessel) override
    {
        vessel.SetAttitudeRotation(bco::Axis::Pitch, 0.0);
    }
};

/**
    VesselControl provides a Control interface between the low Level
    Vessel Control calls and the various classes that automate Vessel
    Control such as the auto pilot and flight computer.  The automation
    class must implement ControlProvider which provides the input for
    the Vessel automation.

    VesselControl itself is a 'ControlProvider' with default settings.
    Setting the active controller to 'self' effectively disables all
    vassel automation.

    The algorithms for Vessel Control also, for the most part, live
    here in VesselControl.  That way the higher-Level implmentation
    of autopilot and flight computer can utilize the same methods.

    Events:
    EnableHeading
    DisableHeading
    UpdateHeading
*/
//class VesselControl :
//      public bco::VesselComponent
//    , public bco::PostStep
//    , public bco::PowerConsumer
//{
//
//public:
//    VesselControl(
//          bco::Vessel& Vessel
//        , bco::PowerProvider& pwr
//        , bco::AvionicsProvider& av
//        , bco::PropulsionControl& pc
//        , bco::SurfaceControl& sc);
//
//    void HandlePostStep(bco::Vessel& Vessel, double simt, double simdt, double mjd) override;
//
//	const FCProgFlags RunningPrograms() const { return runningPrograms_; }
//
//	constexpr bool IsRunning(FCProgFlags pid) const { return (runningPrograms_ & pid) == pid; }
//
//	void ToggleAtmoProgram(FCProgFlags pid);
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
//    bco::PowerProvider&        power_;
//    bco::AvionicsProvider&     avionics_;
//    bco::PropulsionControl&    propulsion_;
//    bco::SurfaceControl&       surface_;
//
//	FCProgFlags         runningPrograms_{ FCProgFlags::None };
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