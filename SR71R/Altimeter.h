/*
Altimeter - SR-71r Orbiter Addon
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

class Altimeter
{

public:

    Altimeter() = default;
    ~Altimeter() = default;
    void Setup(bco::Vessel& vessel);

    void UpdateState(bco::Vessel& vessel, double simdt, IAvionics& avionics);
    void UpdateVCUI(bco::Vessel& vessel, MESHHANDLE mesh);   // Called to update the VC UI when the VC is active.
    void UpdateMainPanelUI(MESHHANDLE mesh);    // Called to update the 2D panel UI when the panel is active.

    void LoadState(const std::string& line);
    std::string GetState() const;

private:

    const double RollOffset = 0.1084;		// flat_roll offset.

    bco::AnimatedValue<bco::StateUpdateWrap>    animAltOnes_{ 2.0 };
    bco::AnimatedValue<bco::StateUpdateWrap>    animAltTens_{ 2.0 };
    bco::AnimatedValue<bco::StateUpdateWrap>    animAltHunds_{ 2.0 };

    UINT aidVCAltOnes   { 0 };
    UINT aidVCAltTens   { 0 };
    UINT aidVCAltHunds  { 0 };

    bco::AnimatedValue<bco::StateUpdateWrap>    animTDIOnes_{ 1.0 };
    bco::AnimatedValue<bco::StateUpdateWrap>    animTDITens_{ 1.0 };
    bco::AnimatedValue<bco::StateUpdateWrap>    animTDIHunds_{ 1.0 };
    bco::AnimatedValue<bco::StateUpdateWrap>    animTDIThous_{ 1.0 };
    bco::AnimatedValue<bco::StateUpdateWrap>    animTDITenThous_{ 1.0 };

    //bco::on_off_display		enabledFlag_{
    //   bm::vc::AltimeterOffFlag_id,
    //      bm::vc::AltimeterOffFlag_vrt,
    //      bm::pnl::pnlAltimeterOffFlag_id,
    //      bm::pnl::pnlAltimeterOffFlag_vrt,
    //      0.0244
    //};

    //bco::on_off_display		altimeterExoModeFlag_{
    //   bm::vc::AltimeterGround_id,
    //      bm::vc::AltimeterGround_vrt,
    //      bm::pnl::pnlAltimeterGround_id,
    //      bm::pnl::pnlAltimeterGround_vrt,
    //      0.0244
    //};
};

inline void Altimeter::Setup(bco::Vessel& vessel)
{
    auto vcIndex = vessel.GetMeshIndex(bm::vc::MESH_NAME);

    aidVCAltOnes = vessel.CreateVesselAnimation();
    aidVCAltTens = vessel.CreateVesselAnimation();
    aidVCAltHunds = vessel.CreateVesselAnimation();

    vessel.AddAnimationGroup(
        aidVCAltOnes,
        vcIndex,
        { bm::vc::vcAlt1Hand_id },
        bm::vc::vcAlt1Hand_loc, bm::vc::AltimeterAxis_loc,
        (360 * RAD),
        0, 1);

    vessel.AddAnimationGroup(
        aidVCAltTens,
        vcIndex,
        { bm::vc::vcAlt10Hand_id },
        bm::vc::vcAlt10Hand_loc, bm::vc::AltimeterAxis_loc,
        (360 * RAD),
        0, 1);

    vessel.AddAnimationGroup(
        aidVCAltHunds,
        vcIndex,
        { bm::vc::vcAlt100Hand_id },
        bm::vc::vcAlt100Hand_loc, bm::vc::AltimeterAxis_loc,
        (360 * RAD),
        0, 1);
}

inline void Altimeter::UpdateState(bco::Vessel& vessel, double simdt, IAvionics& avionics)
{
    double altFeet = 0.0;
    if (avionics.IsAeroActive()) {
        auto altMode = avionics.IsAeroAtmoMode() ? AltitudeMode::ALTMODE_GROUND : AltitudeMode::ALTMODE_MEANRAD;
        int res = 0;
        auto alt = vessel.GetAltitude(altMode, &res);
        altFeet = alt * 3.28084;
    }

    bco::IntParts iOut;
    if (altFeet < 100000) {
        bco::BreakTens((altFeet > 100000 ? 0.0 : altFeet), iOut);
        animAltOnes_.Update(simdt, iOut.Hundreds / 10.0);
        animAltTens_.Update(simdt, iOut.Thousands / 10.0);
        animAltHunds_.Update(simdt, iOut.TenThousands / 10.0);
    }
    else {
        animAltOnes_.Update(simdt, 0.0);
        animAltTens_.Update(simdt, 0.0);
        animAltHunds_.Update(simdt, 0.0);
    }


    // ** ALTIMETER **
    bco::TensParts parts;
    bco::GetDigits((altFeet > 1000000) ? 0 : altFeet, parts);  // Limit to 100000

    //sprintf(oapiDebugString(), "Alt: : %+2i : %+2i : %+2i", altFeet, parts.Tens, parts.Hundreds);

    animTDIOnes_.Update(simdt, parts.Hundreds);
    animTDITens_.Update(simdt, parts.Thousands);
    animTDIHunds_.Update(simdt, parts.TenThousands);
    animTDIThous_.Update(simdt, parts.HundredThousands);
    animTDITenThous_.Update(simdt, parts.Millions);

    //altimeterExoModeFlag_.set_state(		// FALSE show flag, TRUE hide flag
    //    !avionics_.IsAeroActive()			// So if altimeter is OFF set TRUE so the flag does NOT show.
    //    ? true
    //    : avionics_.IsAeroAtmoMode());		// True for this switch means ATMO mode.

    //enabledFlag_.set_state(		// FALSE show flag, TRUE hide flag
    //    !avionics_.IsAeroActive()
    //    ? false								// Avionics off, so show flag.
    //    : ((altFeet > 100000) &&			// If enabled, then SHOW if > 100000 ft
    //        avionics_.IsAeroAtmoMode()		// AND we are in atmo mode.
    //        ? false
    //        : true));
}

inline void Altimeter::UpdateVCUI(bco::Vessel& vessel, MESHHANDLE mesh)
{
    vessel.SetAnimation(aidVCAltOnes, animAltOnes_.GetCurrent());
    vessel.SetAnimation(aidVCAltTens, animAltTens_.GetCurrent());
    vessel.SetAnimation(aidVCAltHunds, animAltHunds_.GetCurrent());

    bco::TranslateUVQuad(mesh, bm::vc::vcTDIAltOnes_id, bm::vc::vcTDIAltOnes_vrt, 0.0, animTDIOnes_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::vc::vcTDIAltTens_id, bm::vc::vcTDIAltTens_vrt, 0.0, animTDITens_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::vc::vcTDIAltHunds_id, bm::vc::vcTDIAltHunds_vrt, 0.0, animTDIHunds_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::vc::vcTDIAltThous_id, bm::vc::vcTDIAltThous_vrt, 0.0, animTDIThous_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::vc::vcTDIAltTenThous_id, bm::vc::vcTDIAltTenThous_vrt, 0.0, animTDITenThous_.GetCurrent() * RollOffset);
}

inline void Altimeter::UpdateMainPanelUI(MESHHANDLE mesh)
{
    bco::RotateMesh(mesh, bm::pnl::pnlAlt1Hand_id,      bm::pnl::pnlAlt1Hand_vrt,   animAltOnes_.GetCurrent() * -PI2);
    bco::RotateMesh(mesh, bm::pnl::pnlAlt10Hand_id,     bm::pnl::pnlAlt10Hand_vrt,  animAltTens_.GetCurrent() * -PI2);
    bco::RotateMesh(mesh, bm::pnl::pnlAlt100Hand_id,    bm::pnl::pnlAlt100Hand_vrt, animAltHunds_.GetCurrent() * -PI2);

    bco::TranslateUVQuad(mesh, bm::pnl::pnlTDIAltOnes_id,       bm::pnl::pnlTDIAltOnes_vrt, 0.0,    animTDIOnes_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::pnl::pnlTDIAltTens_id,       bm::pnl::pnlTDIAltTens_vrt, 0.0,    animTDITens_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::pnl::pnlTDIAltHund_id,       bm::pnl::pnlTDIAltHund_vrt, 0.0,    animTDIHunds_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::pnl::pnlTDIAltThous_id,      bm::pnl::pnlTDIAltThous_vrt, 0.0,   animTDIThous_.GetCurrent() * RollOffset);
    bco::TranslateUVQuad(mesh, bm::pnl::pnlTDIAltTenThou_id,    bm::pnl::pnlTDIAltTenThou_vrt, 0.0, animTDITenThous_.GetCurrent() * RollOffset);
}
