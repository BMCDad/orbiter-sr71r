//	AutoPilot - SR-71r Orbiter Addon
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
#include "OnOffSwitch.h"
#include "RotarySwitch.h"
#include "Animation.h"
#include "TextureVisual.h"
#include "PoweredComponent.h"
#include "Avionics.h"

#include "SR71r_mesh.h"
#include "VCToggleSwitch.h"
#include "VCRotorSwitch.h"

// Interfaces used:
#include "PropulsionController.h"
#include "SurfaceController.h"

#include <vector>
#include <map>
#include <functional>

const double ATM_PRES_XOVER     =      5000     ;
const double MAX_AERO_BANK      = RAD *  30     ;
const double MAX_RUDDER_CONTROL =         0.5   ;
const double AP_MIN_VOLT        =        20.0   ;

namespace bco = bc_orbiter;

/**	AutoPilot
	The Vessel's auto pilot.  When enabled it has the following modes:
	
	Hold Heading:
	Turns the plane to the heading selected with the HDG knob and follows that heading.

	Hold Altitude:
	Maintains the plane at the current altitude.

	Hold Speed:
	Maintains the current speed.  The KEAS / MACH switch determines if KEAS or
	MACH is used for current speed.

	Auto pilot requires power from the main circuit to function.  The power draw is fixed
	amps when the main circuit is powered and the Auto Pilot 'Main' switch is on (up).

	To enable the auto pilot (main circuit already powered and power 'Main' enabled) turn
	the main auto pilot switch on (up) and then enable an autopilot by turning that switch on(up).

	Configuration:
	AUTOPILOT a b c d e f
	a = 0/1 main power.
	b = 0/1 hold heading.
	c = 0/1 hold altitude.
	d = 0/1 hold speed.
	e = 0/1 0 = MACH, 1 = KEAS
	f = 0/1/2 Wheel Adjust Mode: 0 = Altitude, 1 = Speed, 2 = Attitude
*/
class AutoPilot : 
	public bco::PoweredComponent
{
public:
	enum Source             { GuidanceComputer, Navigation };
	enum AdjustMode         { Altitude = 0, Speed = 1, Attitude = 2 };
    enum AdjustSize         { Big = 0, Small = 1 };
	enum AdjustDirection    { Up = 0, Down = 1 };
	enum APPrograms         { None, HoldHeading, HoldAltitude, HoldMach, HoldKeas, HoldAttitude };

	AutoPilot(bco::BaseVessel* vessel, double amps);

    // *** PoweredComponent ***
    virtual double CurrentDraw() override;
    virtual void ChangePowerLevel(double newLevel) override;

    // *** Component ***
    virtual void SetClassCaps() override;
    virtual bool LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
    virtual void SaveConfiguration(FILEHANDLE scn) const override;
	virtual bool VCRedrawEvent(int id, int event, SURFHANDLE surf) override;
	virtual bool LoadVC(int id) override;

	/**
	Runs the active autopilot programs.
	*/
	void Step(double simt, double simdt, double mjd);

    // *** AutoPilot ***

    bool DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

    // Control Provider
    // We override this to always provide the avionics heading for the autopilot.
//    double TargetAtmoHeading() const override { return avionics_->GetSetHeading(); }

    //// Override so we know when we are getting and losing control.  We don't want
    //// display the AP status if we don't have control.
    //void SetIsControlling(bool state) override
    //{
    //    ControlProvider::SetIsControlling(state);
    //    (state) ? TurningOn() : TurningOff();
    //}


	bco::PushButtonSwitch& PowerSwitch()				 { return swAPMain_; }
	bco::OnOffSwitch& HoldHeadingSwitch()		 { return swAPHeading_; }
	bco::OnOffSwitch& HoldAltitudeSwitch()		 { return swAPAltitude_; }
	bco::OnOffSwitch& HoldSpeedSwitch()			 { return swAPSpeed_; }
	bco::OnOffSwitch& SelectKeasMachSwitch()	 { return swAPKeasMach_; }
	bco::OnOffSwitch& HoldAttitudeSwitch()		 { return swAPAttitude_; }

	// Input:
	void SetAvionics(Avionics* av)                      { avionics_ = av; }
	void SetPropulsionControl(PropulsionController* p)  { propulsionControl_ = p; }
	void SetSurfaceControl(SurfaceController* s)        { surfaceController_ = s; }

private:
	const double OnOffset = 0.0;
	const double OffOffset = 0.0361;

	// Atmospheric speed:
	double prevTargetAccel_;

	void ToggleTurningOn();
    void TurningOn();
    void TurningOff();

    void OnChangedHeadingSwitch();
    void OnChangedAltitudeSwitch();
    void OnChangedSpeedSwitch();
    void OnChangedAttitudeSwitch();

	void HandleAdjust(AdjustSize size, AdjustDirection dir);

	std::string			configKey_;

	int			modeSelectorId_;
    int         inputSelectorId_;
	int			pitchUpId_;
	int			pitchDownId_;
	int			bankLeftId_;
	int			bankRightId_;

	AdjustMode		adjustMode_;
    Source          inputSource_;

	PropulsionController*		propulsionControl_;
	Avionics*					avionics_;
	SurfaceController*			surfaceController_;

	bool		isAPActive_;

	Source	currentSource_;
	int			areaId_;


	bco::TextureVisual		visAPMainOn_;
	bco::PushButtonSwitch   swAPMain_				{	bt_mesh::SR71rVC::SwAPMain_location, 0.01 };

    bco::VCToggleSwitch     swAPHeading_            {   bt_mesh::SR71rVC::SwAPHeading_id,
                                                        bt_mesh::SR71rVC::SwAPHeading_location,
                                                        bt_mesh::SR71rVC::APRowRightAxis_location
                                                    };

    bco::VCToggleSwitch     swAPAltitude_           {   bt_mesh::SR71rVC::SwAPAltitude_id,
                                                        bt_mesh::SR71rVC::SwAPAltitude_location,
                                                        bt_mesh::SR71rVC::APRowRightAxis_location
                                                    };

    bco::VCToggleSwitch     swAPSpeed_              {   bt_mesh::SR71rVC::SwAPSpeed_id,
                                                        bt_mesh::SR71rVC::SwAPSpeed_location,
                                                        bt_mesh::SR71rVC::APRowRightAxis_location
                                                    };

    bco::VCToggleSwitch     swAPKeasMach_           {   bt_mesh::SR71rVC::SwAPKeasMach_id,
                                                        bt_mesh::SR71rVC::SwAPKeasMach_location,
                                                        bt_mesh::SR71rVC::APRowRightAxis_location
                                                    };

    bco::VCToggleSwitch     swAPAttitude_           {   bt_mesh::SR71rVC::SwAPAttitude_id,
                                                        bt_mesh::SR71rVC::SwAPAttitude_location,
                                                        bt_mesh::SR71rVC::APRowRightAxis_location
                                                    };


    bco::VCRotorSwitch      swModeSelect_           {   bt_mesh::SR71rVC::APModeSwitch_id, 
                                                        bt_mesh::SR71rVC::APModeSwitch_location, 
                                                        bt_mesh::SR71rVC::APModeSelectorTopAxis_location,
                                                        (90 * RAD),
                                                    };

    bco::EventTarget        eventPitchUp_           { bt_mesh::SR71rVC::APPitchUpTarget_location,        0.01 };
    bco::EventTarget        eventPitchDown_         { bt_mesh::SR71rVC::APPitchDownTarget_location,      0.01 };
};