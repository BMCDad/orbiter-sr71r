#pragma once

#include <concepts>

#include "..\bc_orbiter\IVessel.h"
#include "..\bc_orbiter\Tools.h"
#include "..\bc_orbiter\Types.h"

#include "OrbiterAPI.h"

#include "SR71_Common.h"

namespace bco = bc_orbiter;
namespace cmn = sr71_common;
namespace con = bc_orbiter::contracts;
namespace dta = bc_orbiter::data_type;

template <con::Cockpit TVCcp, con::Texture TVCtex, con::Cockpit TPNLcp, con::Texture TPNLtex >
class StatusLight
{
public:
    StatusLight(const TVCcp& tvccp, const TVCtex& tvctex, const TPNLcp& tpnlcp, const TPNLtex& tpnltex) 
      : vcCockpit_(tvccp),
        vcTexture_(tvctex),
        pnlCockpit_(tpnlcp),
        pnlTexture_(tpnltex)
    {}

    auto Register(bco::IVessel& vessel) -> void;

    auto UpdateStatus(bco::IVessel& vessel, double status) -> void {
        if (state_ != status) {
            state_ = status;
            vessel.RequestPanelRedraw(pnlCockpit_.ID, panelRedrawId_);
            vessel.RequestVCRedraw(vcCockpit_.ID, vcRedrawId_);
        }
    }

    auto UpdateStatus(bco::IVessel& vessel, bool status) -> void {
        UpdateStatus(vessel, status ? 1.0 : 0.0);
    }
private:

    void onPanelRedraw(bco::IVessel& v) {
        bco::DrawPanelOffset(meshPanel_, pnlTexture_.GroupID, pnlTexture_.Verts, textureOffset_ * state_);
    }

    void onVCRedraw(bco::IVessel& v) {
        auto vcMesh = v.GetDeviceMesh(0);
        bco::DrawVCOffset(vcMesh, vcTexture_.GroupID, vcTexture_.Verts, state_ * textureOffset_);
    }


    TVCcp   vcCockpit_;
    TVCtex  vcTexture_;
    TPNLcp  pnlCockpit_;
    TPNLtex pnlTexture_;


    UINT            panelRedrawId_{ (UINT)-1};
    UINT            vcRedrawId_{ (UINT)-1};
    double          textureOffset_{ 0.0 };
    double          state_{ 0.0 };
    MESHHANDLE      meshPanel_{ nullptr };
    MESHHANDLE      meshVC_{ nullptr };

};

inline void StatusLight<dta::Cockpit, dta::Texture, dta::Cockpit, dta::Texture>::Register(bco::IVessel& vessel)
{
    panelRedrawId_ = vessel.RegisterForPanelRedraw(pnlCockpit_.ID, [&](bco::IVessel& v) {onPanelRedraw(v); });
    vcRedrawId_ = vessel.RegisterForVCRedraw(vcCockpit_.ID, [&](bco::IVessel& v) {onVCRedraw(v); });

    meshPanel_ = vessel.GetpanelMeshHandle(pnlCockpit_.ID);
//    meshVC_ = vessel.GetMeshHandle(data_.VCMeshName);

    textureOffset_ = bco::UVOffset(pnlTexture_.Verts);
}

using Status = StatusLight<dta::Cockpit, dta::Texture, dta::Cockpit, dta::Texture>;
