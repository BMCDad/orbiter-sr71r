/*
Canopy - SR-71r Orbiter Addon
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

Canopy
Manages the canopy of the SR-71r.  It can be opened and closed provided there is
electrical power.

VC: Inside and outside window mesh groups.

External:
    Windows and main canopy mesh groups.

Events:
    Canopy Power Toggle:  Toggles the power to the canopy.  If power is on, the canopy can be opened or closed.
    Canopy Open:  Opens the canopy.
*/

#pragma once
#include <string>
#include <sstream>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"
#include "..\bc_orbiter\IUIControl.h"

#include "SR71r_mesh.h"
#include "SR71r2DRight_mesh.h"
#include "SR71Light.h"

#include "ShipMets.h"
#include "IPowerProvider.h"
#include "SR71Toggle.h"

namespace bco = bc_orbiter;

class Canopy
{
public:
    Canopy(bco::Vessel& vessel);
    ~Canopy() = default;

    void Setup(bco::Vessel& vessel);

    void UpdateState(bco::Vessel& vessel, double simdt, IPowerProvider& power);
    void UpdateVCUI(bco::Vessel& vessel);   // Called to update the VC UI when the VC is active.

    /**
     * \brief Opens the canopy.
     * \param vessel The vessel to operate on.
     */
    void ToggleOpenClose() { togDoor_.ToggleState(); };

    /**
     * \brief Toggles the canopy power.
     * \param vessel The vessel to operate on.
     */
    void TogglePower() { togPower_.ToggleState(); };

    /**
    * \brief Opens the canopy.
    */
    void OpenCanopy() { togDoor_.SetState(true); };

    /**
    * \brief Closes the canopy.
    */
    void CloseCanopy() { togDoor_.SetState(false); };

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
    bco::AnimatedValue<bco::StateUpdateTarget> animCanopy_          { 0.2 };

    UINT aidMainCanopy_     { 0 };  // Animation ID for the canopy.
    UINT aidVCCanopy_       { 0 };  // Animation ID for the VC canopy.

    SR71::Toggle togPower_{
        { bm::vc::SwCanopyPower_id },
        bm::vc::SwCanopyPower_loc, bm::vc::PowerTopRightAxis_loc,
        bm::pnlright::pnlPwrCanopy_id,
        bm::pnlright::pnlPwrCanopy_vrt,
        bm::pnlright::pnlPwrCanopy_RC,
        SR71R::RightPanel_ID
    };

    SR71::Toggle togDoor_{
        { bm::vc::SwCanopyOpen_id },
        bm::vc::SwCanopyOpen_loc, bm::vc::DoorsRightAxis_loc,
        bm::pnlright::pnlDoorCanopy_id,
        bm::pnlright::pnlDoorCanopy_vrt,
        bm::pnlright::pnlDoorCanopy_RC,
        SR71R::RightPanel_ID
    };

    SR71::Light status_{
        bm::vc::MsgLightCanopy_id,
        bm::vc::MsgLightCanopy_vrt,
        bm::pnl::pnlMsgLightCanopy_id,
        bm::pnl::pnlMsgLightCanopy_vrt,
        SR71R::MainPanel_ID
    };
};

inline Canopy::Canopy(bco::Vessel& vessel)
{
    vessel.RegisterUIControl(togPower_);
    vessel.RegisterUIControl(togDoor_);
    vessel.RegisterUIControl(status_);
}

inline void Canopy::Setup(bco::Vessel& vessel)
{
    // Animations:
    auto vcIndex = vessel.GetMeshIndex(bm::vc::MESH_NAME);
    auto mainIndex = vessel.GetMeshIndex(bm::main::MESH_NAME);

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
    auto status = SR71::StatusOff;

    if (togPower_.IsOn() && power.GetPowerLevel() > 20.0) {
        isCanopyMoving = animCanopy_.Update(simdt, togDoor_.IsOn() ? 1.0 : 0.0);

        if (isCanopyMoving) {
            status = SR71::StatusWarn;
        }
        else {
            status = animCanopy_.GetCurrent() == 1.0 ? SR71::StatusOn : SR71::StatusOff;
        }
    }

    status_.SetState(status);
    power.DrawPower(isCanopyMoving ? SR71R::CANOPY_AMPS : 0.0); // Draw 20 amps if the canopy is moving.
    vessel.SetAnimation(aidMainCanopy_, animCanopy_.GetCurrent()); // Always step the main canopy animation.
}

inline void Canopy::UpdateVCUI(bco::Vessel& vessel)
{
    vessel.SetAnimation(aidVCCanopy_,       animCanopy_.GetCurrent());
}

inline void Canopy::LoadState(const std::string& line)
{
    std::istringstream is(line);
    int power, door;
    double position;
    is >> power >> door >> position;
    togPower_.SetState(power != 0);
    togDoor_.SetState(door != 0);
    animCanopy_.LoadState(position, togDoor_.IsOn() ? 1.0 : 0.0);
}

inline std::string Canopy::GetState() const
{
    std::ostringstream os;
    os << (togPower_.IsOn() ? 1 : 0)
        << " " << (togDoor_.IsOn() ? 1 : 0)
        << " " << bco::FormatDouble(animCanopy_.GetCurrent());
    return os.str();
}
