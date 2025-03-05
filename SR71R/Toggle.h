#pragma once

#include <memory>
#include <functional>

#include "OrbiterAPI.h"
#include "..\bc_orbiter\Animation.h"
#include "..\bc_orbiter\vessel.h"


namespace bco = bc_orbiter;

namespace sr71
{
    /// <summary>
    /// A class that models a physical toggle switch for the SR71.
    /// </summary>
    class Toggle
    {
    public:
        /// <summary>
        /// Constructs toggle switch.
        /// </summary>
        /// <param name="vcGroup">The group id for the VC switch mesh to animate</param>
        /// <param name="vcLoc">The location of that switch for rotation and hit testing</param>
        /// <param name="vcAxis">The second location used to define the rotation axis</param>
        /// <param name="pnlGroupId">The group id for the panel mesh</param>
        /// <param name="pnlVerts">The NTVERTEX array defining the mesh object</param>
        /// <param name="pnlRect">The rectangle used for hit testing</param>
        /// <param name="panelId">The 2-D panel id where the switch is</param>
        Toggle(
            UINT            vcGroup,
            const VECTOR3&  vcLoc,
            const VECTOR3&  vcAxis,
            UINT            pnlGroupId,
            const NTVERTEX* pnlVerts,
            const RECT&     pnlRect,
            int             panelId)
          : vcGroup_(vcGroup),
            vcLoc_(vcLoc),
            vcAxis_(vcAxis),
            pnlGroupId_(pnlGroupId),
            pnlVerts_(pnlVerts),
            pnlRect_(pnlRect),
            panelId_(panelId)
        { }

        /// <summary>
        /// Register the object with the vessel.  This should be called in SetClassCaps
        /// </summary>
        /// <param name="vessel">The owning vessel</param>
        void Register(bc_orbiter::Vessel& vessel)
        {
            // These function defs basically translate between the specific VC and panel mouse events and the mouseHit event used by this class.
            std::function<void(bco::Vessel&)> mouseFunc = [this](bco::Vessel& v) {mouseHit(v); };
            bco::funcVCMouseEvent fvm = [mouseFunc](bc_orbiter::Vessel& vsl, int id, int event, VECTOR3& location) { mouseFunc(vsl); };
            bco::funcPanelMouseEvent fpm = [mouseFunc](bc_orbiter::Vessel& vsl, int id, int event, int mx, int my) { mouseFunc(vsl); };

            // Register the events with the vessel
            vessel.RegisterVCMouseEvent(vcLoc_, radius, fvm);
            vessel.RegisterPanelMouseEvent(pnlRect_, fpm);
            panelRedrawId_ = vessel.RegisterPanelRedrawEvent([this](bco::Vessel& vsl, int id, int event, SURFHANDLE surf) {this->panelRedraw(vsl, id, event, surf); });

            // Setup the switch animation
            auto vcIdx = vessel.GetVCMeshIndex();

            VECTOR3 axis = vcAxis_ - vcLoc_;
            normalise(axis);
            transform_ = std::make_unique<MGROUP_ROTATE>(vcIdx, &vcGroup_, 1, vcLoc_, axis, angle);

            animId_ = vessel.CreateVesselAnimation(anim_);
            anim_.VesselId(animId_);
            vessel.AddAnimationComponent(animId_, 0, 1, transform_.get());
        }

        /// <summary>
        /// Forces a redraw of the panel area.  This is available to the outer class, but it probably not needed.  Calling SetIsOn will
        /// also trigger a redraw.
        /// </summary>
        /// <param name="vessel">Owning vessel</param>
        void RequestRedraw(bc_orbiter::Vessel& vessel) {
            vessel.TriggerPanelRedrawArea(panelId_, panelRedrawId_);
        }

        /// <summary>
        /// This is called from the Step method so the switch animation can run.
        /// </summary>
        /// <param name="vessel">Owning vessel</param>
        /// <param name="simdt">Time delta</param>
        void Update(bc_orbiter::Vessel& vessel, double simdt) { anim_.Update(vessel, isOn_ ? 1.0 : 0.0, simdt); }

        bool IsOn() const { return isOn_; }
        void SetIsOn(bco::Vessel& v, bool o) { 
            isOn_ = o; 
            RequestRedraw(v);
        }
    private:

        const float     angle   =  1.5708;           // Rotation angle (RAD)
        const double    speed   = 10.0;              // Anim speed
        const double    start   =  0.0;              // anim start
        const double    end     =  1.0;              // anim end
        const double    radius  =  0.01;             // VC hit radius

        void mouseHit(bco::Vessel& v) {
            isOn_ = !isOn_;
            RequestRedraw(v);
        }

        void panelRedraw(bc_orbiter::Vessel& v, int id, int event, SURFHANDLE surf) {
            auto ofs = bco::UVOffset(pnlVerts_);
            auto mesh = v.GetpanelMeshHandle(panelId_);
            bco::DrawPanelOffset(mesh, pnlGroupId_, pnlVerts_, ofs * (isOn_ ? 1.0 : 0.0));
        }

        UINT            vcGroup_;
        const VECTOR3&  vcLoc_;
        const VECTOR3&  vcAxis_;
        UINT            pnlGroupId_;
        const NTVERTEX* pnlVerts_;
        const RECT&     pnlRect_;
        int             panelId_;

        bool            isOn_{ false };
        std::unique_ptr<MGROUP_ROTATE> transform_;
        int panelRedrawId_{ 0 };

        UINT animId_{ 0 };

        bc_orbiter::AnimationTarget anim_{ speed };
    };

};