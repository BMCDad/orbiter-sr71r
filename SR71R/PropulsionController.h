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

#include "../bc_orbiter/Animation.h"
#include "../bc_orbiter/OnOffInput.h"
#include "../bc_orbiter/RotaryDisplay.h"
#include "../bc_orbiter/Vessel.h"
#include "../bc_orbiter/VesselEvent.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "SR71r_mesh.h"
#include "SR71rVC_mesh.h"
#include "SR71rPanel_mesh.h"
#include "SR71rPanelRight_mesh.h"

#include "Common.h"
#include "ShipMets.h"

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

class VESSEL3;

/**
Manages propulsion and propellents for the Vessel including thrust limiter and fuel
fill, transfer, and dump functions.

Thrust Limit:
The Thrust Limit switch has two modes; A/B (down) limits maximum thrust to the SR71's after burner
thrust at sea Level (68000 lbs).  FULL mode boots the top thrust Level to about 110000 lbs.

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

class PropulsionController : public bco::VesselComponent, public bco::PowerConsumer
{
public:
    PropulsionController(bco::PowerProvider& pwr, bco::Vessel& vessel);

    // PostStep
    void HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd) override;

    // PowerConsumer
    double AmpDraw() const { return (isFilling_ ? 4.0 : 0.0) + (isRCSFilling_ ? 4.0 : 0.0); }

    // SetClassCaps
    void HandleSetClassCaps(bco::Vessel& vessel) override;

    // ManageState
    bool HandleLoadState(bco::Vessel& vessel, const std::string& line) override;
    std::string HandleSaveState(bco::Vessel& vessel) override;

    // DrawHud
    void HandleDrawHud(bco::Vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;

    double GetVesselMainThrustLevel();
    void SetVesselMainThrustLevel(double level);
    void SetAttitudeRotLevel(bco::Axis axis, double level);
    double CurrentMaxThrust() { return maxThrustLevel_; }

//    bco::signal<double>& MainFuelLevelSignal() { return signalMainFuelLevel_; }

    double MainFuelLevel() const { return mainFuelLevel_; }

    void ToggleThrustLimit(VESSEL4& vessel) { switchThrustLimit_.toggle_state(vessel); }

private:
    bco::Vessel& vessel_;
    bco::PowerProvider& power_;

    bool IsPowered() { return power_.VoltsAvailable() > 24.0; }

//    bco::signal<double>		signalMainFuelLevel_;

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

    const char* ConfigKey = "PROPULSION";
    double		mainFuelLevel_;
    double		rcsFuelLevel_;
    double		maxThrustLevel_;
    int			areaId_;
    double		prevTime_;
    bool		isFilling_{ false };
    bool		isExternAvail_{ false };

    bool		isRCSFilling_{ false };

    // Switches
    bco::OnOffInput		switchThrustLimit_{		// Thrust Limit
        { bm::vc::swThrottleLimit_id },
        bm::vc::swThrottleLimit_loc, bm::vc::navPanelAxis_loc,
        cmn::toggleOnOff,
        bm::pnl::pnlThrottleLimit_id,
        bm::pnl::pnlThrottleLimit_vrt,
        bm::pnl::pnlThrottleLimit_RC
    };

    bco::OnOffInput       switchFuelDump_{        // Fuel dump
        { bm::vc::swDumpFuel_id },
        bm::vc::swDumpFuel_loc, bm::vc::FuelTransferRightAxis_loc,
        cmn::toggleOnOff,
        bm::pnlright::pnlFuelDump_id,
        bm::pnlright::pnlFuelDump_vrt,
        bm::pnlright::pnlFuelDump_RC,
        1
    };

    // Gauges
    bco::RotaryDisplay<bco::AnimationTarget>		gaugeFuelFlow_{
        { bm::vc::gaFuelFlow_id },
        bm::vc::gaFuelFlow_loc, bm::vc::FuelFlowAxisFront_loc,
        bm::pnl::pnlGaFuelFlow_id,
        bm::pnl::pnlGaFuelFlow_vrt,
        (270 * RAD),	// Clockwise
        1.0
    };

    bco::RotaryDisplay<bco::AnimationTarget>		gaugeFuelMain_{
        { bm::vc::gaMainFuel_id },
        bm::vc::gaMainFuel_loc, bm::vc::FuelLevelAxisFront_loc,
        bm::pnl::pnlGaFuelMain_id,
        bm::pnl::pnlGaFuelMain_vrt,
        (256 * RAD),	// Clockwise
        1.0
    };

    bco::RotaryDisplay<bco::AnimationTarget>		gaugeFuelRCS_{
        { bm::vc::gaRCSFuel_id },
        bm::vc::gaRCSFuel_loc, bm::vc::RCSLevelAxisFront_loc,
        bm::pnl::pnlGaFuelRCS_id,
        bm::pnl::pnlGaFuelRCS_vrt,
        (264 * RAD),	// Clockwise
        1.0
    };

    // Displays
    bco::VesselTextureElement       lightFuelAvail_ {
        bm::vc::FuelSupplyOnLight_id,
        bm::vc::FuelSupplyOnLight_vrt,
        cmn::vc::main,
        bm::pnlright::pnlFuelAvail_id,
        bm::pnlright::pnlFuelAvail_vrt,
        cmn::panel::right
    };

    bco::VesselTextureElement       lightRCSAvail_{
        bm::vc::RCSSupplyOnLight_id,
        bm::vc::RCSSupplyOnLight_vrt,
        cmn::vc::main,
        bm::pnlright::pnlRCSAvail_id,
        bm::pnlright::pnlRCSAvail_vrt,
        cmn::panel::right
    };

    // Load FUEL pump
    bco::VesselEvent     btnFuelValveOpen_{
        bm::vc::FuelValveOpenSwitch_loc,
        0.01,
        cmn::vc::main,
        bm::pnlright::pnlFuelValveSwitch_RC,
        cmn::panel::right
    };

    bco::VesselTextureElement       lightFuelValveOpen_{
        bm::vc::FuelValveOpenSwitch_id,
        bm::vc::FuelValveOpenSwitch_vrt,
        cmn::vc::main,
        bm::pnlright::pnlFuelValveSwitch_id,
        bm::pnlright::pnlFuelValveSwitch_vrt,
        cmn::panel::right
    };

    // Load RCS pump
    bco::VesselEvent     btnRCSValveOpen_{
        bm::vc::RCSValveOpenSwitch_loc,
        0.01,
        cmn::vc::main ,
        bm::pnlright::pnlRCSValveSwitch_RC,
        cmn::panel::right
    };

    bco::VesselTextureElement       lightRCSValveOpen_ {
        bm::vc::RCSValveOpenSwitch_id,
        bm::vc::RCSValveOpenSwitch_vrt,
        cmn::vc::main,
        bm::pnlright::pnlRCSValveSwitch_id,
        bm::pnlright::pnlRCSValveSwitch_vrt,
        cmn::panel::right
    };

    bco::VesselTextureElement       statusFuel_ {
        bm::vc::MsgLightFuelWarn_id,
        bm::vc::MsgLightFuelWarn_vrt,
        cmn::vc::main,
        bm::pnl::pnlMsgLightFuelWarn_id,
        bm::pnl::pnlMsgLightFuelWarn_vrt,
        cmn::panel::main
    };

    bco::VesselTextureElement       statusLimiter_{
        bm::vc::MsgLightThrustLimit_id,
        bm::vc::MsgLightThrustLimit_vrt,
        cmn::vc::main,
        bm::pnl::pnlMsgLightThrustLimit_id,
        bm::pnl::pnlMsgLightThrustLimit_vrt,
        cmn::panel::main
    };
};

inline PropulsionController::PropulsionController(bco::PowerProvider& pwr, bco::Vessel& vessel) :
    power_(pwr),
    vessel_(vessel),
    mainFuelLevel_(0.0),
    rcsFuelLevel_(0.0)
    //slotThrottleLimit_(	[&](bool v) { SetThrustLevel((v) ? ENGINE_THRUST : ENGINE_THRUST_AB); }),
    //slotFuelDump_(		[&](bool v) { }), 
    //slotTransferSel_(	[&](bool v) { }),
    //slotFuelValveOpen_(	[&](bool b) { ToggleFill(); slotFuelValveOpen_.set(); })
{
    power_.AttachConsumer(this);

    maxMainFlow_ = (ENGINE_THRUST / THRUST_ISP) * 2;

    vessel.AddControl(&switchFuelDump_);
    vessel.AddControl(&switchThrustLimit_);
    vessel.AddControl(&gaugeFuelFlow_);
    vessel.AddControl(&gaugeFuelMain_);
    vessel.AddControl(&gaugeFuelRCS_);
    vessel.AddControl(&btnFuelValveOpen_);
    vessel.AddControl(&lightFuelValveOpen_);
    vessel.AddControl(&lightFuelAvail_);
    vessel.AddControl(&lightRCSAvail_);

    vessel.AddControl(&btnRCSValveOpen_);
    vessel.AddControl(&lightRCSValveOpen_);

    vessel.AddControl(&statusFuel_);
    vessel.AddControl(&statusLimiter_);

    switchThrustLimit_.AttachOnChange([&]() { SetThrustLevel(switchThrustLimit_.IsOn() ? ENGINE_THRUST : ENGINE_THRUST_AB); });

    btnFuelValveOpen_.Attach([&](VESSEL4&) { ToggleFill(); });
    btnRCSValveOpen_.Attach([&](VESSEL4&) { ToggleRCSFill(); });
}

inline void PropulsionController::ToggleFill()
{
    if (isFilling_) {
        isFilling_ = false;
    }
    else {
        if (IsPowered() && isExternAvail_) {
            isFilling_ = true;
        }
    }
}

inline void PropulsionController::ToggleRCSFill()
{
    if (isRCSFilling_) {
        isRCSFilling_ = false;
    }
    else {
        if (IsPowered() && isExternAvail_) {
            isRCSFilling_ = true;
        }
    }
}

inline void PropulsionController::HandlePostStep(bco::Vessel& vessel, double simt, double simdt, double mjd)
{
    // Limit how often we run
    if (fabs(simt - prevTime_) > 0.1) {
        Update((simt - prevTime_));
        prevTime_ = simt;
    }
}

inline void PropulsionController::Update(double deltaUpdate)
{
    isExternAvail_ = (IsPowered() && vessel_.IsStoppedOrDocked());

    lightFuelAvail_.SetState(vessel_, isExternAvail_);

    lightRCSAvail_.SetState(vessel_, isExternAvail_);

    if (!isExternAvail_) {
        isFilling_ = false;
        isRCSFilling_ = false;
    }

    lightFuelValveOpen_.SetState(vessel_, isFilling_);

    lightRCSValveOpen_.SetState(vessel_, isRCSFilling_);

    HandleTransfer(deltaUpdate); // <- this sets the current levels.

    // Main flow
    auto flow = vessel_.GetPropellantFlowrate(vessel_.MainPropellant());
    //    auto trFlow = (flow / maxMainFlow_) * (PI2 * 0.75);	// 90.718 = 200lbs per hour : 270 deg.
    if ((flow < 0.0) || switchFuelDump_.IsOn()) flow = 0.0;	 // Don't report flow if dumping.
    //	sigFuelFlowRate_.fire(flow / maxMainFlow_);	 // Converted to 0-1 range.
    gaugeFuelFlow_.SetState(flow / maxMainFlow_);

    statusLimiter_.SetState(vessel_,
        (!IsPowered() || switchThrustLimit_.IsOn())
        ? cmn::status::off
        : cmn::status::on);
}

inline void PropulsionController::HandleTransfer(double deltaUpdate)
{
    // Determine fuel levels:
    mainFuelLevel_ = vessel_.GetPropellantMass(vessel_.MainPropellant()) /
        vessel_.GetPropellantMaxMass(vessel_.MainPropellant());

    rcsFuelLevel_ = vessel_.GetPropellantMass(vessel_.RcsPropellant()) /
        vessel_.GetPropellantMaxMass(vessel_.RcsPropellant());

    gaugeFuelMain_.SetState(mainFuelLevel_);
    gaugeFuelRCS_.SetState(rcsFuelLevel_);

    // Dumping and filling of the main tank will happen regardless of the transfer switch position.
    if (isFilling_) { // cannot be enabled if external fuel is unavailable.
        // Add to main.
        auto actualFill = FillMainFuel(FUEL_FILL_RATE * deltaUpdate);
        if (actualFill <= 0.0) {
            isFilling_ = false;
        }
    }

    if (isRCSFilling_) {
        auto actualFill = FillRCSFuel(FUEL_FILL_RATE * deltaUpdate);
        if (actualFill <= 0.0) {
            isRCSFilling_ = false;
        }
    }

    if (IsPowered() && switchFuelDump_.IsOn()) {
        auto actualDump = DrawMainFuel(FUEL_DUMP_RATE * deltaUpdate);
    }

//    signalMainFuelLevel_.fire(mainFuelLevel_);

    auto fuelStatus = (mainFuelLevel_ > 0.2) || !IsPowered()
        ? cmn::status::off : mainFuelLevel_ == 0.0
        ? cmn::status::error : cmn::status::warn;

    statusFuel_.SetState(vessel_, fuelStatus);
}

inline double PropulsionController::GetVesselMainThrustLevel()
{
    return vessel_.GetThrusterGroupLevel(THGROUP_MAIN);
}

inline double PropulsionController::DrawMainFuel(double amount)
{
    auto mainProp = vessel_.MainPropellant();

    // Subtract the minimum fuel allowed to determine what available current is.
    auto current = vessel_.GetPropellantMass(mainProp);
    auto currentAvail = current - FUEL_MINIMUM_DUMP;

    if (currentAvail <= 0.0) {
        // Not enough fuel to dump.
        return 0.0;
    }

    auto actual = min(amount, currentAvail);

    vessel_.SetPropellantMass(mainProp, (current - actual));
    return actual;
}

inline double PropulsionController::FillMainFuel(double amount)
{
    auto mainProp = vessel_.MainPropellant();

    auto current = vessel_.GetPropellantMass(mainProp);
    auto topOff = MAX_FUEL - current;

    auto result = min(amount, topOff);
    vessel_.SetPropellantMass(mainProp, (current + result));
    return result;
}

inline double PropulsionController::DrawRCSFuel(double amount)
{
    auto rcsProp = vessel_.RcsPropellant();

    auto current = vessel_.GetPropellantMass(rcsProp);

    auto result = min(amount, current);
    vessel_.SetPropellantMass(rcsProp, (current - result));
    return result;
}

inline double PropulsionController::FillRCSFuel(double amount)
{
    auto rcsProp = vessel_.RcsPropellant();

    auto current = vessel_.GetPropellantMass(rcsProp);
    auto topOff = MAX_RCS_FUEL - current;

    auto result = min(amount, topOff);
    vessel_.SetPropellantMass(rcsProp, (current + result));
    return result;
}

inline void PropulsionController::HandleSetClassCaps(bco::Vessel& vessel)
{
    //	Start with max thrust (ENGINE_THRUST) this will change base on the max thrust selector.
    mainThrustHandles_[0] = vessel.CreateThruster(
        bm::main::ThrusterP_loc,
        _V(0, 0, 1),
        ENGINE_THRUST,
        vessel.MainPropellant(),
        THRUST_ISP);

    mainThrustHandles_[1] = vessel.CreateThruster(
        bm::main::ThrusterS_loc,
        _V(0, 0, 1),
        ENGINE_THRUST,
        vessel.MainPropellant(),
        THRUST_ISP);

    vessel.CreateThrusterGroup(mainThrustHandles_, 2, THGROUP_MAIN);

    EXHAUSTSPEC es_main[2] = {
       { mainThrustHandles_[0], NULL, NULL, NULL, 12, 1, 0, 0.1, NULL },
       { mainThrustHandles_[1], NULL, NULL, NULL, 12, 1, 0, 0.1, NULL }
    };

    PARTICLESTREAMSPEC exhaust_main = {
       0, 2.0, 13, 150, 0.1, 0.2, 16, 1.0, PARTICLESTREAMSPEC::EMISSIVE,
       PARTICLESTREAMSPEC::LVL_SQRT, 0, 1,
       PARTICLESTREAMSPEC::ATM_PLOG, 1e-5, 0.1
    };

    for (auto i = 0; i < 2; i++) vessel.AddExhaust(es_main + i);
    vessel.AddExhaustStream(mainThrustHandles_[0], _V(-4.6230, 0, -11), &exhaust_main);
    vessel.AddExhaustStream(mainThrustHandles_[1], _V(4.6230, 0, -11), &exhaust_main);

    THRUSTER_HANDLE th_att_rot[4], th_att_lin[4];

    // Pitch up/down attack points are simplified 8 meters forward and aft.
    th_att_rot[0] = th_att_lin[0] = vessel.CreateThruster(_V(0, 0, 8), _V(0, 1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
    th_att_rot[1] = th_att_lin[3] = vessel.CreateThruster(_V(0, 0, -8), _V(0, -1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
    th_att_rot[2] = th_att_lin[2] = vessel.CreateThruster(_V(0, 0, 8), _V(0, -1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
    th_att_rot[3] = th_att_lin[1] = vessel.CreateThruster(_V(0, 0, -8), _V(0, 1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);

    vessel.CreateThrusterGroup(th_att_rot, 2, THGROUP_ATT_PITCHUP);
    vessel.CreateThrusterGroup(th_att_rot + 2, 2, THGROUP_ATT_PITCHDOWN);
    vessel.CreateThrusterGroup(th_att_lin, 2, THGROUP_ATT_UP);
    vessel.CreateThrusterGroup(th_att_lin + 2, 2, THGROUP_ATT_DOWN);

    // Exhaust visuals
    vessel.AddExhaust(th_att_rot[0], 0.6, 0.078, bm::main::RCS_FL_DOWN_loc, _V(0, -1, 0));
    vessel.AddExhaust(th_att_rot[0], 0.6, 0.078, bm::main::RCS_FR_DOWN_loc, _V(0, -1, 0));
    vessel.AddExhaust(th_att_rot[1], 0.79, 0.103, bm::main::RCS_RL_UP_loc, _V(0, 1, 0));
    vessel.AddExhaust(th_att_rot[1], 0.79, 0.103, bm::main::RCS_RR_UP_loc, _V(0, 1, 0));
    vessel.AddExhaust(th_att_rot[2], 0.6, 0.078, bm::main::RCS_FL_UP_loc, _V(0, 1, 0));
    vessel.AddExhaust(th_att_rot[2], 0.6, 0.078, bm::main::RCS_FR_UP_loc, _V(0, 1, 0));
    vessel.AddExhaust(th_att_rot[3], 0.79, 0.103, bm::main::RCS_RL_DOWN_loc, _V(0, -1, 0));
    vessel.AddExhaust(th_att_rot[3], 0.79, 0.103, bm::main::RCS_RR_DOWN_loc, _V(0, -1, 0));


    // Yaw left and right simplified at 10 meters.
    th_att_rot[0] = th_att_lin[0] = vessel.CreateThruster(_V(0, 0, 10), _V(-1, 0, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
    th_att_rot[1] = th_att_lin[3] = vessel.CreateThruster(_V(0, 0, -10), _V(1, 0, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
    th_att_rot[2] = th_att_lin[2] = vessel.CreateThruster(_V(0, 0, 10), _V(1, 0, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
    th_att_rot[3] = th_att_lin[1] = vessel.CreateThruster(_V(0, 0, -10), _V(-1, 0, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);

    vessel.CreateThrusterGroup(th_att_rot, 2, THGROUP_ATT_YAWLEFT);
    vessel.CreateThrusterGroup(th_att_rot + 2, 2, THGROUP_ATT_YAWRIGHT);
    vessel.CreateThrusterGroup(th_att_lin, 2, THGROUP_ATT_LEFT);
    vessel.CreateThrusterGroup(th_att_lin + 2, 2, THGROUP_ATT_RIGHT);

    vessel.AddExhaust(th_att_rot[0], 0.6, 0.078, bm::main::RCS_FR_RIGHT_loc, _V(1, 0, 0));
    vessel.AddExhaust(th_att_rot[1], 0.94, 0.122, bm::main::RCS_RL_LEFT_loc, _V(-1, 0, 0));
    vessel.AddExhaust(th_att_rot[2], 0.6, 0.078, bm::main::RCS_FL_LEFT_loc, _V(-1, 0, 0));
    vessel.AddExhaust(th_att_rot[3], 0.94, 0.122, bm::main::RCS_RR_RIGHT_loc, _V(1, 0, 0));

    // Bank left and right.

    th_att_rot[0] = vessel.CreateThruster(_V(5, 0, 0), _V(0, 1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
    th_att_rot[1] = vessel.CreateThruster(_V(-5, 0, 0), _V(0, -1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
    th_att_rot[2] = vessel.CreateThruster(_V(-5, 0, 0), _V(0, 1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
    th_att_rot[3] = vessel.CreateThruster(_V(5, 0, 0), _V(0, -1, 0), RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);

    vessel.CreateThrusterGroup(th_att_rot, 2, THGROUP_ATT_BANKLEFT);
    vessel.CreateThrusterGroup(th_att_rot + 2, 2, THGROUP_ATT_BANKRIGHT);

    vessel.AddExhaust(th_att_rot[0], 1.03, 0.134, bm::main::RCS_L_TOP_loc, _V(0, 1, 0));
    vessel.AddExhaust(th_att_rot[1], 1.03, 0.134, bm::main::RCS_R_BOTTOM_loc, _V(0, -1, 0));
    vessel.AddExhaust(th_att_rot[2], 1.03, 0.134, bm::main::RCS_R_TOP_loc, _V(0, 1, 0));
    vessel.AddExhaust(th_att_rot[3], 1.03, 0.134, bm::main::RCS_L_BOTTOM_loc, _V(0, -1, 0));


    // Forward and back
    th_att_lin[0] = vessel.CreateThruster(_V(0, 0, -7), _V(0, 0, 1), 2 * RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);
    th_att_lin[1] = vessel.CreateThruster(_V(0, 0, 7), _V(0, 0, -1), 2 * RCS_THRUST, vessel.RcsPropellant(), THRUST_ISP);

    vessel.CreateThrusterGroup(th_att_lin, 1, THGROUP_ATT_FORWARD);
    vessel.CreateThrusterGroup(th_att_lin + 1, 1, THGROUP_ATT_BACK);

    vessel.AddExhaust(th_att_lin[0], 0.6, 0.078, bm::main::RCS_RL_LEFT_loc, _V(0, 0, -1));
    vessel.AddExhaust(th_att_lin[0], 0.6, 0.078, bm::main::RCS_RR_RIGHT_loc, _V(0, 0, -1));
    vessel.AddExhaust(th_att_lin[1], 0.6, 0.078, bm::main::RCS_L_FORWARD_loc, _V(0, 0, 1));
    vessel.AddExhaust(th_att_lin[1], 0.6, 0.078, bm::main::RCS_R_FORWARD_loc, _V(0, 0, 1));
}

inline bool PropulsionController::HandleLoadState(bco::Vessel& vessel, const std::string& line)
{
    std::istringstream in(line);
    in >> switchThrustLimit_;
    return true;
}

inline std::string PropulsionController::HandleSaveState(bco::Vessel& vessel)
{
    std::ostringstream os;
    os << switchThrustLimit_;
    return os.str();
}

inline void PropulsionController::SetVesselMainThrustLevel(double level)
{
    vessel_.SetThrusterGroupLevel(THGROUP_MAIN, level);
}

inline void PropulsionController::SetAttitudeRotLevel(bco::Axis axis, double level)
{
    vessel_.SetAttitudeRotLevel((int)axis, level);
}

inline void PropulsionController::SetThrustLevel(double newLevel)
{
    maxThrustLevel_ = newLevel;
    vessel_.SetThrusterMax0(mainThrustHandles_[0], maxThrustLevel_);
    vessel_.SetThrusterMax0(mainThrustHandles_[1], maxThrustLevel_);
}

inline void PropulsionController::HandleDrawHud(bco::Vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return;

    auto th = vessel.GetThrusterGroupLevel(THGROUP_MAIN);
    auto hv = vessel.GetThrusterGroupLevel(THGROUP_HOVER);
    auto rt = vessel.GetThrusterGroupLevel(THGROUP_RETRO);


    char cbuf[16];
    int lvl = 0;
    int xLeft = hps->W - 62;
    int yTop = hps->H - 90;

    int txX = hps->W - 56;
    int bxLeft = hps->W - 62;
    int bxRight = bxLeft + 45;
    int bxHeight = 25;
    int lbX = hps->W - 12;


    // Main thrust
    skp->Rectangle(
        bxLeft,
        yTop,
        bxRight,
        yTop + bxHeight);

    auto mt = 0.0;
    const char* LB = (th >= rt) ? "M" : "R";
    if (th >= rt) {
        mt = th;
        LB = "M";
    }
    else {
        mt = rt;
        LB = "R";
    }

    lvl = (int)(mt * 100);
    sprintf_s(cbuf, "%3i", lvl);
    skp->Text(txX, yTop + 1, cbuf, 3);
    skp->Text(lbX, yTop + 1, LB, 1);

    // Hover
    yTop += 30;
    skp->Rectangle(
        bxLeft,
        yTop,
        bxRight,
        yTop + bxHeight);

    lvl = (int)(hv * 100);
    sprintf_s(cbuf, "%3i", lvl);

    skp->Text(txX, yTop + 1, cbuf, 3);
    skp->Text(lbX, yTop + 1, "H", 1);
}