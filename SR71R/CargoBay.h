/*
CargoBayController - SR-71r Orbiter Addon
Copyright(C) 2015  Blake Christensen

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

/**	CargoBayController
	Controls the cargo bay doors.
	Draws from the main power circuit.  The power draw only
	happens when the doors are in motion.  That draw can be fairly high so
	try to operate one or the other at a time.

	To operate the cargo bay:
	- Power the main circuit (external or fuel cell).
	- Turn 'Main' power on (up).
	- Turn 'Cargo bay' (CRGO) power switch (right panel) on (up).
	- To Open switch 'Cargo Bay' switch (left panel) to OPEN.
	- The message board [BAY] light will show orange while the cargo bay doors
	are in motion, and white when fully open.

	Configuration:
	CARGOBAY a b c
	a - 0/1 Power switch off/on.
	b - 0/1 Open close switch closed/open.
	c - 0.0-1.0 current door position.
*/
#pragma once

#include "OrbiterSDK.h"

#include "../bc_orbiter/vessel.h"
#include "../bc_orbiter/AnimatedValue.h"
#include "../bc_orbiter/MeshTools.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "IPowerProvider.h"

namespace bco = bc_orbiter;

class CargoBay
{
public:
    CargoBay() = default;
    ~CargoBay() = default;

    void Setup(bco::Vessel& vessel);

    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power);
    void UpdateVCUI(bco::Vessel& vessel);   // Called to update the VC UI when the VC is active.
    void UpdateRightPanelUI(MESHHANDLE mesh);    // Called to update the 2D panel UI when the panel is active.

    void LoadVC();       // Called when the VC is loaded to setup animations.
    void LoadPanel(bco::Vessel& vessel, PANELHANDLE handle);    // Called when the 2D panel is loaded to setup animations.

    /**
     * \brief Opens the cargo bay doors.
     * \param vessel The vessel to operate on.
     */
    void ToggleOpenClose() {
        isOpenSwitchOn_ = !isOpenSwitchOn_;
    };

    /**
     * \brief Toggles the power.
     * \param vessel The vessel to operate on.
     */
    void TogglePower() {
        isPowerSwitchOn_ = !isPowerSwitchOn_;
    };

    /**
    * \brief Opens the cargo bay.
    */
    void OpenCargoBay() {
        isOpenSwitchOn_ = true;
    };

    /**
    * \brief Closes the cargo bay.
    */
    void CloseCanopy() {
        isOpenSwitchOn_ = false;
    };

    /**
     * \brief Loads the state of the canopy from a string.
     * \param line The string containing the state information.
     */
    void LoadState(const std::string& line);
    /**
     * \brief Gets the current state of the canopy as a string.
     * \return A string representing the current state of the canopy.
     */
    std::string GetState() const;

private:
    bco::AnimatedValue<bco::StateUpdateTarget> animVCPowerSwitch_{ SR71R::ToggleAnimSpeed };
    bco::AnimatedValue<bco::StateUpdateTarget> animVCDoorSwitch_{ SR71R::ToggleAnimSpeed };
    bco::AnimatedValue<bco::StateUpdateTarget> animCargoBay_{ 0.2 };

    UINT aidVCPowerSwitch_{ 0 };  // Animation ID for the power switch.
    UINT aidVCDoorSwitch_{ 0 };  // Animation ID for the door switch.
    UINT aidMainCargoBay_{ 0 };  // Animation ID for the canopy.

    int eventId_Power_{ -1 };
    int eventId_Open_{ -1 };

    bool isPowerSwitchOn_{ false };
    bool isOpenSwitchOn_{ false };
};

