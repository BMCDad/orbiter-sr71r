//	PropulsionController - SR-71r Orbiter Addon
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

#include <string>

#include "bc_orbiter/Animation.h"
#include "bc_orbiter/vessel.h"
#include "bc_orbiter/on_off_input.h"
#include "bc_orbiter/on_off_display.h"
#include "bc_orbiter/simple_event.h"
#include "bc_orbiter/rotary_display.h"
#include "bc_orbiter/status_display.h"

#include "SR71r_mesh.h"
#include "SR71r_common.h"
#include "ShipMets.h"

namespace bco = bc_orbiter;

class VESSEL3;

/**
Manages propulsion and propellents for the vessel including thrust limiter and fuel
fill, transfer, and dump functions.

Thrust Limit:
The Thrust Limit switch has two modes; A/B (down) limits maximum thrust to the SR71's after burner
thrust at sea level (68000 lbs).  FULL mode boots the top thrust level to about 110000 lbs.

Fuel Management:
Fuel can be added to the Main tank when the supply available light is on.  Press the valve 'OPEN'
button to begin filling.  

Fuel can be transfered between the Main and RCS tanks by selecting the
transfer select switch which will select the target tank and then turning the tranfer pump ON.  This
is the only way to fill the RCS tank from the cockpit.

Fuel can be dumped from the MAIN tank be turning on the FUEL DUMP switch.  To empty both tanks
transfer all fuel from the RCS tank into the MAIN tank to dump.  Dumping will stop when there is
less then 8400 lbs of fuel in the MAIN tank.

Fuel dump:
2400 lbs per minute.  Stops at 8400 lbs.

Hover:
The hover nozzles are covered to protect them during atmospheric flight.  To operate the hover
engines, the hover doors must be in the full-open position.  The hover door switch is on the
left-bottom panel.  Open is down.  The hover doors are on the main electrical switch, which
must be ON for the hover doors to operate.

Retro:
The retro rockets are located in the front end of the nacel pods.  The aero-spike must be fully
retracted for the retro rockets to work.  The retro door (aero-spike retraction) switch is on
the left-bottom panel.  Open is down.  The retro doors are on the main electrical switch, which
must be ON for the retro doors to operate.

Configuration:
PROPULSION a b c d e
a = 0/1 Thrust limit AB/Full.
b = 0/1 Transfer mode: RCS/Main.
c = 0/1 Fill valve is open.
e = 0/1 Transfer pump is active.
e = 0/1 Dump pump is active.
*/

