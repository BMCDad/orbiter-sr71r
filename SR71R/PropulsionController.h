/*
PropulsionController - SR-71r Orbiter Addon
Copyright(C) 2025  Blake Christensen

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Orbitersdk.h"

#include "..\bc_orbiter\Vessel.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "IFuelSystem.h"
#include "IPowerProvider.h"
#include "SR71Toggle.h"
#include "SR71Gauge.h"
#include "SR71Light.h"
#include "SR71Button.h"

namespace bco = bc_orbiter;

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
b = 0/1 Fill valve is open.
*/

class PropulsionController : public IFuelSystem
{
public:
    PropulsionController(bco::Vessel& vessel);
    ~PropulsionController() = default;

    void Setup(bco::Vessel& vessel);

    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power);

    double GetMainFuelLevel() const override { return mainFuelRangeValue_; }
    PROPELLANT_HANDLE GetMainProppelantHandle() const { return mainPropellant_; }

    // draw_hud
//    void handle_draw_hud(bco::vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;

    void SetVesselMainThrustLevel(double level);

    void ToggleThrustLimit() { switchThrustLimit_.ToggleState(); }

    void LoadState(const std::string& line);
    std::string GetState() const;

private:
    bco::Vessel& vessel_;

    double FillMainFuel(bco::Vessel& vessel, double amount);

    void ToggleFill() { isFilling_ = !isFilling_; };
    void SetThrustLevel(double newLevel);
    void Update(double deltaUpdate);

    THRUSTER_HANDLE mainThrustHandles_[2];
    THRUSTER_HANDLE retroThrustHandles_[2];

    double      maxMainFlow_        { 0.0 };       	// MAXTHRUST / ISP.
    const char* ConfigKey =         "PROPULSION";
    double      rcsFuelLevel_       { 0.0 };
    double      maxThrustLevel_     { 0.0 };
    int         areaId_             { 0 };
    double      prevTime_           { 0.0 };
    bool        isFilling_          { false };
    bool        isExternAvail_      { false };

    bool        isRCSFilling_       { false };

    double      fuelFlowRangeValue_ { 0.0 };  // Value to drive the fuel flow gauge.
    double      mainFuelRangeValue_ { 0.0 };    // Value to drive the main fuel gauge.
    double      rcsFuelRangeValue_  { 0.0 };     // Value to drive the RCS fuel gauge. 

    PROPELLANT_HANDLE   mainPropellant_{ nullptr };


    // Switches
    SR71::Toggle   switchThrustLimit_{		// Thrust Limit
        bm::vc::swThrottleLimit_id,
        bm::vc::swThrottleLimit_loc, bm::vc::navPanelAxis_loc,
        bm::pnl::pnlThrottleLimit_id,
        bm::pnl::pnlThrottleLimit_vrt,
        bm::pnl::pnlThrottleLimit_RC,
        SR71R::MainPanel_ID
    };

    // Gauges
    SR71::Gauge gaugeFuelFlow_{
        bm::vc::gaFuelFlow_id,
        bm::vc::gaFuelFlow_loc, bm::vc::FuelFlowAxisFront_loc,
        bm::pnl::pnlGaFuelFlow_id,
        bm::pnl::pnlGaFuelFlow_vrt,
        (270 * RAD),	// Clockwise
        fuelFlowRangeValue_,
        SR71R::MainPanel_ID
    };

    SR71::Gauge gaugeFuelMain_{
        bm::vc::gaMainFuel_id,
        bm::vc::gaMainFuel_loc, bm::vc::FuelLevelAxisFront_loc,
        bm::pnl::pnlGaFuelMain_id,
        bm::pnl::pnlGaFuelMain_vrt,
        (256 * RAD),	// Clockwise
        mainFuelRangeValue_,
        SR71R::MainPanel_ID
    };

    SR71::Gauge gaugeFuelRCS_{
        bm::vc::gaRCSFuel_id,
        bm::vc::gaRCSFuel_loc, bm::vc::RCSLevelAxisFront_loc,
        bm::pnl::pnlGaFuelRCS_id,
        bm::pnl::pnlGaFuelRCS_vrt,
        (264 * RAD),	// Clockwise
        rcsFuelRangeValue_,
        SR71R::MainPanel_ID
    };

    // Displays

    SR71::Light lightFuelAvail_{
        bm::vc::FuelSupplyOnLight_id,
        bm::vc::FuelSupplyOnLight_vrt,
        bm::pnlright::pnlFuelAvail_id,
        bm::pnlright::pnlFuelAvail_vrt,
        SR71R::RightPanel_ID
    };

    SR71::Light lightRCSAvail_{
        bm::vc::RCSSupplyOnLight_id,
        bm::vc::RCSSupplyOnLight_vrt,
        bm::pnlright::pnlRCSAvail_id,
        bm::pnlright::pnlRCSAvail_vrt,
        SR71R::RightPanel_ID
    };

    SR71::Button btnFuelValveOpen_{
        bm::vc::FuelValveOpenSwitch_id,
        bm::vc::FuelValveOpenSwitch_loc,
        bm::vc::FuelValveOpenSwitch_vrt,
        bm::pnlright::pnlFuelValveSwitch_id,
        bm::pnlright::pnlFuelValveSwitch_vrt,
        bm::pnlright::pnlFuelValveSwitch_RC,
        SR71R::RightPanel_ID,
        [this](bool) { ToggleFill(); }
    };

    SR71::Light     statusFuel_{
        bm::vc::MsgLightFuelWarn_id,
        bm::vc::MsgLightFuelWarn_vrt,
        bm::pnl::pnlMsgLightFuelWarn_id,
        bm::pnl::pnlMsgLightFuelWarn_vrt,
        SR71R::MainPanel_ID
    };

    SR71::Light     statusLimiter_{
        bm::vc::MsgLightThrustLimit_id,
        bm::vc::MsgLightThrustLimit_vrt,
        bm::pnl::pnlMsgLightThrustLimit_id,
        bm::pnl::pnlMsgLightThrustLimit_vrt,
        SR71R::MainPanel_ID
    };

};

