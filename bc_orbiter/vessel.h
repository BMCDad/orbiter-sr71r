//	Vessel - bco Orbiter Library
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
#include "Animation.h"
#include "Control.h"
#include "HandlerInterfaces.h"
#include "Orbitersdk.h"

#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>

namespace bc_orbiter
{
	class Component;
}

namespace bc_orbiter
{
    /**
    Base class for Orbiter vessels.
    */
    class Vessel 
      : public VESSEL4,
        public AvionicsProvider,
        public PropulsionControl
    {
    public:
        Vessel(OBJHANDLE hvessel, int flightmodel);

        // clbk overrides:
        virtual bool clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;
        virtual bool clbkLoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) override;
        virtual bool clbkLoadVC(int id) override;
        virtual bool clbkPanelMouseEvent(int id, int event, int mx, int my) override;
        virtual bool clbkPanelRedrawEvent(int id, int event, SURFHANDLE surf, void* context) override;
        virtual void clbkPostCreation() override;
        virtual void clbkPostStep(double simt, double simdt, double mjd) override;
        virtual void clbkSetClassCaps(FILEHANDLE cfg) override;
        virtual bool clbkVCMouseEvent(int id, int event, VECTOR3& p) override;
        virtual bool clbkVCRedrawEvent(int id, int event, SURFHANDLE surf) override;
        virtual void clbkVisualCreated(VISHANDLE visHandle, int refCount) override;
        virtual void clbkVisualDestroyed(VISHANDLE vis, int refcount) override;

        virtual ~Vessel() {}

        DEVMESHHANDLE   GetVirtualCockpitMesh0() { return meshVirtualCockpit0_; }
        MESHHANDLE      GetVCMeshHandle0() { return vcMeshHandle0_; }
        void            SetVCMeshHandle0(MESHHANDLE mh) { vcMeshHandle0_ = mh; }
        void            SetVCMeshIndex0(int index) { vcIndex0_ = index; }
        void            SetMainMeshIndex(int index) { mainIndex_ = index; }
        UINT            GetVCMeshIndex() const { return vcIndex0_; }

        UINT            GetMainMeshIndex() const { return mainIndex_; }
        void            SetPanelMeshHandle(int id, MESHHANDLE mh) { panel_mesh_handles_[id] = mh; }
        MESHHANDLE      GetpanelMeshHandle(int id) const {
            auto c = panel_mesh_handles_.find(id);
            return (c == panel_mesh_handles_.end()) ? nullptr : c->second;
        }

        bool            IsStoppedOrDocked();
        bool            IsCreated() { return isCreated_; }
        void            CreateMainPropellant(double max) { mainPropellant_ = CreatePropellantResource(max); }
        void            CreateRcsPropellant(double max) { rcsPropellant_ = CreatePropellantResource(max); }

        PROPELLANT_HANDLE MainPropellant() const { return mainPropellant_; }
        PROPELLANT_HANDLE RcsPropellant() const { return rcsPropellant_; }

        /**
        Registers Animation with the base Vessel, and assigns its id.
        @param Animation Animation to register.
        */
        UINT CreateVesselAnimation(Animation& animation)
        {
            auto animId = VESSEL3::CreateAnimation(0.0);
            animation.VesselId(animId);
            return animId;
        }

        /**
        Adds an Animation group to an existing Animation.  See CreateVesselAnimation.
        @param animId The Animation id returned from CreateVesselAnimation.
        @param meshIdx The mesh index to animate.
        @param trans The mesh transformation.
        @param parent The parent group if any.
        */
        ANIMATIONCOMPONENT_HANDLE AddVesselAnimationComponent(
            UINT animId,
            UINT meshIdx,
            AnimationGroup* transform,
            ANIMATIONCOMPONENT_HANDLE parent = nullptr)
        {
            ANIMATIONCOMPONENT_HANDLE result = nullptr;

            //auto eh = animations_.find(animId);

            transform->transform_->mesh = meshIdx;
            result = VESSEL3::AddAnimationComponent(
                animId,
                transform->start_,
                transform->stop_,
                transform->transform_.get(),
                parent);

            return result;
        }

