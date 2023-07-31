//	vessel - bco Orbiter Library
//	Copyright(C) 2015  Blake Christensen
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
#include "Component.h"
#include "Control.h"
#include "handler_interfaces.h"
#include "IAnimationState.h"
#include "Orbitersdk.h"
#include "signals.h"

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
    class vessel : 
		  public VESSEL4
		, public avionics_provider
		, public propulsion_control
    {
    public:
        vessel(OBJHANDLE hvessel, int flightmodel);

		// clbk overrides:
		virtual bool clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;
		virtual bool clbkLoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) override;
		virtual bool clbkLoadVC(int id) override;
		virtual bool clbkPanelMouseEvent(int id, int event, int mx, int my) override;
		virtual bool clbkPanelRedrawEvent(int id, int event, SURFHANDLE surf, void* context) override;
		virtual void clbkPostCreation() override;
        virtual void clbkPostStep(double simt, double simdt, double mjd) override;
		virtual void clbkSetClassCaps(FILEHANDLE cfg) override;
		virtual bool clbkVCMouseEvent(int id, int event, VECTOR3 &p) override;
		virtual bool clbkVCRedrawEvent(int id, int event, SURFHANDLE surf) override;
		virtual void clbkVisualCreated(VISHANDLE visHandle, int refCount) override;
		virtual void clbkVisualDestroyed(VISHANDLE vis, int refcount) override;

		virtual ~vessel() {}

		DEVMESHHANDLE   GetVirtualCockpitMesh0()			{ return meshVirtualCockpit0_; }
        MESHHANDLE      GetVCMeshHandle0()					{ return vcMeshHandle0_; }
        void            SetVCMeshHandle0(MESHHANDLE mh)		{ vcMeshHandle0_    = mh; }
		void            SetVCMeshIndex0(int index)			{ vcIndex0_         = index; }
        void            SetMainMeshIndex(int index)			{ mainIndex_        = index; }
        UINT            GetVCMeshIndex() const				{ return vcIndex0_; }

		UINT            GetMainMeshIndex() const			{ return mainIndex_; }
		void			SetPanelMeshHandle0(MESHHANDLE mh)	{ panelMeshHandle0_ = mh; }
		MESHHANDLE		GetpanelMeshHandle0() const			{ return panelMeshHandle0_; }

		bool			IsStoppedOrDocked();
		bool			IsCreated()							{ return isCreated_; }
        void			CreateMainPropellant(double max)	{ mainPropellant_ = CreatePropellantResource(max); }
        void			CreateRcsPropellant(double max)		{ rcsPropellant_ = CreatePropellantResource(max); }

        PROPELLANT_HANDLE MainPropellant() const			{ return mainPropellant_; }
        PROPELLANT_HANDLE RcsPropellant() const				{ return rcsPropellant_; }

        /**
        Creates an animation and registers it with the base vessel.  animation_target registered
		with the base class automatically receive part of the PostStep time processing.
        @param target IAnimationState object that will control the animation state.
        @param speed The speed of the animation.
        @param func The function to call when the animation hits its target state.
        */
        template<typename AT=animation_target>
        UINT CreateVesselAnimation(IAnimationState* target, double speed = 1.0, func_target_achieved func = nullptr)
        {
            auto animId = VESSEL3::CreateAnimation(0.0);

            animations_[animId] = std::make_unique<AT>(target, speed, func);
            return animId;
        }

		/**
		Adds an animation group to an existing animation.  See CreateVesselAnimation.
		@param animId The animation id returned from CreateVesselAnimation.
		@param meshIdx The mesh index to animate.
		@param trans The mesh transformation.
		@param parent The parent group if any.
		*/
        ANIMATIONCOMPONENT_HANDLE AddVesselAnimationComponent(
            UINT animId, 
            UINT meshIdx,
            animation_group* transform,
            ANIMATIONCOMPONENT_HANDLE parent = nullptr)
        {
            ANIMATIONCOMPONENT_HANDLE result = nullptr;

            auto eh = animations_.find(animId);

            if (eh != animations_.end())
            {
                transform->transform_->mesh = meshIdx;
                result = VESSEL3::AddAnimationComponent(
					animId, 
					transform->start_, 
					transform->stop_, 
					transform->transform_.get(), 
					parent);
            }

            return result;
        }

		/**
		Sets the state of an animation directly.  Call this when an animation
		should immediatly reflect the desired state and not move to it over time.
		For example when loading the state from a configuration file.
		@param id The animation id to set.
		@param state The state to set the animation to.
		*/
        void SetAnimationState(const animation_target& anim)
        {
            auto eh = animations_.find(anim.VesselId());
            if (eh != animations_.end())	eh->second->SetState(anim.GetState());
        }

		int GetIdForComponent(Component* comp)
		{
			auto id = ++nextEventId_;
			idComponentMap_[id] = comp;
			return id;
		}

		int GetControlId() { return ++nextEventId_; }

		void AddControl(control* ctrl) { 
			if (ctrl->get_id() == -1) ctrl->set_id(GetControlId());
			controls_.push_back(ctrl); 
		}

		void AddComponent(vessel_component* c) { components_.push_back(c); }

		// Clean this up later when Component goes away
		void RegisterVCComponent(int id, load_vc* vc) {
			map_vc_component_[id] = vc;
		}

		void RegisterPanelComponent(int id, load_panel* pnl) {
			map_panel_component_[id] = pnl;
		}

		// avionics_provider
		double get_altitude() const				override { return this->GetAltitude(); }
		void   get_angular_velocity(VECTOR3& v)	override { this->GetAngularVel(v); }
		double get_bank() const					override { return this->GetBank(); }
		double get_heading() const				override { return this->GetYaw(); }
		double get_keas() const					override { return GetVesselKeas(this); }
		double get_mach() const					override { return this->GetMachNumber(); }
		double get_pitch() const				override { return this->GetPitch(); }
		double get_vertical_speed() const		override { return GetVerticalSpeedFPM(this); }

		// propulsion_control
		double get_main_thrust_level() const	override { return this->GetThrusterGroupLevel(THGROUP_MAIN); }
		void   set_main_thrust_level(double l)	override { this->SetThrusterGroupLevel(THGROUP_MAIN, l); }
		void   set_attitude_rotation(Axis axis, double level) override { this->SetAttitudeRotLevel((int)axis, level); }

		// surface_control
		void set_aileron_level(double l)		{ this->SetControlSurfaceLevel(AIRCTRL_AILERON, l); }
		void set_elevator_level(double l)		{ this->SetControlSurfaceLevel(AIRCTRL_ELEVATOR, l); }

	private:

		std::vector<control*>							controls_;
		std::map<int, vc_event_target*>					map_vc_targets_;
		std::map<int, panel_event_target*>				map_panel_targets_;
		std::vector<panel_animation*>					map_panel_animations_;
		std::vector<vc_tex_animation*>					vc_texture_animations_;
		std::map<int, vc_animation*>					map_vc_animations_;

		std::vector<vessel_component*>					components_;
		std::vector<post_step*>							post_step_components_;
		std::vector<set_class_caps*>					set_class_caps_components_;
		std::vector<draw_hud*>							draw_hud_components_;
		std::vector<load_vc*>							load_vc_components_;
		std::vector<load_panel*>						load_panel_components_;
		std::map<int, load_vc*>							map_vc_component_;
		std::map<int, load_panel*>						map_panel_component_;

		std::map<int, Component*>						idComponentMap_;		// still used by MFDs.  Need to figure that out, then we can get rid of Component
		std::map<UINT, std::unique_ptr<animation>>     animations_;
		
		int					nextEventId_				{ 0 };
		bool				isCreated_					{ false };	// Set true after clbkPostCreation
		VISHANDLE			visualHandle_				{ nullptr };
		DEVMESHHANDLE		meshVirtualCockpit0_		{ nullptr };
		MESHHANDLE			vcMeshHandle0_				{ nullptr };
		MESHHANDLE			panelMeshHandle0_			{ nullptr };
		UINT				vcIndex0_					{ 0 };
		UINT                mainIndex_					{ 0 };
		VESSELSTATUS2		vesselStatus_;

		// Propellent (multiple components need this on setup, so put it in the vessel class)
		PROPELLANT_HANDLE	mainPropellant_				{ nullptr };
		PROPELLANT_HANDLE	rcsPropellant_				{ nullptr };
	};

	inline vessel::vessel(OBJHANDLE hvessel, int flightmodel) :
		VESSEL4(hvessel, flightmodel)
	{
		// handle_set_class_caps vessel status.
		memset(&vesselStatus_, 0, sizeof(vesselStatus_));
		vesselStatus_.version = 2;
	}

	inline bool vessel::clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
	{
		for (auto& ps : draw_hud_components_) {
			ps->handle_draw_hud(*this, mode, hps, skp);
		}
		return true;
	}

	inline bool vessel::clbkLoadVC(int id)
	{
		// Handle controls with load vc requirements.
		for each (auto & vc in map_vc_targets_) {
			oapiVCRegisterArea(
				vc.first,							// Area ID
				vc.second->vc_redraw_flags(),		// PANEL_REDRAW_*
				vc.second->vc_mouse_flags());		// PANEL_MOUSE_*

			oapiVCSetAreaClickmode_Spherical(
				vc.first,							// Area ID
				vc.second->vc_event_location(),
				vc.second->vc_event_radius());
		}

		// handle vessel components that require load vc.
		for each (auto & vc in load_vc_components_) {
			vc->handle_load_vc(*this, id);
		}

		return true;
	}

	inline void vessel::clbkSetClassCaps(FILEHANDLE cfg)
	{
		// set_class_caps will flesh out to a more general 'component' list (non-ui/control intities)
		for each (auto & cc in components_) {
			if (auto* ac = dynamic_cast<post_step*		>(cc))	post_step_components_.push_back(ac);
			if (auto* ac = dynamic_cast<set_class_caps*	>(cc))	set_class_caps_components_.push_back(ac);
			if (auto* ac = dynamic_cast<draw_hud*		>(cc))	draw_hud_components_.push_back(ac);
			if (auto* ac = dynamic_cast<load_vc*		>(cc))	load_vc_components_.push_back(ac);
			if (auto* ac = dynamic_cast<load_panel*		>(cc))	load_panel_components_.push_back(ac);
		}

		// Handle controls that need initialization.
		for each (auto & vc in controls_)
		{
			if (auto* c = dynamic_cast<vc_animation*>(vc)) {
				auto aid = VESSEL3::CreateAnimation(0);
				auto trans = c->vc_animation_group();
				trans->transform_->mesh = GetVCMeshIndex();
				VESSEL3::AddAnimationComponent(
					aid,
					trans->start_,
					trans->stop_,
					trans->transform_.get());

				map_vc_animations_[aid] = c;
			}

			if (auto* c = dynamic_cast<panel_animation*		>(vc)) map_panel_animations_.push_back(c);
			if (auto* c = dynamic_cast<vc_event_target*		>(vc)) map_vc_targets_[vc->get_id()] = c;
			if (auto* c = dynamic_cast<panel_event_target*	>(vc)) map_panel_targets_[vc->get_id()] = c;
			if (auto* c = dynamic_cast<vc_tex_animation*	>(vc)) vc_texture_animations_.push_back(c);
		}

		for each (auto & sc in set_class_caps_components_) {
			sc->handle_set_class_caps(*this);
		}
	}

	inline bool vessel::clbkVCMouseEvent(int id, int event, VECTOR3 &p)
	{
		// Old new mode...
		auto c = idComponentMap_.find(id);
		if (c != idComponentMap_.end())
		{
			return c->second->OnVCMouseEvent(id, event);
		}

		// NEW mode
		auto vc = map_vc_targets_.find(id);
		if (vc != map_vc_targets_.end()) {
			vc->second->on_event(id, event);
		}

		return false;
	}

	inline bool vessel::clbkVCRedrawEvent(int id, int event, SURFHANDLE surf)
	{
		if (nullptr == meshVirtualCockpit0_)	return false;

		auto c = idComponentMap_.find(id);
		if (c != idComponentMap_.end())
		{
			return c->second->OnVCRedrawEvent(id, event, surf);
		}

		// NEW mode
		auto pe = map_vc_targets_.find(id);
		if (pe != map_vc_targets_.end()) {
			pe->second->on_vc_redraw(meshVirtualCockpit0_);
			return true;
		}

		auto pc = map_vc_component_.find(id);
		if (pc != map_vc_component_.end()) {
			pc->second->handle_redraw_vc(*this, id, event, surf);
			return true;
		}

		return false;
	}

	inline void vessel::clbkVisualCreated(VISHANDLE visHandle, int refCount)
	{
		visualHandle_ = visHandle;
		meshVirtualCockpit0_ = GetDevMesh(visualHandle_, vcIndex0_);
	}

	inline void vessel::clbkVisualDestroyed(VISHANDLE vis, int refcount)
	{
		visualHandle_ = nullptr;
		meshVirtualCockpit0_ = nullptr;
		isCreated_ = false;
	}

    inline void vessel::clbkPostStep(double simt, double simdt, double mjd)
    {
        // Update animations
        for (auto& a : animations_)
        {
            a.second->Step(simdt);
            auto state = a.second->GetState();
            VESSEL3::SetAnimation(a.first, state);
        }

		// NEW MODE  << This will go away eventually
		if (oapiCockpitMode() == COCKPIT_VIRTUAL) {
			for (auto& va : map_vc_animations_) {
				auto newState = va.second->vc_step(simdt);
				VESSEL3::SetAnimation(va.first, newState);
			}

			auto mesh = GetVirtualCockpitMesh0();
			for (auto& vt : vc_texture_animations_) {
				vt->vc_step(mesh, simdt);
			}
		}

		if (oapiCockpitMode() == COCKPIT_PANELS) {
			auto mesh = GetpanelMeshHandle0();
			for (auto& pa : map_panel_animations_) {
				pa->panel_step(mesh, simdt);
			}
		}

		for (auto& ps : post_step_components_) {
			ps->handle_post_step(*this, simt, simdt, mjd);
		}
    }

	inline void vessel::clbkPostCreation()
	{
		isCreated_ = true;
	}

 	inline bool vessel::IsStoppedOrDocked()
	{
		vesselStatus_.flag = 0;
		GetStatusEx(&vesselStatus_);
		return ((vesselStatus_.status == 1) || (DockingStatus(0) == 1));
	}

	inline bool vessel::clbkLoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
	{
		for each (auto & p in map_panel_targets_) {	// For panel, mouse and redraw happen in the same call.
			RegisterPanelArea(
				hPanel,
				p.first,							// Area ID
				p.second->panel_rect(),
				p.second->panel_redraw_flags(),	// PANEL_REDRAW_*
				p.second->panel_mouse_flags());	// PANEL_MOUSE_*
		}

		for each (auto & vc in load_panel_components_) {
			vc->handle_load_panel(*this, id, hPanel);
		}

		return true;
	}

	inline bool vessel::clbkPanelRedrawEvent(int id, int event, SURFHANDLE surf, void* context)
	{
		// OLD New mode...
		auto c = idComponentMap_.find(id);
		if (c != idComponentMap_.end())
		{
			return c->second->OnPanelRedrawEvent(id, event, surf);
		}

		// NEW mode
		auto pe = map_panel_targets_.find(id);
		if (pe != map_panel_targets_.end()) {
			pe->second->on_panel_redraw(GetpanelMeshHandle0());
			return true;
		}

		auto pc = map_panel_component_.find(id);
		if (pc != map_panel_component_.end()) {
			pc->second->handle_redraw_panel(*this, id, event, surf);
		}

		return true;
	}

	inline bool vessel::clbkPanelMouseEvent(int id, int event, int mx, int my)
	{
		// Old New mode...
		auto c = idComponentMap_.find(id);
		if (c != idComponentMap_.end())
		{
			return c->second->OnPanelMouseEvent(id, event);
		}

		// NEW mode
		auto pe = map_panel_targets_.find(id);
		if (pe != map_panel_targets_.end()) {
			pe->second->on_event(id, event);
		}

		return true;
	}
}