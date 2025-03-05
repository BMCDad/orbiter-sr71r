#pragma once

#include <memory>
#include <functional>

#include "OrbiterAPI.h"
#include "..\bc_orbiter\vessel.h"

namespace bco = bc_orbiter;

namespace sr71
{
    class IndicatorLight
    {
    public:
        IndicatorLight(
            UINT            vcGroupId,
            const NTVERTEX* vcVerts,
            UINT            pnlGroupId,
            const NTVERTEX* pnlVerts,
            int             panelId)
          : vcGroupId_(vcGroupId),
            vcVerts_(vcVerts),
            pnlGroupId_(pnlGroupId),
            pnlVerts_(pnlVerts),
            panelId_(panelId)
        { }

        void Register(bc_orbiter::Vessel& vessel) {
            vcRedrawId_ = vessel.RegisterVCRedrawEvent([this](bco::Vessel& v, int id, int event, SURFHANDLE surf, DEVMESHHANDLE devMesh) { this->vcRedraw(v, id, event, surf, devMesh); });
            panelRedrawId_ = vessel.RegisterPanelRedrawEvent([this](bco::Vessel& vsl, int id, int event, SURFHANDLE surf) {this->panelRedraw(vsl, id, event, surf); });
        }

        void SetStatus(bco::Vessel& vessel, double status) {
            state_ = status;
            vessel.TriggerPanelRedrawArea(panelId_, panelRedrawId_);
            vessel.TriggerRedrawArea(0, 0, vcRedrawId_);
        }

    private:

        void panelRedraw(bc_orbiter::Vessel& v, int id, int event, SURFHANDLE surf) {
            auto ofs = bco::UVOffset(pnlVerts_);
            auto mesh = v.GetpanelMeshHandle(panelId_);
            bco::DrawPanelOffset(mesh, pnlGroupId_, pnlVerts_, ofs * state_);
        }

        void vcRedraw(bc_orbiter::Vessel& v, int id, int event, SURFHANDLE surf, DEVMESHHANDLE devMesh) {
            auto ofs = bco::UVOffset(vcVerts_);
            bco::DrawVCOffset(devMesh, vcGroupId_, vcVerts_, ofs * state_);
        }


        UINT            vcGroupId_;
        const NTVERTEX* vcVerts_;
        UINT            pnlGroupId_;
        const NTVERTEX* pnlVerts_;
        int             panelId_;

        double          state_{ 0.0 };

        UINT            panelRedrawId_{ 0 };
        UINT            vcRedrawId_{ 0 };
    };
}