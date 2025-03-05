//	Component Common - SR-71r Orbiter Addon
//	Copyright(C) 2025  Blake Christensen
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
#include "OrbiterAPI.h"
#include "..\bc_orbiter\OnOffInput.h"



namespace bc_orbiter
{
    class Vessel;
}
namespace sr71_common
{
    namespace status
    {
        constexpr double on = 0.0;
        constexpr double off = 1.0;
        constexpr double error = 2.0;
        constexpr double warn = 3.0;
    }

    namespace panel
    {
        constexpr int main = 0;
        constexpr int right = 1;
    }

    namespace vc
    {
        constexpr int main = 0;
    }

    /* Control data for on/off up/down physical toggle switches */
    const bc_orbiter::OnOffInputMeta toggleOnOff{
           1.5708,              // Rotation angle (RAD)
          10.0,                 // Anim speed
           0.0,                 // anim start
           1.0,                 // anim end
           0.01,                // VC hit radius
           0.0148,              // Panel offset
          PANEL_REDRAW_NEVER,   // VCRedrawFlags
          PANEL_MOUSE_LBDOWN,   // vcMouseFlag
          PANEL_REDRAW_MOUSE,   // panel redraw flag
          PANEL_MOUSE_LBDOWN    // panel mouse flag
    };

//    class Toggle
//    {
//    public:
//        Toggle() {}
//
//        void SetupAnimation(bc_orbiter::Vessel& vessel, UINT grp, const VECTOR3& locA, const VECTOR3& locB) {
//            // Animation
//            auto vcIdx = vessel.GetVCMeshIndex();
//
//            group_ = grp;
//
//            VECTOR3 axis = locB - locA;
//            normalise(axis);
//            transform_ = std::make_unique<MGROUP_ROTATE>(vcIdx, &group_, 1, locA, axis, (float)toggleOnOff.animRotation);
//
//            animId_ = vessel.CreateVesselAnimation(anim_);
//            anim_.VesselId(animId_);
//            vessel.AddAnimationComponent(animId_, 0, 1, transform_.get());
//        }
//
//        void Register(bc_orbiter::Vessel& vessel,
//            const VECTOR3& vcLoc,
//            const RECT& pnlLoc,
//            std::function<void(bc_orbiter::Vessel& vessel)> func,
//            const RECT& rcPnlRedraw,
////            std::function<void(bc_orbiter::Vessel& vessel)> funcRedraw
//            int panelId,
//            const NTVERTEX& pnlVerts,
//            UINT panelGroupId
//        ) {
//
//            bc_orbiter::funcVCMouseEvent fvm = [func](bc_orbiter::Vessel& vsl, int id, int event, VECTOR3& location) { func(vsl); };
//            bc_orbiter::funcPanelMouseEvent fpm = [func](bc_orbiter::Vessel& vsl, int id, int event, int mx, int my) {func(vsl); };
////            bc_orbiter::funcPanelRedrawEvent fpr = [funcRedraw](bc_orbiter::Vessel& vsl, int id, int event, SURFHANDLE surf) { funcRedraw(vsl); };
//
//            rcPanel = rcPnlRedraw;
//            pnlVerts = &pnlVerts;   
//
//            vessel.RegisterVCMouseEvent(vcLoc, toggleOnOff.hitRadius, fvm);
//            vessel.RegisterPanelMouseEvent(pnlLoc, fpm);
//            panelRedrawId_ = vessel.RegisterPanelRedrawEvent(rcPnlRedraw, fpr);
//        }
//
//        void RequestRedraw(bc_orbiter::Vessel& vessel) {
//            vessel.TriggerPanelRedrawArea(panel::right, panelRedrawId_);
//        }
//
//        void Update(bc_orbiter::Vessel& vessel, bool isOn, double simdt) { anim_.Update(vessel, isOn ? 1.0 : 0.0, simdt); }
//    private:
//        void panelRedraw(bc_orbiter::Vessel& v, int id, int event, SURFHANDLE surf) {
//            auto ofs = bco::UVOffset(bm::pnlright::pnlPwrCanopy_vrt);
//            auto mesh = vessel.GetpanelMeshHandle(cmn::panel::right);
//            bco::DrawPanelOffset(mesh, bm::pnlright::pnlPwrCanopy_id, bm::pnlright::pnlPwrCanopy_vrt, ofs * (isPowerSwitchOn_ ? 1.0 : 0.0));
//        }
//
//        int panelRedrawId_{ 0 };
//        UINT group_;
//
//        UINT animId_{ 0 };
//        std::unique_ptr<MGROUP_ROTATE> transform_;
//
//        bc_orbiter::AnimationTarget anim_{ toggleOnOff.animSpeed };
//      
//        RECT rcPanel{ 0.0, 0.0, 0.0, 0.0 };
//        NTVERTEX* panelVerts_{ nullptr };
//    };

}