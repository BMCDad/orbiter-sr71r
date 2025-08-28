/*
Canopy - SR-71r Orbiter Addon
Copyright(C) 2025  Blake Christensen

Manages the canopy of the SR-71r.  It can be opened and closed provided there is
electrical power.

VC: Inside and outside window mesh groups.

External:
    Windows and main canopy mesh groups.

Events:
    Canopy Power Toggle:  Toggles the power to the canopy.  If power is on, the canopy can be opened or closed.
    Canopy Open:  Opens the canopy.

-----

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
#include <string>
#include <sstream>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"
#include "IPowerProvider.h"

namespace bco = bc_orbiter;

class Canopy
{
    public:
    Canopy() = default;
    ~Canopy() = default;

    void Setup(bco::Vessel& vessel);

    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power);
    void UpdateVCUI(bco::Vessel& vessel);   // Called to update the VC UI when the VC is active.
    void UpdateRightPanelUI(MESHHANDLE mesh);    // Called to update the 2D panel UI when the panel is active.

    void LoadVC();       // Called when the VC is loaded to setup animations.
    void LoadPanel(bco::Vessel& vessel, PANELHANDLE handle);    // Called when the 2D panel is loaded to setup animations.

    /**
     * \brief Opens the canopy.
     * \param vessel The vessel to operate on.
     */
    void ToggleOpenClose() { 
        isOpenSwitchOn_ = !isOpenSwitchOn_; 
        isOpenSwitchChanged_ = true; // Mark that the open/close switch has changed state.
    };

    /**
     * \brief Toggles the canopy power.
     * \param vessel The vessel to operate on.
     */
    void TogglePower() { 
        isPowerSwitchOn_ = !isPowerSwitchOn_; 
        isPowerSwitchChanged_ = true; // Mark that the power switch has changed state.
    };

    /**
    * \brief Opens the canopy.
    */
    void OpenCanopy() {
        isOpenSwitchOn_ = true; 
        isOpenSwitchChanged_ = true; // Mark that the open/close switch has changed state.
    };

    /**
    * \brief Closes the canopy.
    */
    void CloseCanopy() {
         isOpenSwitchOn_ = false; 
         isOpenSwitchChanged_ = true; // Mark that the open/close switch has changed state.
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
    bco::AnimatedValue<bco::StateUpdateTarget> animVCPowerSwitch_   { SR71R::ToggleAnimSpeed };
    bco::AnimatedValue<bco::StateUpdateTarget> animVCDoorSwitch_    { SR71R::ToggleAnimSpeed };
    bco::AnimatedValue<bco::StateUpdateTarget> animCanopy_          { 0.2 };

    UINT aidVCPowerSwitch_  { 0 };  // Animation ID for the power switch.
    UINT aidVCDoorSwitch_   { 0 };  // Animation ID for the door switch.
    UINT aidMainCanopy_     { 0 };  // Animation ID for the canopy.
    UINT aidVCCanopy_       { 0 };  // Animation ID for the VC canopy.

    int eventId_Power_{ -1 };
    int eventId_Open_{ -1 };

    bool isPowerSwitchOn_       { false };
    bool isPowerSwitchChanged_  { false }; // Indicates if the power switch has changed state.

    bool isOpenSwitchOn_        { false };
    bool isOpenSwitchChanged_   { false }; // Indicates if the open/close switch has changed state.
};