inline PropulsionController::PropulsionController(bco::Vessel& vessel) :
    vessel_(vessel),
    mainThrustHandles_{nullptr, nullptr},
    retroThrustHandles_{ nullptr, nullptr }
{
    maxMainFlow_ = (SR71R::ENGINE_THRUST / SR71R::THRUST_ISP) * 2;

    vessel.RegisterUIControl(switchThrustLimit_);
    vessel.RegisterUIControl(gaugeFuelFlow_);
    vessel.RegisterUIControl(gaugeFuelMain_);
    vessel.RegisterUIControl(gaugeFuelRCS_);
    vessel.RegisterUIControl(lightFuelAvail_);
    vessel.RegisterUIControl(lightRCSAvail_);
    vessel.RegisterUIControl(btnFuelValveOpen_);

    vessel.RegisterUIControl(statusFuel_);
    vessel.RegisterUIControl(statusLimiter_);
}

inline void PropulsionController::Setup(bco::Vessel& vessel)
{
    mainPropellant_ = vessel.CreatePropellantResource(SR71R::MAX_FUEL);

    //	Start with max thrust (ENGINE_THRUST) this will change base on the max thrust selector.
    mainThrustHandles_[0] = vessel.CreateThruster(
        bm::main::ThrusterP_loc,
        _V(0, 0, 1),
        SR71R::ENGINE_THRUST,
        mainPropellant_,
        SR71R::THRUST_ISP);

    mainThrustHandles_[1] = vessel.CreateThruster(
        bm::main::ThrusterS_loc,
        _V(0, 0, 1),
        SR71R::ENGINE_THRUST,
        mainPropellant_,
        SR71R::THRUST_ISP);

    vessel.CreateThrusterGroup(mainThrustHandles_, 2, THGROUP_MAIN);

    EXHAUSTSPEC es_main[2] =
    {
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
    th_att_rot[0] = th_att_lin[0] = vessel.CreateThruster(_V(0, 0,  8), _V(0,  1, 0), SR71R::RCS_THRUST, mainPropellant_, SR71R::THRUST_ISP);
    th_att_rot[1] = th_att_lin[3] = vessel.CreateThruster(_V(0, 0, -8), _V(0, -1, 0), SR71R::RCS_THRUST, mainPropellant_, SR71R::THRUST_ISP);
    th_att_rot[2] = th_att_lin[2] = vessel.CreateThruster(_V(0, 0,  8), _V(0, -1, 0), SR71R::RCS_THRUST, mainPropellant_, SR71R::THRUST_ISP);
    th_att_rot[3] = th_att_lin[1] = vessel.CreateThruster(_V(0, 0, -8), _V(0, 1, 0), SR71R::RCS_THRUST, mainPropellant_, SR71R::THRUST_ISP);

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
    th_att_rot[0] = th_att_lin[0] = vessel.CreateThruster(_V(0, 0,  10), _V(-1, 0, 0), SR71R::RCS_THRUST, mainPropellant_, SR71R::THRUST_ISP);
    th_att_rot[1] = th_att_lin[3] = vessel.CreateThruster(_V(0, 0, -10), _V( 1, 0, 0), SR71R::RCS_THRUST, mainPropellant_, SR71R::THRUST_ISP);
    th_att_rot[2] = th_att_lin[2] = vessel.CreateThruster(_V(0, 0,  10), _V( 1, 0, 0), SR71R::RCS_THRUST, mainPropellant_, SR71R::THRUST_ISP);
    th_att_rot[3] = th_att_lin[1] = vessel.CreateThruster(_V(0, 0, -10), _V(-1, 0, 0), SR71R::RCS_THRUST, mainPropellant_, SR71R::THRUST_ISP);

    vessel.CreateThrusterGroup(th_att_rot, 2, THGROUP_ATT_YAWLEFT);
    vessel.CreateThrusterGroup(th_att_rot + 2, 2, THGROUP_ATT_YAWRIGHT);
    vessel.CreateThrusterGroup(th_att_lin, 2, THGROUP_ATT_LEFT);
    vessel.CreateThrusterGroup(th_att_lin + 2, 2, THGROUP_ATT_RIGHT);

    vessel.AddExhaust(th_att_rot[0], 0.6, 0.078, bm::main::RCS_FR_RIGHT_loc, _V(1, 0, 0));
    vessel.AddExhaust(th_att_rot[1], 0.94, 0.122, bm::main::RCS_RL_LEFT_loc, _V(-1, 0, 0));
    vessel.AddExhaust(th_att_rot[2], 0.6, 0.078, bm::main::RCS_FL_LEFT_loc, _V(-1, 0, 0));
    vessel.AddExhaust(th_att_rot[3], 0.94, 0.122, bm::main::RCS_RR_RIGHT_loc, _V(1, 0, 0));

    // Bank left and right.

    th_att_rot[0] = vessel.CreateThruster(_V( 5, 0, 0), _V(0,  1, 0), SR71R::RCS_THRUST, mainPropellant_, SR71R::THRUST_ISP);
    th_att_rot[1] = vessel.CreateThruster(_V(-5, 0, 0), _V(0, -1, 0), SR71R::RCS_THRUST, mainPropellant_, SR71R::THRUST_ISP);
    th_att_rot[2] = vessel.CreateThruster(_V(-5, 0, 0), _V(0, 1, 0), SR71R::RCS_THRUST,  mainPropellant_, SR71R::THRUST_ISP);
    th_att_rot[3] = vessel.CreateThruster(_V(5, 0, 0), _V(0, -1, 0), SR71R::RCS_THRUST,  mainPropellant_, SR71R::THRUST_ISP);

    vessel.CreateThrusterGroup(th_att_rot, 2, THGROUP_ATT_BANKLEFT);
    vessel.CreateThrusterGroup(th_att_rot + 2, 2, THGROUP_ATT_BANKRIGHT);

    vessel.AddExhaust(th_att_rot[0], 1.03, 0.134, bm::main::RCS_L_TOP_loc, _V(0, 1, 0));
    vessel.AddExhaust(th_att_rot[1], 1.03, 0.134, bm::main::RCS_R_BOTTOM_loc, _V(0, -1, 0));
    vessel.AddExhaust(th_att_rot[2], 1.03, 0.134, bm::main::RCS_R_TOP_loc, _V(0, 1, 0));
    vessel.AddExhaust(th_att_rot[3], 1.03, 0.134, bm::main::RCS_L_BOTTOM_loc, _V(0, -1, 0));


    // Forward and back
    th_att_lin[0] = vessel.CreateThruster(_V(0, 0, -7), _V(0, 0, 1), 2 * SR71R::RCS_THRUST, mainPropellant_, SR71R::THRUST_ISP);
    th_att_lin[1] = vessel.CreateThruster(_V(0, 0, 7), _V(0, 0, -1), 2 * SR71R::RCS_THRUST, mainPropellant_, SR71R::THRUST_ISP);

    vessel.CreateThrusterGroup(th_att_lin, 1, THGROUP_ATT_FORWARD);
    vessel.CreateThrusterGroup(th_att_lin + 1, 1, THGROUP_ATT_BACK);

    vessel.AddExhaust(th_att_lin[0], 0.6, 0.078, bm::main::RCS_RL_LEFT_loc, _V(0, 0, -1));
    vessel.AddExhaust(th_att_lin[0], 0.6, 0.078, bm::main::RCS_RR_RIGHT_loc, _V(0, 0, -1));
    vessel.AddExhaust(th_att_lin[1], 0.6, 0.078, bm::main::RCS_L_FORWARD_loc, _V(0, 0, 1));
    vessel.AddExhaust(th_att_lin[1], 0.6, 0.078, bm::main::RCS_R_FORWARD_loc, _V(0, 0, 1));
}

inline void PropulsionController::UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power)
{
    auto isPowered = power.GetPowerLevel() > 20.0;

    isExternAvail_ = (isPowered && vessel_.IsStoppedOrDocked());
    lightFuelAvail_.SetState(isExternAvail_);

    if (!isExternAvail_ || !isPowered) { isFilling_ = false; }

    btnFuelValveOpen_.SetState(isFilling_);

    // Determine fuel levels:
    mainFuelRangeValue_ = vessel_.GetPropellantMass(mainPropellant_) / vessel_.GetPropellantMaxMass(mainPropellant_);

    // Main flow
    auto flow = vessel_.GetPropellantFlowrate(mainPropellant_);
    //    auto trFlow = (flow / maxMainFlow_) * (PI2 * 0.75);	// 90.718 = 200lbs per hour : 270 deg.

    fuelFlowRangeValue_ = flow / maxMainFlow_;

    if (isFilling_) // cannot be enabled if external fuel is unavailable.
    {
        // Add to main.
        auto actualFill = FillMainFuel(vessel, SR71R::FUEL_FILL_RATE * simdt);
        if (actualFill <= 0.0)
        {
            isFilling_ = false;
        }
    }

    statusLimiter_.SetState(
        (!isPowered || switchThrustLimit_.IsOn())
        ? SR71::StatusOff
        : SR71::StatusOn
    );

    statusFuel_.SetState(
        (mainFuelRangeValue_ > 0.2) || !isPowered
        ? SR71::StatusOff
        : mainFuelRangeValue_ == 0.0
        ? SR71::StatusError
        : SR71::StatusWarn);
}

inline void PropulsionController::LoadState(const std::string& line)
{
    std::istringstream in(line);
    int power, fillbtn;

    in >> power >> fillbtn;

    switchThrustLimit_.SetState(power != 0);
    btnFuelValveOpen_.SetState(fillbtn != 0);
}

inline std::string PropulsionController::GetState() const
{
    std::ostringstream out;
    out << (switchThrustLimit_.IsOn() ? 1 : 0) << " " << (btnFuelValveOpen_.IsOn() ? 1 : 0);
    return out.str();
}

inline double PropulsionController::FillMainFuel(bco::Vessel& vessel, double amount)
{
    auto current = vessel_.GetPropellantMass(mainPropellant_);
    auto topOff = SR71R::MAX_FUEL - current;

    auto result = min(amount, topOff);
    vessel_.SetPropellantMass(mainPropellant_, (current + result));
    return result;
}