        /**
        Sets the state of an Animation directly.  Call this when an Animation
        should immediatly reflect the desired state and not move to it over time.
        For example when loading the state from a configuration file.
        @param id The Animation id to set.
        @param state The state to set the Animation to.
        */
        void SetAnimationState(const AnimationTarget& anim)
        {
            //auto eh = animations_.find(anim.VesselId());
            //if (eh != animations_.end())	eh->second->SetState(anim.GetState());
        }

        int GetIdForComponent(Component* comp)
        {
            auto id = ++nextEventId_;
            idComponentMap_[id] = comp;
            return id;
        }

        int GetControlId() { return ++nextEventId_; }

        void AddControl(Control* ctrl) {
            if (ctrl->GetId() == -1) ctrl->SetId(GetControlId());
            controls_.push_back(ctrl);
        }

        void AddComponent(VesselComponent* c) { components_.push_back(c); }

        // Clean this up later when Component goes away
        void RegisterVCComponent(int id, VesselComponent* vc) {
            map_vc_component_[id] = vc;
        }

        void RegisterPanelComponent(int id, VesselComponent* pnl) {
            map_panel_component_[id] = pnl;
        }

        // AvionicsProvider
        double GetAvAltitude() const				override { return this->GetAltitude(); }
        void   GetAngularVelocity(VECTOR3& v)	override { this->GetAngularVel(v); }
        double GetAvBank() const					override { return this->GetBank(); }
        double GetHeading() const				override { return this->GetYaw(); }
        double GetKEAS() const					override { return GetVesselKeas(this); }
        double GetMACH() const					override { return this->GetMachNumber(); }
        double GetAvPitch() const				override { return this->GetPitch(); }
        double GetVerticalSpeed() const		override { return GetVerticalSpeedFPM(this); }

        // PropulsionControl
        double GetMainThrustLevel() const	override { return this->GetThrusterGroupLevel(THGROUP_MAIN); }
        void   SetMainThrustLevel(double l)	override { this->SetThrusterGroupLevel(THGROUP_MAIN, l); }
        void   SetAttitudeRotation(Axis axis, double level) override { this->SetAttitudeRotLevel((int)axis, level); }

        // SurfaceControl
        void set_aileron_level(double l) { this->SetControlSurfaceLevel(AIRCTRL_AILERON, l); }
        void set_elevator_level(double l) { this->SetControlSurfaceLevel(AIRCTRL_ELEVATORTRIM, l); }


        // HERE is the new event handler code:::
        int RegisterVCMouseEvent(const VECTOR3& hitLocation, double radius, funcVCMouseEvent func) {
            auto id = GetControlId();
            mapVCMouseEvents_[id] = std::make_unique<VCMouseEventData>(hitLocation, radius, func);
            return id;
        }


        int RegisterPanelMouseEvent(const RECT& rect, funcPanelMouseEvent func) {
            auto id = GetControlId();
            mapPanelMouseEvents_[id] = std::make_unique<PanelMouseEventData>(rect, func);
            return id;
        }


        int RegisterVCRedrawEvent(funcVCRedrawEvent func) {
            return RegisterVCRedrawEvent(_R(0.0, 0.0, 0.0, 0.0), func);
        }

        int RegisterVCRedrawEvent(const RECT& rect, funcVCRedrawEvent func) {
            auto id = GetControlId();
            mapVCRedrawEvents_[id] = std::make_unique<VCRedrawEventData>(rect, func);
            return id;
        }


        int RegisterPanelRedrawEvent(funcPanelRedrawEvent func) {
            auto id = GetControlId();
            mapPanelRedrawEvents_[id] = std::make_unique<PanelRedrawEventData>(func);
            return id;
        }


        int RegisterVCMFDEvent(RECT rect, const VECTOR3& loc, double radius, DWORD txId, funcPanelMouseEvent mouse, funcPanelRedrawEvent redraw) {
            auto id = GetControlId();
            mapVCMFDEvents_[id] = std::make_unique<VCMFDEventData>(rect, loc, radius, txId, mouse, redraw);
            return id;
        }

