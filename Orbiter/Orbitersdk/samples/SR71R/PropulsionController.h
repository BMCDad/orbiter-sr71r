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

#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\MeshVisual.h"
#include "bc_orbiter\OnOffSwitch.h"
#include "bc_orbiter\TextureVisual.h"
#include "bc_orbiter\PushButtonSwitch.h"
#include "bc_orbiter\RotarySwitch.h"
#include "bc_orbiter\VCGauge.h"
#include "bc_orbiter\VCToggleSwitch.h"

#include "PropulsionController.h"
#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class VESSEL3;

const double FUEL_FILL_RATE     =  170.0;
const double FUEL_TRANFER_RATE  =   20.0;	// Transfers between tanks will be slow.
const double FUEL_DUMP_RATE     =  130.0;
const double FUEL_MINIMUM_DUMP  = 8400.0 * 0.4536;	// LBS. -> kg.

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

class PropulsionController : public bco::PoweredComponent
{
public:
	PropulsionController(bco::BaseVessel* vessel, double amps);

    enum class Axis { Pitch = 0, Yaw = 1, Roll = 2 };

	void Step(double simt, double simdt, double mjd);

	virtual void SetClassCaps() override;
	virtual bool LoadVC(int id) override;
	virtual bool VCRedrawEvent(int id, int event, SURFHANDLE surf) override;
	virtual bool LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	virtual void SaveConfiguration(FILEHANDLE scn) const override;

	double GetMainFuelLevel()			{ return mainFuelLevel_; }
	double GetRcsFuelLevel()			{ return rcsFuelLevel_; }
	double GetVesselMainThrustLevel();
	void SetVesselMainThrustLevel(double level);
    void SetAttitudeRotLevel(Axis axis, double level);
	double CurrentMaxThrust() { return maxThrustLevel_; }

	double DrawMainFuel(double amount);
	double FillMainFuel(double amount);

	double DrawRCSFuel(double amount);
	double FillRCSFuel(double amount);

	bco::OnOffSwitch&		ThrustLimitSwitch() { return swThrustLimit_; }

    bool DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

private:
	enum TransMode
	{
		Main = 0,
		None = 1,
		RCS = 2
	};

	TransMode		transMode_;

	void SetThrustLevel(double newLevel);
	void Update(double deltaUpdate);

	void ToggleFill();
	void ToggleTransfer();
	void SetDumpOn(bool mode);

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

	bool		isFilling_;
	bool		isAvailable_;

	bool		isTransfering_;
	bool		isDumping_;



    bco::VCGauge            gaFuelFlow_         { {bt_mesh::SR71rVC::gaFuelFlow_id},
                                                    bt_mesh::SR71rVC::gaFuelFlow_location,   
                                                    bt_mesh::SR71rVC::FuelFlowAxisFront_location,
                                                    (RAD*270), 
                                                    1.0
                                                };

    bco::VCGauge            gaFuelMain_         { {bt_mesh::SR71rVC::gaMainFuel_id },
                                                    bt_mesh::SR71rVC::gaMainFuel_location, 
                                                    bt_mesh::SR71rVC::FuelLevelAxisFront_location, 
                                                    (RAD*256), 
                                                    1.0 
                                                };

    bco::VCGauge            gaFuelRCS_          { {bt_mesh::SR71rVC::gaRCSFuel_id },
                                                    bt_mesh::SR71rVC::gaRCSFuel_location, 
                                                    bt_mesh::SR71rVC::RCSLevelAxisFront_location, 
                                                    (RAD*264), 
                                                    1.0
                                                };
	// Thrust limit
    bco::VCToggleSwitch     swThrustLimit_      {   bt_mesh::SR71rVC::swThrottleLimit_id,
                                                    bt_mesh::SR71rVC::swThrottleLimit_location, 
                                                    bt_mesh::SR71rVC::TopRowSwitchRightAxis_location
                                                };

    bco::VCToggleSwitch     swSelectTransfer_   {   bt_mesh::SR71rVC::swTransferSelect_id,
                                                    bt_mesh::SR71rVC::swTransferSelect_location,
                                                    bt_mesh::SR71rVC::FuelTransferRightAxis_location
                                                };

	bco::TextureVisual		visTranferPumpOn_;
    bco::PushButtonSwitch   swTransferPump_     {   bt_mesh::SR71rVC::FuelTransferSwitch_location,			0.01 };

    bco::VCToggleSwitch     swDumpFuel_         {   bt_mesh::SR71rVC::swDumpFuel_id,
                                                    bt_mesh::SR71rVC::swDumpFuel_location, 
                                                    bt_mesh::SR71rVC::FuelTransferRightAxis_location
                                                };

	// Fill
	bco::TextureVisual		lightFuelSupplyAvailable_;
	bco::TextureVisual		lightFuelSupplyValveOpen_;
	bco::PushButtonSwitch	switchFuelSupplyValveOpen_  { bt_mesh::SR71rVC::FuelValveOpenSwitch_location,    0.01 };
};