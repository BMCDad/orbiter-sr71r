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
#include "bc_orbiter\pid_altitude.h"

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
	Base class for any program that will control the vessel.  All
	program input and output (control) will come through the IVesselControl
	interface.

	control programs contain the logic of what the program does. For example,
	the HoldHeading program implements the logic for steering the vessel
	towards a target heading.
*/
struct control_program
{
    virtual void set_target(double t) = 0;
    virtual void start(bco::Vessel& Vessel) = 0;
    virtual void stop(bco::Vessel& Vessel) = 0;
    virtual void step(bco::Vessel& Vessel, double simt, double simdt, double mjd) = 0;
};

class HoldHeadingProgram : public control_program
{
    const double RAD30 = 30 * RAD;
	double target_{ 0.0 };

public:
    HoldHeadingProgram() 
    {}

	void set_target(double target) override { target_ = target; }

    void step(bco::Vessel& Vessel, double simt, double simdt, double mjd) override
    {
		// GetHeading will return a value between 0 and PI2 (0 - 360 in degrees)
        auto currentYaw     = Vessel.AvGetHeading();

		// Bank will return between -PI2 to PI2
        auto currentBank    = Vessel.AvGetBank();

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

        Vessel.SetAileronLevel(-deltaBank);
//        sprintf(oapiDebugString(), "Hold heading ON - Target: %.2f, deltaBank: %.2f", target * DEG, deltaBank);
    }

    void start(bco::Vessel& Vessel) override {}

    void stop(bco::Vessel& Vessel) override
    {
        Vessel.SetAileronLevel(0.0);
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
        elevator control.

    */
    // Auto pilot consts:
    const double TargetClimbRateMPS = 20.0;
    const double AltitudeHoldRange = 100.0;
    const double ClimbHoldRange = 2.0;

	double target_{ 0.0 };

    bco::pid_altitude pid_;
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

    void step(bco::Vessel& Vessel, double simt, double simdt, double mjd) override
    {
        auto currentAltitude = Vessel.AvGetAltitude();
        auto altError = target_ - currentAltitude;

        auto vertSpeed = Vessel.AvGetVerticalSpeed();

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

        Vessel.SetAileronLevel(ctrlLevel / 2);
    }

    void stop(bco::Vessel& Vessel) override
    {
        Vessel.SetElevatorLevel(0.0);
    }

	void start(bco::Vessel& Vessel) override
	{
        Vessel.SetElevatorLevel(0.0);
        target_ = Vessel.AvGetAltitude();
        pid_.reset(target_, kp, ki, kd);
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

    void step(bco::Vessel& Vessel, double simt, double simdt, double mjd) override
    {
        auto currentKeas    = Vessel.AvGetKEAS();
        auto thLevel        = Vessel.GetMainThrustLevel();

        auto accel          = (currentKeas - prevAtmoKeas_) / simdt;
        prevAtmoKeas_       = currentKeas;

        auto dspd           = target_ - currentKeas;
        auto dThrot         = (dspd - accel) * simdt;

        Vessel.SetMainThrustLevel(thLevel + dThrot);
    }

    void stop(bco::Vessel& Vessel) override
    {
        prevAtmoKeas_ = -1.0;
    }

	void start(bco::Vessel& Vessel) override
	{
        target_ = Vessel.AvGetKEAS();
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

    void step(bco::Vessel& Vessel, double simt, double simdt, double mjd) override
    {
        double mach         = Vessel.AvGetMach();
        
        auto thLevel        = Vessel.GetMainThrustLevel();

        auto currentMach    = mach;
        auto accel          = (currentMach - prevAtmoMach_) / simdt;
        prevAtmoMach_       = currentMach;

		auto dspd			= target_ -currentMach;
        auto dThrot         = (dspd - accel) * simdt;

        Vessel.SetMainThrustLevel(thLevel + dThrot);
    }

    void stop(bco::Vessel& Vessel) override
    {
        prevAtmoMach_ = -1.0;
    }

	void start(bco::Vessel& Vessel) override
	{
        double mach = Vessel.AvGetMach();
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

    void step(bco::Vessel& Vessel, double simt, double simdt, double mjd) override
    {
        // Pitch range is between 90 (up) and -90 (down).  A positive pitch rate
        // pitches up, and a negative rate down unless we are upside down (abs(bank) > 90),
        // then it reverses.

        // Start by finding the error (eP) by subtracting the actual from the 
        // target pitch.
        auto pitch = Vessel.AvGetPitch();
        auto eP = targetAOA_ - pitch;

        // Example: target = -10, actual = -20:  -10 - -20 = eP of 10.

        // Assuming 'up' a postive eP needs a positive pitch rate and vic-versa.
        // Note this is opposite the 'roll' logic.

        // We should never get a value outside of abs(180) so we don't need to 
        // check for rollover errors.
        auto bank = Vessel.AvGetBank();

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
        Vessel.AvGetAngularVelocity(v);

        // Find the rate error.
        auto eRate = tRate - v.x;

        auto Level = (eRate / MAX_ROT_RATE);

        Vessel.SetAttitudeRotation(bco::Axis::Pitch, Level);
    }

    void start(bco::Vessel& Vessel) override {}

    void stop(bco::Vessel& Vessel) override
    {
        Vessel.SetAttitudeRotation(bco::Axis::Pitch, 0.0);
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
//class VesselControl :
//      public bco::vessel_component
//    , public bco::post_step
//    , public bco::power_consumer
//{
//
//public:
//    VesselControl(
//          bco::vessel& vessel
//        , bco::power_provider& pwr
//        , bco::avionics_provider& av
//        , bco::propulsion_control& pc
//        , bco::surface_control& sc);
//
//    void handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd) override;
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
//    bco::power_provider&        power_;
//    bco::avionics_provider&     avionics_;
//    bco::propulsion_control&    propulsion_;
//    bco::surface_control&       surface_;
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