        // END
    private:
        // HERE is the new event handlermaps:
        std::map<int, std::unique_ptr<VCMouseEventData>>        mapVCMouseEvents_;
        std::map<int, std::unique_ptr<VCRedrawEventData>>       mapVCRedrawEvents_;
        std::map<int, std::unique_ptr<PanelMouseEventData>>     mapPanelMouseEvents_;
        std::map<int, std::unique_ptr<PanelRedrawEventData>>    mapPanelRedrawEvents_;
        std::map<int, std::unique_ptr<VCMFDEventData>>          mapVCMFDEvents_;
        // END


        std::vector<Control*>							controls_;
        std::map<int, VCEventTarget*>					map_vc_targets_;
        std::map<int, PanelEventTarget*>				map_panel_targets_;
        std::vector<PanelAnimation*>					panel_animations_;
        std::vector<VCTexAnimation*>					vc_texture_animations_;
        std::map<int, VCAnimation*>					map_vc_animations_;

        std::vector<VesselComponent*>       components_;
        std::map<int, VesselComponent*>     map_vc_component_;
        std::map<int, VesselComponent*>     map_panel_component_;

        std::map<int, Component*>                   idComponentMap_;		// still used by MFDs.  Need to figure that out, then we can get rid of Component
//        std::map<UINT, std::unique_ptr<Animation>>  animations_;
        std::map<int, MESHHANDLE>                   panel_mesh_handles_;

        int					nextEventId_{ 0 };
        bool				isCreated_{ false };	// Set true after clbkPostCreation
        VISHANDLE			visualHandle_{ nullptr };
        DEVMESHHANDLE		meshVirtualCockpit0_{ nullptr };
        MESHHANDLE			vcMeshHandle0_{ nullptr };
        MESHHANDLE			panelMeshHandle0_{ nullptr };
        MESHHANDLE			panelMeshHandle1_{ nullptr };
        MESHHANDLE			panelMeshHandle2_{ nullptr };
        UINT				vcIndex0_{ 0 };
        UINT                mainIndex_{ 0 };
        VESSELSTATUS2		vesselStatus_;