inline void CargoBay::Setup(bco::Vessel& vessel)
{
    eventId_Power_ = vessel.GetEventId();
    eventId_Open_ = vessel.GetEventId();

    vessel.RegisterEventHandler(eventId_Power_, [this](int, int) { TogglePower(); return true; });
    vessel.RegisterEventHandler(eventId_Open_,  [this](int, int) { ToggleOpenClose(); return true; });

    // Animations:
    auto vcIndex = vessel.GetMeshIndex(bm::vc::MESH_NAME);
    auto mainIndex = vessel.GetMeshIndex(bm::main::MESH_NAME);

    aidVCPowerSwitch_ = vessel.CreateVesselAnimation();
    aidVCDoorSwitch_ = vessel.CreateVesselAnimation();
    aidMainCargoBay_ = vessel.CreateVesselAnimation();

    vessel.AddAnimationGroup(
        aidVCPowerSwitch_,
        vcIndex,
        { bm::vc::SwCargoPower_id },
        bm::vc::SwCargoPower_loc, bm::vc::PowerTopRightAxis_loc,
        SR71R::ToggleAnimAngle,
        0.0, 1.0);

    vessel.AddAnimationGroup(
        aidVCDoorSwitch_,
        vcIndex,
        { bm::vc::SwCargoOpen_id },
        bm::vc::SwCargoOpen_loc, bm::vc::DoorsRightAxis_loc,
        SR71R::ToggleAnimAngle,
        0.0, 1.0);

    vessel.AddAnimationGroup(
        aidMainCargoBay_,
        mainIndex,
        { bm::main::BayDoorPF_id },
        bm::main::Bay1AxisPA_loc, bm::main::Bay1AxisPF_loc,
        (160 * RAD),
        0.51, 0.74);

    vessel.AddAnimationGroup(
        aidMainCargoBay_,
        mainIndex,
        { bm::main::BayDoorSF_id },
        bm::main::Bay1AxisPA_loc, bm::main::Bay1AxisPF_loc,
        (160 * RAD),
        0.76, 1.0);

    vessel.AddAnimationGroup(
        aidMainCargoBay_,
        mainIndex,
        { bm::main::BayDoorPA_id },
        bm::main::Bay2AxisPA_loc, bm::main::Bay2AxisPF_loc,
        (160 * RAD),
        0.0, 0.24);

    vessel.AddAnimationGroup(
        aidMainCargoBay_,
        mainIndex,
        { bm::main::BayDoorSA_id },
        bm::main::Bay2AxisSF_loc, bm::main::Bay2AxisSA_loc,
        (160 * RAD),
        0.26, 0.49);
}

inline void CargoBay::UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power)
{
    animVCDoorSwitch_.Update(simdt, isOpenSwitchOn_);
    animVCPowerSwitch_.Update(simdt, isPowerSwitchOn_);

    if (isPowerSwitchOn_ && power.GetPowerLevel() > 0.0) {
        animCargoBay_.Update(simdt, isOpenSwitchOn_ ? 1.0 : 0.0);
    }

    vessel.SetAnimation(aidMainCargoBay_, animCargoBay_.GetCurrent());
}

inline void CargoBay::UpdateVCUI(bco::Vessel& vessel)
{
    vessel.SetAnimation(aidVCDoorSwitch_, animVCDoorSwitch_.GetCurrent());
    vessel.SetAnimation(aidVCPowerSwitch_, animVCPowerSwitch_.GetCurrent());
}

inline void CargoBay::UpdateRightPanelUI(MESHHANDLE mesh)
{
    bco::DrawPanelOnOff(mesh, bm::pnlright::pnlPwrCargo_id, bm::pnlright::pnlPwrCargo_vrt, isPowerSwitchOn_, SR71R::TogglePnlOffset);
    bco::DrawPanelOnOff(mesh, bm::pnlright::pnlDoorCargo_id, bm::pnlright::pnlDoorCargo_vrt, isOpenSwitchOn_, SR71R::TogglePnlOffset);
}

inline void CargoBay::LoadVC()
{
    bco::LoadVCSimpleEvent(eventId_Power_, bm::vc::SwCargoPower_loc, SR71R::ToggleHitRadius);
    bco::LoadVCSimpleEvent(eventId_Open_, bm::vc::SwCargoOpen_loc, SR71R::ToggleHitRadius);
}

inline void CargoBay::LoadPanel(bco::Vessel& vessel, PANELHANDLE handle)
{
    bco::LoadPanelSimpleEvent(vessel, eventId_Power_, handle, bm::pnlright::pnlPwrCargo_RC);
    bco::LoadPanelSimpleEvent(vessel, eventId_Open_, handle, bm::pnlright::pnlDoorCargo_RC);
}
