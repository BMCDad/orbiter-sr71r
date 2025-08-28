/*
Airspeed - SR-71r Orbiter Addon
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

#include <cmath>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"

#include "IAvionics.h"

namespace bco = bc_orbiter;

class Airspeed
{
public:
    Airspeed() = default;
    ~Airspeed() = default;
    void Setup(bco::Vessel& vessel);
    
    void UpdateState(bco::Vessel& vessel, double simdt, IAvionics& avionics);
    void UpdateVCUI(bco::Vessel& vessel, MESHHANDLE mesh);   // Called to update the VC UI when the VC is active.
    void UpdateMainPanelUI(MESHHANDLE mesh);    // Called to update the 2D panel UI when the panel is active.
    
    void LoadState(const std::string& line);
    std::string GetState() const;

private:

    const double MIN_PIN = 0.0;
    const double MAX_PIN_RAD = 5.236; // 300.0 deg
    double l22 = log(23);
    const double RollOffset = 0.1084;		// flat_roll offset.

    const double ANGLE = -(300.0 * RAD); // 300 degrees in radians.
    bco::AnimatedValue<bco::StateUpdateTarget>  animMachHand_       { 2.0 };
    bco::AnimatedValue<bco::StateUpdateTarget>  animKiesHand_       { 2.0 };
    bco::AnimatedValue<bco::StateUpdateTarget>  animMaxMachHand_    { 2.0 };

    bco::AnimatedValue<bco::StateUpdateWrap>    animTDIKeasOne_     { 1.0 };
    bco::AnimatedValue<bco::StateUpdateWrap>    animTDIKeasTen_     { 1.0 };
    bco::AnimatedValue<bco::StateUpdateWrap>    animTDIKeasHundred_ { 1.0 };

    bco::AnimatedValue<bco::StateUpdateWrap>    animMACHOne_        { 1.0 };
    bco::AnimatedValue<bco::StateUpdateWrap>    animMACHTens_       { 1.0 };
    bco::AnimatedValue<bco::StateUpdateWrap>    animMACHHundred_    { 1.0 };

    UINT aidVCMachHand_     { 0 };
    UINT aidVCKiesHand_     { 0 };
    UINT aidVCMaxMachHand_  { 0 };
};

inline void Airspeed::Setup(bco::Vessel& vessel)
{
    // VC Animations:
    auto vcIndex = vessel.GetMeshIndex(bm::vc::MESH_NAME);

    aidVCMachHand_ = vessel.CreateVesselAnimation();
    aidVCKiesHand_ = vessel.CreateVesselAnimation();
    aidVCMachHand_ = vessel.CreateVesselAnimation();

    vessel.AddAnimationGroup(
        aidVCMachHand_,
        vcIndex,
        { bm::vc::vcMachHand_id },
        bm::vc::vcMachHand_loc, bm::vc::SpeedAxis_loc, 
        (300 * RAD), 
        0, 1);

    vessel.AddAnimationGroup(
        aidVCKiesHand_,
        vcIndex,
        { bm::vc::vcKiesHand_id },
        bm::vc::vcKiesHand_loc, bm::vc::SpeedAxis_loc,
        (300 * RAD),
        0, 1);

    vessel.AddAnimationGroup(
        aidVCMaxMachHand_,
        vcIndex,
        { bm::vc::vcMachMaxHand_id },
        bm::vc::vcMachMaxHand_loc, bm::vc::SpeedAxis_loc,
        (300 * RAD),
        0, 1);
}

inline void Airspeed::UpdateState(bco::Vessel& vessel, double simdt, IAvionics& avionics)
{
    double  keas            = 0.0;  // equivalent airspeed, shows in TDI
    double  kias            = 0.0;  // indicated, shows as dial.
    double  mach            = 0.0;  // shows in TDI and as a dial
    double  maxMach         = 0.0;  // Error bar on dial
    double  speedRatio      = 0.0;  // 
    double  maxMachRatio    = 0.0;
    double  kiasSpeed       = 0.0;
    bool	isOverSpeed        = false;

    if (avionics.IsAeroActive()) {
        keas = vessel.GetKeas();
        kias = vessel.GetKias();
        mach = vessel.GetMachNumber();
        auto atmDens = vessel.GetAtmDensity();

        if (atmDens > 0.0)
        {
            maxMach = sqrt(SR71R::MaxPress / atmDens) / 331.34;
            isOverSpeed = mach > maxMach;
        }

        auto machGauge = (mach > SR71R::MAX_MACH) ? SR71R::MAX_MACH : mach;		// machGauge will be pinned, mach itself will be used to set the dials, so it not pinned.

        if (!avionics.IsAeroAtmoMode())  // if exo mode, use velocity for machGauge
        {
            machGauge = vessel.GetAirspeed() / 100;
            maxMach = 22.0;
        }

        // Set WARNING MAX MACH gauge : MACH scale is log base 22 (max mach):
        // LOG range 1 to 23 (22 positions) to avoid <1 values.

        // Calculate the max mach needle.
        if (maxMach > 22.0) maxMach = 22.0;		// Pin MAX to 22 and 1
        if (maxMach < 0.0) maxMach = 0.0;
        maxMachRatio = (maxMach == 0.0)
            ? 0.0
            : ((log(maxMach + 1) / l22) * MAX_PIN_RAD) / MAX_PIN_RAD;	// Determine LOG based on speed then convert to 0-1 ration for the gauge.

        // Kies dial
        speedRatio = ((log(machGauge + 1.0) / l22) * MAX_PIN_RAD) / MAX_PIN_RAD; // FIX << RATIO does not need MAX_PIN_RAD
        auto kRatio = kias / 600; // 600 max kias speed
        kiasSpeed = (avionics.IsAeroAtmoMode()) ? (speedRatio - kRatio) : speedRatio;	// If atmo, subtract the speedRatio to get correct rotation.
    }

    animMachHand_.Update(simdt, speedRatio);
    animKiesHand_.Update(simdt, speedRatio - (kias / 600));
    animMaxMachHand_.Update(simdt, maxMachRatio);

    bco::TensParts parts;
    bco::GetDigits(keas, parts);
    animTDIKeasOne_.Update(simdt, parts.Tens);
    animTDIKeasTen_.Update(simdt, parts.Hundreds);
    animTDIKeasHundred_.Update(simdt, parts.Thousands);

    animMaxMachHand_.Update(simdt, maxMachRatio);
    animMachHand_.Update(simdt, speedRatio);
    animKiesHand_.Update(simdt, speedRatio - (kias / 600));

    bco::GetDigits(mach, parts);
    animMACHOne_.Update(simdt, parts.Tenths);
    animMACHTens_.Update(simdt, parts.Tens);
    animMACHHundred_.Update(simdt, parts.Hundreds);

    //status_.set_state(isOverSpeed ? bco::status_display::status::error : bco::status_display::status::off);
    //enabledFlag_.set_state(avionics_.IsAeroActive());
    //velocityFlag_.set_state(
    //    avionics_.IsAeroActive()
    //    ? avionics_.IsAeroAtmoMode()
    //    : true);
}

inline void Airspeed::UpdateVCUI(bco::Vessel& vessel, MESHHANDLE mesh)
{
    vessel.SetAnimation(aidVCMachHand_, animMachHand_.GetCurrent());
    vessel.SetAnimation(aidVCKiesHand_, animKiesHand_.GetCurrent());
    vessel.SetAnimation(aidVCMaxMachHand_, animMaxMachHand_.GetCurrent());

    bco::TranslateUVQuad(mesh, bm::vc::vcTDIKeasOnes_id,    bm::vc::vcTDIKeasOnes_vrt,  0.0, animTDIKeasOne_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::vc::vcTDIKeasTens_id,    bm::vc::vcTDIKeasTens_vrt,  0.0, animTDIKeasTen_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::vc::vcTDIKeasHunds_id,   bm::vc::vcTDIKeasHunds_vrt, 0.0, animTDIKeasHundred_.GetCurrent() * RollOffset);

    bco::TranslateUVQuad(mesh, bm::vc::vcTDIMachOne_id,     bm::vc::vcTDIMachOne_vrt,   0.0, animMACHOne_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::vc::vcTDIMachTens_id,    bm::vc::vcTDIMachTens_vrt,  0.0, animMACHTens_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::vc::vcTDIMachHunds_id,   bm::vc::vcTDIMachHunds_vrt, 0.0, animMACHHundred_.GetCurrent() * RollOffset);
}

inline void Airspeed::UpdateMainPanelUI(MESHHANDLE mesh)
{
    bco::RotateMesh(mesh, bm::pnl::pnlMachHand_id, bm::pnl::pnlMachHand_vrt, animMachHand_.GetCurrent() * ANGLE);
    bco::RotateMesh(mesh, bm::pnl::pnlKiesHand_id, bm::pnl::pnlKiesHand_vrt, animKiesHand_.GetCurrent() * ANGLE);
    bco::RotateMesh(mesh, bm::pnl::pnlMachMaxHand_id, bm::pnl::pnlMachMaxHand_vrt, animMaxMachHand_.GetCurrent() * ANGLE);

    bco::TranslateUVQuad(mesh, bm::pnl::pnlTDIKEASOnes_id,  bm::pnl::pnlTDIKEASOnes_vrt,    0.0, animTDIKeasOne_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::pnl::pnlTDIKEASTens_id,  bm::pnl::pnlTDIKEASTens_vrt,    0.0, animTDIKeasTen_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::pnl::pnlTDIKEASHunds_id, bm::pnl::pnlTDIKEASHunds_vrt,   0.0, animTDIKeasHundred_.GetCurrent() * RollOffset);

    bco::TranslateUVQuad(mesh, bm::pnl::pnlTDIMACHOne_id,   bm::pnl::pnlTDIMACHOne_vrt, 0.0,    animMACHOne_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::pnl::pnlTDIMACHTens_id,  bm::pnl::pnlTDIMACHTens_vrt, 0.0,   animMACHTens_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::pnl::pnlTDIMACHHunds_id, bm::pnl::pnlTDIMACHHunds_vrt, 0.0,  animMACHHundred_.GetCurrent() * RollOffset);
}