        // Propellent (multiple components need this on setup, so put it in the Vessel class)
        PROPELLANT_HANDLE	mainPropellant_{ nullptr };
        PROPELLANT_HANDLE	rcsPropellant_{ nullptr };
    };

    inline Vessel::Vessel(OBJHANDLE hvessel, int flightmodel) :
        VESSEL4(hvessel, flightmodel)
    {
        // HandleSetClassCaps Vessel status.
        memset(&vesselStatus_, 0, sizeof(vesselStatus_));
        vesselStatus_.version = 2;
    }

    inline bool Vessel::clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
    {
        for (auto& ps : components_) {
            ps->HandleDrawHud(*this, mode, hps, skp);
        }

        VESSEL3::clbkDrawHUD(mode, hps, skp);
        return true;
    }

    inline bool Vessel::clbkLoadVC(int id)
    {
        // NEW HANDLER HERE
        auto vcMeshHandle = GetVCMeshHandle0(); // Assuming one VC.

        for(auto & h : mapVCMouseEvents_) {
            oapiVCRegisterArea(h.first, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
            oapiVCSetAreaClickmode_Spherical(h.first, h.second->location_, h.second->radius_);
        }

        for (auto& h : mapVCRedrawEvents_) {
            oapiVCRegisterArea(h.first, PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE);
        }

        for (auto& m : mapVCMFDEvents_) {
            auto surfHandle = oapiGetTextureHandle(vcMeshHandle, m.second->texId_);
            oapiVCRegisterArea(
                m.first, 
                m.second->rect_, 
                PANEL_REDRAW_USER, 
                PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY, 
                PANEL_MAP_BACKGROUND, 
                surfHandle);
        }
        // END
        


        // Handle controls with load vc requirements.
        for (auto & vc : map_vc_targets_) {
            oapiVCRegisterArea(
                vc.first,							// Area ID
                vc.second->VCRedrawFlags(),		// PANEL_REDRAW_*
                vc.second->VCMouseFlags());		// PANEL_MOUSE_*

            oapiVCSetAreaClickmode_Spherical(
                vc.first,							// Area ID
                vc.second->VCEventLocation(),
                vc.second->VCEventRadius());
        }

        // handle Vessel components that require load vc.
        for (auto & vc : components_) {
            vc->HandleLoadVC(*this, id);
        }

        return true;
    }

    inline void Vessel::clbkSetClassCaps(FILEHANDLE cfg)
    {
        // Handle controls that need initialization.
        for (auto & vc : controls_) {
            if (auto* c = dynamic_cast<VCAnimation*>(vc)) {
                auto aid = VESSEL3::CreateAnimation(0);
                auto trans = c->VCAnimationGroup();
                trans->transform_->mesh = GetVCMeshIndex();
                VESSEL3::AddAnimationComponent(
                    aid,
                    trans->start_,
                    trans->stop_,
                    trans->transform_.get());

                map_vc_animations_[aid] = c;
            }

            if (auto* c = dynamic_cast<PanelAnimation*>(vc)) panel_animations_.push_back(c);
            if (auto* c = dynamic_cast<VCEventTarget*>(vc)) map_vc_targets_[vc->GetId()] = c;
            if (auto* c = dynamic_cast<PanelEventTarget*>(vc)) map_panel_targets_[vc->GetId()] = c;
            if (auto* c = dynamic_cast<VCTexAnimation*>(vc)) vc_texture_animations_.push_back(c);
        }

        for (auto & sc : components_) {
            sc->HandleSetClassCaps(*this);
        }
    }

    inline bool Vessel::clbkVCMouseEvent(int id, int event, VECTOR3& p)
    {
        // NEW HERE
        auto vme = mapVCMouseEvents_.find(id);
        if (vme != mapVCMouseEvents_.end()) {
            vme->second->func_(*this, id, event, p);
        }

        auto vmf = mapVCMFDEvents_.find(id);
        if (vmf != mapVCMFDEvents_.end()) {
            vmf->second->funcMouse_(*this, id, event, 0, 0);
        }

        // END NEW
        // Old new mode...
        //auto c = idComponentMap_.find(id);
        //if (c != idComponentMap_.end()) {
        //    return c->second->OnVCMouseEvent(id, event);
        //}

        // NEW mode
        auto vc = map_vc_targets_.find(id);
        if (vc != map_vc_targets_.end()) {
            vc->second->OnMouseClick(*this, id, event);
        }

        return false;
    }

    inline bool Vessel::clbkVCRedrawEvent(int id, int event, SURFHANDLE surf)
    {
        if (nullptr == meshVirtualCockpit0_)	return false;

        auto vcr = mapVCRedrawEvents_.find(id);
        if (vcr != mapVCRedrawEvents_.end()) {
            vcr->second->func_(*this, id, event, surf, meshVirtualCockpit0_);
        }

        auto vcm = mapVCMFDEvents_.find(id);
        if (vcm != mapVCMFDEvents_.end()) {
            vcm->second->funcRedraw_(*this, id, event, surf);
        }

        //auto c = idComponentMap_.find(id);
        //if (c != idComponentMap_.end()) {
        //    return c->second->OnVCRedrawEvent(id, event, surf);
        //}

        // NEW mode
        auto pe = map_vc_targets_.find(id);
        if (pe != map_vc_targets_.end()) {
            pe->second->OnVCRedraw(meshVirtualCockpit0_);
            return true;
        }

        auto pc = map_vc_component_.find(id);
        if (pc != map_vc_component_.end()) {
            pc->second->HandleRedrawVC(*this, id, event, surf);
            return true;
        }

        return false;
    }

    inline void Vessel::clbkVisualCreated(VISHANDLE visHandle, int refCount)
    {
        visualHandle_ = visHandle;
        meshVirtualCockpit0_ = GetDevMesh(visualHandle_, vcIndex0_);
    }

    inline void Vessel::clbkVisualDestroyed(VISHANDLE vis, int refcount)
    {
        visualHandle_ = nullptr;
        meshVirtualCockpit0_ = nullptr;
        isCreated_ = false;
    }

    inline void Vessel::clbkPostStep(double simt, double simdt, double mjd)
    {
        // Update animations
        //for (auto& a : animations_)
        //{
        //    a.second->Step(simdt);
        //    auto state = a.second->GetState();
        //    VESSEL3::SetAnimation(a.first, state);
        //}

        // NEW MODE  << This will go away eventually
        if (oapiCockpitMode() == COCKPIT_VIRTUAL) {
            for (auto& va : map_vc_animations_) {
                auto newState = va.second->VCStep(simdt);
                VESSEL3::SetAnimation(va.first, newState);
            }

            auto mesh = GetVirtualCockpitMesh0();
            for (auto& vt : vc_texture_animations_) {
                vt->VCStep(mesh, simdt);
            }
        }

        if (oapiCockpitMode() == COCKPIT_PANELS) {
            for (auto& pa : panel_animations_) {
                auto mesh = GetpanelMeshHandle(pa->PanelID());
                pa->PanelStep(mesh, simdt);
            }
        }

        for (auto& ps : components_) {
            ps->HandlePostStep(*this, simt, simdt, mjd);
        }
    }

    inline void Vessel::clbkPostCreation()
    {
        isCreated_ = true;
    }

    inline bool Vessel::IsStoppedOrDocked()
    {
        vesselStatus_.flag = 0;
        GetStatusEx(&vesselStatus_);
        return ((vesselStatus_.status == 1) || (DockingStatus(0) == 1));
    }

    inline bool Vessel::clbkLoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
    {
        // NEW HANDLER HERE
        for (auto& p : mapPanelMouseEvents_) {
            RegisterPanelArea(hPanel, p.first, p.second->rect_, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
        }

        for (auto& p : mapPanelRedrawEvents_) {
            RegisterPanelArea(hPanel, p.first, _R(0.0, 0.0, 0.0, 0.0), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE);
        }
        // NEW END


        for (auto & p : map_panel_targets_) {	// For panel, mouse and redraw happen in the same call.
            if (p.second->PanelId() != id) continue;

            RegisterPanelArea(
                hPanel,
                p.first,                        // Area ID
                p.second->PanelRect(),
                p.second->PanelRedrawFlags(), // PANEL_REDRAW_*
                p.second->PanelMouseFlags()); // PANEL_MOUSE_*
        }

        for (auto & vc : components_) {
            vc->HandleLoadPanel(*this, id, hPanel);
        }

        return true;
    }

    inline bool Vessel::clbkPanelRedrawEvent(int id, int event, SURFHANDLE surf, void* context)
    {
        // NEW HERE
        auto pre = mapPanelRedrawEvents_.find(id);
        if (pre != mapPanelRedrawEvents_.end()) {
            pre->second->func_(*this, id, event, surf);
            return true;
        }
        // NEW END

        // OLD New mode...
        //auto c = idComponentMap_.find(id);
        //if (c != idComponentMap_.end())
        //{
        //    return c->second->OnPanelRedrawEvent(id, event, surf);
        //}


        auto pe = map_panel_targets_.find(id);
        if (pe != map_panel_targets_.end()) {
            pe->second->OnPanelRedraw(GetpanelMeshHandle(pe->second->PanelId()));
            return true;
        }

        auto pc = map_panel_component_.find(id);
        if (pc != map_panel_component_.end()) {
            pc->second->HandleRedrawPanel(*this, id, event, surf);
        }

        return true;
    }

    inline bool Vessel::clbkPanelMouseEvent(int id, int event, int mx, int my)
    {
        // NEW HANDLER HERE
        auto pne = mapPanelMouseEvents_.find(id);
        if (pne != mapPanelMouseEvents_.end()) {
            pne->second->func_(*this, id, event, mx, my);
        }
        // NEW END

        // Old New mode...
        //auto c = idComponentMap_.find(id);
        //if (c != idComponentMap_.end())
        //{
        //    return c->second->OnPanelMouseEvent(id, event);
        //}



        // 
        auto pe = map_panel_targets_.find(id);
        if (pe != map_panel_targets_.end()) {
            pe->second->OnMouseClick(*this, id, event);
        }

        return true;
    }
}