inline void Canopy::Setup(bco::Vessel& vessel)
{
    eventId_Open_ = vessel.GetEventId();
    eventId_Open_ = vessel.GetEventId();

    vessel.RegisterEventHandler(eventId_Power_, [this](int, int) { TogglePower(); return true; });
    vessel.RegisterEventHandler(eventId_Open_,  [this](int, int) { ToggleOpenClose(); return true; });

    // Animations:
    auto vcIndex = vessel.GetMeshIndex(bm::vc::MESH_NAME);
    auto mainIndex = vessel.GetMeshIndex(bm::main::MESH_NAME);

    // VC Power switch
    aidVCPowerSwitch_ = vessel.CreateVesselAnimation();
    vessel.AddAnimationGroup(
        aidVCPowerSwitch_,
        vcIndex,
        { bm::vc::SwCanopyPower_id },
        bm::vc::SwCanopyPower_loc, bm::vc::PowerTopRightAxis_loc,
        SR71R::ToggleAnimAngle,
        0.0, 1.0);

    // VC Door switch
    aidVCDoorSwitch_ = vessel.CreateVesselAnimation();
    vessel.AddAnimationGroup(
        aidVCDoorSwitch_,
        vcIndex,
        { bm::vc::SwCanopyOpen_id },
        bm::vc::SwCanopyOpen_loc, bm::vc::DoorsRightAxis_loc,
        SR71R::ToggleAnimAngle,
        0.0, 1.0);

    // Main Canopy animation
    aidMainCanopy_ = vessel.CreateVesselAnimation();
    vessel.AddAnimationGroup(
        aidMainCanopy_,
        mainIndex,
        { bm::main::Canopy_id,
          bm::main::CanopyWindowInside_id,
          bm::main::CanopyWindowOutside_id },
        bm::main::CockpitAxisS_loc, bm::main::CockpitAxisP_loc,
        (55 * RAD), 0, 1);

    // VC Canopy animation
    aidVCCanopy_ = vessel.CreateVesselAnimation();
    vessel.AddAnimationGroup(
        aidVCCanopy_,
        vcIndex,
        { bm::vc::CanopyFI_id,
          bm::vc::CanopyFO_id,
          bm::vc::CanopyWindowInside_id,
          bm::vc::CanopyWindowSI_id },
        bm::main::CockpitAxisS_loc, bm::main::CockpitAxisP_loc,
        (55 * RAD), 0, 1);
}

inline void Canopy::UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power)
{
    auto isCanopyMoving = false;

    // Switches always get a time step.
    animVCDoorSwitch_.Update(simdt,   isOpenSwitchOn_ ? 1.0 : 0.0);
    animVCPowerSwitch_.Update(simdt,  isPowerSwitchOn_ ? 1.0 : 0.0);

    if (isPowerSwitchOn_ && power.GetPowerLevel() > 0.0) {
        isCanopyMoving = animCanopy_.Update(simdt, isOpenSwitchOn_ ? 1.0 : 0.0);
    }

    vessel.SetAnimation(aidMainCanopy_, animCanopy_.GetCurrent()); // Always step the main canopy animation.

    if (oapiCockpitMode() == COCKPIT_PANELS) {
        auto mesh = vessel.GetPanelMeshHandle(SR71R::RightPanel_ID);
        if (isPowerSwitchChanged_) {
            isPowerSwitchChanged_ = false; // Reset the change flag after processing.
            bco::DrawPanelOnOff(mesh, bm::pnlright::pnlPwrCanopy_id, bm::pnlright::pnlPwrCanopy_vrt, isPowerSwitchOn_, SR71R::TogglePnlOffset);
        }

        if (isOpenSwitchChanged_) {
            isOpenSwitchChanged_ = false; // Reset the change flag after processing.
            bco::DrawPanelOnOff(mesh, bm::pnlright::pnlDoorCanopy_id, bm::pnlright::pnlDoorCanopy_vrt, isOpenSwitchOn_, SR71R::TogglePnlOffset);
        }
    }
}

inline void Canopy::UpdateVCUI(bco::Vessel& vessel)
{
    vessel.SetAnimation(aidVCCanopy_,       animCanopy_.GetCurrent());
    vessel.SetAnimation(aidVCDoorSwitch_,   animVCDoorSwitch_.GetCurrent());
    vessel.SetAnimation(aidVCPowerSwitch_,  animVCPowerSwitch_.GetCurrent());
}

inline void Canopy::UpdateRightPanelUI(MESHHANDLE mesh)
{
    bco::DrawPanelOnOff(mesh, bm::pnlright::pnlPwrCanopy_id,    bm::pnlright::pnlPwrCanopy_vrt,     isPowerSwitchOn_,   SR71R::TogglePnlOffset);
    bco::DrawPanelOnOff(mesh, bm::pnlright::pnlDoorCanopy_id,   bm::pnlright::pnlDoorCanopy_vrt,    isOpenSwitchOn_,    SR71R::TogglePnlOffset);
}

inline void Canopy::LoadVC()
{
    bco::LoadVCSimpleEvent(eventId_Power_, bm::vc::SwCanopyPower_loc, SR71R::ToggleHitRadius);
    bco::LoadVCSimpleEvent(eventId_Open_, bm::vc::SwCanopyOpen_loc, SR71R::ToggleHitRadius);
}

inline void Canopy::LoadPanel(bco::Vessel& vessel, PANELHANDLE handle)
{
    bco::LoadPanelSimpleEvent(vessel, eventId_Power_, handle, bm::pnlright::pnlPwrCanopy_RC);
    bco::LoadPanelSimpleEvent(vessel, eventId_Open_, handle, bm::pnlright::pnlDoorCanopy_RC);
}