class PropulsionController : 
	  public bco::VesselComponent
	, public bco::HandlesSetClassCaps
	, public bco::PowerConsumer
	, public bco::HandlesPostStep
	, public bco::HandlesState
	, public bco::HandlesDrawHud
{
public:
	PropulsionController(bco::PowerProvider& pwr, bco::vessel& vessel);

	// post_step
	void HandlePostStep(bco::vessel& vessel, double simt, double simdt, double mjd) override;

	// power_consumer
	double AmpDraw() const override { return (isFilling_ ? 4.0 : 0.0) + (isRCSFilling_ ? 4.0 : 0.0); }

	// set_class_caps
	void HandleSetClassCaps(bco::vessel& vessel) override;

	// manage_state
	bool HandleLoadState(bco::vessel& vessel, const std::string& line) override;
	std::string HandleSaveState(bco::vessel& vessel) override;

	// draw_hud
    void HandleDrawHUD(bco::vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;

	double GetVesselMainThrustLevel();
	void SetVesselMainThrustLevel(double Level);
    void SetAttitudeRotLevel(bco::Axis axis, double Level);
	double CurrentMaxThrust() { return maxThrustLevel_; }

	bco::signal<double>&	MainFuelLevelSignal() { return signalMainFuelLevel_; }

	void ToggleThrustLimit() { switchThrustLimit_.toggle_state(); }

private:
	bco::vessel&		vessel_;
	bco::PowerProvider&	power_;

	bool IsPowered() { return power_.VoltsAvailable() > 24.0; }

	bco::signal<double>		signalMainFuelLevel_;

	double DrawMainFuel(double amount);
	double FillMainFuel(double amount);

	double DrawRCSFuel(double amount);
	double FillRCSFuel(double amount);

	void SetThrustLevel(double newLevel);
	void Update(double deltaUpdate);

	void ToggleFill();
	void ToggleRCSFill();

	void HandleTransfer(double deltaUpdate);

	THRUSTER_HANDLE		mainThrustHandles_[2];
    THRUSTER_HANDLE     retroThrustHandles_[2];

	double				maxMainFlow_;			// MAXTHRUST / ISP.

	const char*	ConfigKey = "PROPULSION";
	double		mainFuelLevel_;
	double		rcsFuelLevel_;
	double		maxThrustLevel_;
	int			areaId_;
	double		prevTime_;
	bool		isFilling_			{ false };
	bool		isExternAvail_		{ false };

	bool		isRCSFilling_		{ false };

	// Switches
	bco::on_off_input		switchThrustLimit_{		// Thrust Limit
		{ bm::vc::swThrottleLimit_id },
			bm::vc::swThrottleLimit_loc, bm::vc::TopRowSwitchRightAxis_loc,
			toggleOnOff,
			bm::pnl::pnlThrottleLimit_id,
			bm::pnl::pnlThrottleLimit_vrt,
			bm::pnl::pnlThrottleLimit_RC
	};

	bco::on_off_input		switchFuelDump_{		// Fuel dump
		{ bm::vc::swDumpFuel_id },
			bm::vc::swDumpFuel_loc, bm::vc::FuelTransferRightAxis_loc,
			toggleOnOff,
			bm::pnl::pnlFuelDump_id,
			bm::pnl::pnlFuelDump_vrt,
			bm::pnl::pnlFuelDump_RC
	};

	// Gauges
	bco::rotary_display<bco::animation_target>		gaugeFuelFlow_{
		{ bm::vc::gaFuelFlow_id },
			bm::vc::gaFuelFlow_loc, bm::vc::FuelFlowAxisFront_loc,
			bm::pnl::pnlGaFuelFlow_id,
			bm::pnl::pnlGaFuelFlow_vrt,
			(270 * RAD),	// Clockwise
			1.0
	};

	bco::rotary_display<bco::animation_target>		gaugeFuelMain_{
		{ bm::vc::gaMainFuel_id },
			bm::vc::gaMainFuel_loc, bm::vc::FuelLevelAxisFront_loc,
			bm::pnl::pnlGaFuelMain_id,
			bm::pnl::pnlGaFuelMain_vrt,
			(256 * RAD),	// Clockwise
			1.0
	};

	bco::rotary_display<bco::animation_target>		gaugeFuelRCS_{
		{ bm::vc::gaRCSFuel_id },
			bm::vc::gaRCSFuel_loc, bm::vc::RCSLevelAxisFront_loc,
			bm::pnl::pnlGaFuelRCS_id,
			bm::pnl::pnlGaFuelRCS_vrt,
			(264 * RAD),	// Clockwise
			1.0
	};

	// Displays
	bco::on_off_display		lightFuelAvail_{
		bm::vc::FuelSupplyOnLight_id,
			bm::vc::FuelSupplyOnLight_vrt,
			bm::pnl::pnlFuelAvail_id,
			bm::pnl::pnlFuelAvail_vrt,
			0.0244
	};

	bco::on_off_display		lightRCSAvail_{
		bm::vc::RCSSupplyOnLight_id,
			bm::vc::RCSSupplyOnLight_vrt,
			bm::pnl::pnlRCSAvail_id,
			bm::pnl::pnlRCSAvail_vrt,
			0.0244
	};

	// Load FUEL pump
	bco::simple_event<>		btnFuelValveOpen_{
		bm::vc::FuelValveOpenSwitch_loc,
			0.01,
			bm::pnl::pnlFuelValveSwitch_RC
	};

	bco::on_off_display		lightFuelValveOpen_{
		bm::vc::FuelValveOpenSwitch_id,
			bm::vc::FuelValveOpenSwitch_vrt,
			bm::pnl::pnlFuelValveSwitch_id,
			bm::pnl::pnlFuelValveSwitch_vrt,
			0.0352
	};

	// Load RCS pump
	bco::simple_event<>		btnRCSValveOpen_{
		bm::vc::RCSValveOpenSwitch_loc,
			0.01,
			bm::pnl::pnlRCSValveSwitch_RC
	};

	bco::on_off_display		lightRCSValveOpen_{
		bm::vc::RCSValveOpenSwitch_id,
			bm::vc::RCSValveOpenSwitch_vrt,
			bm::pnl::pnlRCSValveSwitch_id,
			bm::pnl::pnlRCSValveSwitch_vrt,
			0.0352
	};

	bco::status_display     statusFuel_     {           
		bm::vc::MsgLightFuelWarn_id,
		bm::vc::MsgLightFuelWarn_vrt,
		bm::pnl::pnlMsgLightFuelWarn_id,
		bm::pnl::pnlMsgLightFuelWarn_vrt,
		0.0361
	};

	bco::status_display     statusLimiter_     {           
		bm::vc::MsgLightThrustLimit_id,
		bm::vc::MsgLightThrustLimit_vrt,
		bm::pnl::pnlMsgLightThrustLimit_id,
		bm::pnl::pnlMsgLightThrustLimit_vrt,
		0.0361
	};
};