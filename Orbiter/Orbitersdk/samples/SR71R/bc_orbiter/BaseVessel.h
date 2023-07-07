//	BaseVessel - bco Orbiter Library
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
#include "Orbitersdk.h"
#include "Component.h"
#include "Animation.h"
#include "IAnimationState.h"

#include "Control.h"
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
	class ITimeStep;
	class IVirtualCockpit;
}

namespace bc_orbiter
{
	struct vessel_data {
		BaseVessel* vessel;
		
		bool operator!=(const vessel_data& other) const {
			return (vessel != other.vessel);
		}
	};

	struct draw_hud_data : public vessel_data {
		int					mode;
		const HUDPAINTSPEC*	paintSpec;
		oapi::Sketchpad*	sketchPad;
	};

	struct navmode_data {
		int	mode;
		bool active;

		bool operator!=(const navmode_data& other) const {
			return ((mode != other.mode) || (active != other.active));
		}
	};

	/**
	Base class for Orbiter vessels.
	*/
    class BaseVessel : public VESSEL4
    {
    public:
        BaseVessel(OBJHANDLE hvessel, int flightmodel);

        /**	RegisterComponent
        Adds a component to the base class component list.
        */
        virtual void RegisterComponent(Component* comp);

		// clbk overrides:
		virtual bool clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;
		virtual bool clbkLoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH) override;
//		virtual void clbkLoadStateEx(FILEHANDLE scn, void *vs) override;
		virtual bool clbkLoadVC(int id) override;
		virtual bool clbkPanelMouseEvent(int id, int event, int mx, int my) override;
		virtual bool clbkPanelRedrawEvent(int id, int event, SURFHANDLE surf, void* context) override;
		virtual void clbkPostCreation() override;
        virtual void clbkPostStep(double simt, double simdt, double mjd) override;
		//virtual void clbkSaveState(FILEHANDLE scn) override;
		virtual void clbkSetClassCaps(FILEHANDLE cfg) override;
		virtual bool clbkVCMouseEvent(int id, int event, VECTOR3 &p) override;
		virtual bool clbkVCRedrawEvent(int id, int event, SURFHANDLE surf) override;
		virtual void clbkVisualCreated(VISHANDLE visHandle, int refCount) override;
		virtual void clbkVisualDestroyed(VISHANDLE vis, int refcount) override;

		virtual ~BaseVessel() {}

		DEVMESHHANDLE   GetVirtualCockpitMesh0()        { return meshVirtualCockpit0_; }
        MESHHANDLE      GetVCMeshHandle0()              { return vcMeshHandle0_; }
        
        void            SetVCMeshHandle0(MESHHANDLE mh) { vcMeshHandle0_    = mh; }
		void            SetVCMeshIndex0(int index)      { vcIndex0_         = index; }
        void            SetMainMeshIndex(int index)     { mainIndex_        = index; }

        UINT            GetVCMeshIndex() const          { return vcIndex0_; }
        UINT            GetMainMeshIndex() const        { return mainIndex_; }

		void			SetPanelMeshHandle0(MESHHANDLE mh) { panelMeshHandle0_ = mh; }
		MESHHANDLE		GetpanelMeshHandle0() const			{ return panelMeshHandle0_; }

		void UpdateUIArea(int area) { TriggerRedrawArea(0, 0, area); }

		bool IsStoppedOrDocked();

		bool IsCreated() { return isCreated_; }

        void CreateMainPropellant(double max)   { mainPropellant_ = CreatePropellantResource(max); }
        void CreateRcsPropellant(double max)    { rcsPropellant_ = CreatePropellantResource(max); }

        PROPELLANT_HANDLE MainPropellant()  const { return mainPropellant_; }
        PROPELLANT_HANDLE RcsPropellant()   const { return rcsPropellant_; }

        /**
        Creates an animation and registers it with the base vessel.  Animation registered
		with the base class automatically receive part of the PostStep time processing.
        @param target IAnimationState object that will control the animation state.
        @param speed The speed of the animation.
        @param func The function to call when the animation hits its target state.
        */
        template<typename AT=Animation>
        UINT CreateVesselAnimation(IAnimationState* target, double speed = 1.0, TargetAchievedFunc func = nullptr)
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
            AnimationGroup* transform,
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
        void SetAnimationState(UINT id, double state)
        {
            auto eh = animations_.find(id);
            if (eh != animations_.end())	eh->second->SetState(state);
        }

		int GetIdForComponent(Component* comp)
		{
			auto id = ++nextEventId_;
			idComponentMap_[id] = comp;
			return id;
		}

		int GetControlId() { return ++nextEventId_; }

		void AddControl(control* ctrl) { controls_.push_back(ctrl); }

		void AddComponent(vessel_component* c) { components_.push_back(c); }

		// Callback signals.
		signal<draw_hud_data>&		DrawHudSignal()		{ return hudSignal_; }
	private:
		// sigs
		signal<draw_hud_data>	hudSignal_;


		void HandleClassCaps()
		{
			for each (auto& vc in controls_)
			{
				if (auto* c = dynamic_cast<vc_animation*>(vc)) {
					//auto aid = CreateVCAnimation(c->vc_animation_state(), c->vc_animation_speed());
					//auto tr = c->vc_animation_group();
					//AddVCAnimationComponent(aid, GetVCMeshIndex(), tr);
					auto aid = VESSEL3::CreateAnimation(0);
//					AddVCAnimationComponent(aid, GetVCMeshIndex(), c->vc_animation_group());
					auto trans = c->vc_animation_group();
					trans->transform_->mesh = GetVCMeshIndex();
					VESSEL3::AddAnimationComponent(
						aid,
						trans->start_,
						trans->stop_,
						trans->transform_.get());

					mapVCAnimations_[aid] = c;
				}

				if (auto* c = dynamic_cast<panel_animation*>(vc)) {
					vecPNLAnimations_.push_back(c);
				}

				if (auto* c = dynamic_cast<vc_event_target*>(vc)) {
					mapVCTargets_[vc->get_id()] = c;
				}

				if (auto* c = dynamic_cast<panel_event_target*>(vc)) {
					mapPNLTargets_[vc->get_id()] = c;
				}

				if (auto* c = dynamic_cast<vc_tex_animation*>(vc)) {
					vecVCTexAnimations_.push_back(c);
				}
			}

			// set_class_caps will flesh out to a more general 'component' list (non-ui/control intities)
			for each (auto & cc in components_) {
				if (auto* ac = dynamic_cast<post_step*>(cc)) comp_post_step_.push_back(ac);

				if (auto* ac = dynamic_cast<set_class_caps*>(cc)) comp_set_class_caps_.push_back(ac);

				if (auto* ac = dynamic_cast<draw_hud*>(cc)) comp_draw_hud_.push_back(ac);
			}

			for each (auto & sc in comp_set_class_caps_) {
				sc->handle_set_class_caps(*this);
			}
		}

		void HandleLoadVC()
		{
			for each (auto & vc in mapVCTargets_) {
				oapiVCRegisterArea(
					vc.first,							// Area ID
					vc.second->vc_redraw_flags(),		// PANEL_REDRAW_*
					vc.second->vc_mouse_flags());		// PANEL_MOUSE_*

				oapiVCSetAreaClickmode_Spherical(
					vc.first,							// Area ID
					vc.second->vc_event_location(), 
					vc.second->vc_event_radius());
			}
			
			// TODO: vcRedraw
			// TODO: vc Reset animations to current state.
		}

		void HandleLoadPanel(PANELHANDLE hPanel)
		{
			for each (auto & p in mapPNLTargets_) {	// For panel, mouse and redraw happen in the same call.
				RegisterPanelArea(
					hPanel, 
					p.first,							// Area ID
					p.second->panel_rect(), 
					p.second->panel_redraw_flags(),	// PANEL_REDRAW_*
					p.second->panel_mouse_flags());	// PANEL_MOUSE_*
			}
		}

std::map<UINT, std::unique_ptr<IAnimation>>     vcAnimations_;
std::map<int, vc_event_target*>					mapVCTargets_;
std::map<int, panel_event_target*>				mapPNLTargets_;
std::vector<panel_animation*>					vecPNLAnimations_;
std::vector<vc_tex_animation*>					vecVCTexAnimations_;
std::map<int, vc_animation*>					mapVCAnimations_;

std::vector<vessel_component*>					components_;
std::vector<post_step*>							comp_post_step_;
std::vector<set_class_caps*>					comp_set_class_caps_;
std::vector<draw_hud*>							comp_draw_hud_;

//**** END NEW STYLE

std::vector<control*>			controls_;

	private:
		std::vector<Component*>		vesselComponents_;

		// Manage components registering for redraw events.  We maintain a separate
		// map for VC and Panels.
		std::map<int, Component*>	vcRedrawMap_;
		std::map<int, Component*>	panelRedrawMap_;

		std::map<int, Component*>	vcMouseEventMap_;

		//        std::map<int, EventTarget*> vcEventTargetMap_;
		//		std::map<int, PanelEventTarget*> pnlEventTargetMap_;

				// Map a control id to its component so we know who to call.
		std::map<int, Component*>	idComponentMap_;

		std::map<UINT, std::unique_ptr<IAnimation>>      animations_;

		int		nextEventId_{ 0 };

		bool isCreated_{ false };	// Set true after clbkPostCreation

		VISHANDLE			visualHandle_;
		DEVMESHHANDLE		meshVirtualCockpit0_;
		MESHHANDLE			vcMeshHandle0_;
		MESHHANDLE			panelMeshHandle0_{ nullptr };
		UINT				vcIndex0_;
		UINT                mainIndex_;
		VESSELSTATUS2		vesselStatus_;

		// Propellent (multiple components need this on setup, so put it in the vessel class)
		PROPELLANT_HANDLE	    mainPropellant_;
		PROPELLANT_HANDLE	    rcsPropellant_;
	};

	inline BaseVessel::BaseVessel(OBJHANDLE hvessel, int flightmodel) :
		VESSEL4(hvessel, flightmodel),
		visualHandle_(nullptr),
		meshVirtualCockpit0_(nullptr),
		vcMeshHandle0_(nullptr)
	{
		// handle_set_class_caps vessel status.
		memset(&vesselStatus_, 0, sizeof(vesselStatus_));
		vesselStatus_.version = 2;
	}

	inline void BaseVessel::RegisterComponent(Component* comp)
	{
		vesselComponents_.push_back(comp);
		auto redrawId = ++nextEventId_;
		vcRedrawMap_[redrawId] = comp;
		panelRedrawMap_[redrawId] = comp;
		comp->SetRedrawId(redrawId);
	}

	inline bool BaseVessel::clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
	{
		hudSignal_.fire({ this, mode, hps, skp });
		//for (auto& ps : comp_draw_hud_) {
		//	ps->handle_draw_hud(*this, mode, hps, skp);
		//}
		return true;
	}

	//inline void BaseVessel::clbkLoadStateEx(FILEHANDLE scn, void* vs)
	//{
	//	char* line;

	//	while (oapiReadScenario_nextline(scn, line))
	//	{
	//		bool handled = false;

	//		for (auto& p : vesselComponents_)
	//		{
	//			if (p->OnLoadConfiguration(line, scn, line))
	//			{
	//				handled = true;
	//				break;
	//			}
	//		}
	//		
	//		if (!handled) {
	//			ParseScenarioLineEx(line, vs);
	//		}
	//	}
	//}

	inline bool BaseVessel::clbkLoadVC(int id)
	{
		for (auto& p : vesselComponents_)	p->OnLoadVC(id);

//        for (auto& et : vcEventTargetMap_)	et.second->RegisterMouseEvents();

		HandleLoadVC();
		return true;
	}

	//inline void BaseVessel::clbkSaveState(FILEHANDLE scn)
	//{
	//	VESSEL3::clbkSaveState(scn);	// Save default state.

	//	for (auto &p : vesselComponents_)	p->OnSaveConfiguration(scn);

	//	for (auto& p : comp_manage_state_) {
	//		std::string key;
	//		std::string config;
	//		p->handle_save(key, config);

	//		oapiWriteScenario_string(scn, (char*)key.c_str(), (char*)(config.c_str()));
	//	}
	//}

	inline void BaseVessel::clbkSetClassCaps(FILEHANDLE cfg)
	{
		for (auto &p : vesselComponents_)	p->OnSetClassCaps();

		// ** New stuff:
		HandleClassCaps();
	}

	inline bool BaseVessel::clbkVCMouseEvent(int id, int event, VECTOR3 &p)
	{
		auto eh = vcMouseEventMap_.find(id);
		if (eh != vcMouseEventMap_.end())	return eh->second->OnVCMouseEvent(id, event);

        //auto el = vcEventTargetMap_.find(id);
        //if (el != vcEventTargetMap_.end())	return el->second->HandleMouse(event);

		// Old new mode...
		auto c = idComponentMap_.find(id);
		if (c != idComponentMap_.end())
		{
			return c->second->OnVCMouseEvent(id, event);
		}

		// NEW mode
		auto vc = mapVCTargets_.find(id);
		if (vc != mapVCTargets_.end()) {
			vc->second->on_event();
		}

		return false;
	}

	inline bool BaseVessel::clbkVCRedrawEvent(int id, int event, SURFHANDLE surf)
	{
		if (nullptr == meshVirtualCockpit0_)	return false;

		auto eh = vcRedrawMap_.find(id);
		if (eh != vcRedrawMap_.end())
		{
			return eh->second->OnVCRedrawEvent(id, event, surf);
		}
		else
		{
			// New mode...
			auto c = idComponentMap_.find(id);
			if (c != idComponentMap_.end())
			{
				return c->second->OnVCRedrawEvent(id, event, surf);
			}
		}

		// NEW mode
		auto pe = mapVCTargets_.find(id);
		if (pe != mapVCTargets_.end()) {
			pe->second->on_vc_redraw(meshVirtualCockpit0_);
		}

		return false;
	}

	inline void BaseVessel::clbkVisualCreated(VISHANDLE visHandle, int refCount)
	{
		visualHandle_ = visHandle;
		meshVirtualCockpit0_ = GetDevMesh(visualHandle_, vcIndex0_);
	}

	inline void BaseVessel::clbkVisualDestroyed(VISHANDLE vis, int refcount)
	{
		visualHandle_ = nullptr;
		meshVirtualCockpit0_ = nullptr;
		isCreated_ = false;
	}

    inline void BaseVessel::clbkPostStep(double simt, double simdt, double mjd)
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
			for (auto& va : mapVCAnimations_) {
				//va.second->Step(simdt);
				//auto state = va.second->GetState();
				//VESSEL3::SetAnimation(va.first, state);
				auto newState = va.second->vc_step(simdt);
				VESSEL3::SetAnimation(va.first, newState);
			}

			auto mesh = GetVirtualCockpitMesh0();
			for (auto& vt : vecVCTexAnimations_) {
				vt->vc_step(mesh, simdt);
			}
		}

		if (oapiCockpitMode() == COCKPIT_PANELS) {
			auto mesh = GetpanelMeshHandle0();
			for (auto& pa : vecPNLAnimations_) {
				pa->panel_step(mesh, simdt);
			}
		}

		for (auto& ps : comp_post_step_) {
			ps->handle_post_step(*this, simt, simdt, mjd);
		}
    }

	inline void BaseVessel::clbkPostCreation()
	{
		isCreated_ = true;
	}

 	inline bool BaseVessel::IsStoppedOrDocked()
	{
		vesselStatus_.flag = 0;
		GetStatusEx(&vesselStatus_);
		return ((vesselStatus_.status == 1) || (DockingStatus(0) == 1));
	}

	inline bool BaseVessel::clbkLoadPanel2D(int id, PANELHANDLE hPanel, DWORD viewW, DWORD viewH)
	{
		//for (auto& et : pnlEventTargetMap_)
		//{
		//	et.second->OnLoad(id, hPanel, (VESSEL4*)this);
		//	et.second->RegisterMouseEvents();
		//}

		for (auto& p : vesselComponents_)	p->OnLoadPanel2D(id, hPanel);

		//*** NEW
		HandleLoadPanel(hPanel);

		return true;
	}

	inline bool BaseVessel::clbkPanelRedrawEvent(int id, int event, SURFHANDLE surf, void* context)
	{
		//auto eh = panelRedrawTarget_.find(id);
		//if (eh != panelRedrawTarget_.end())
		//{
		//	eh->second->Draw(GetpanelMeshHandle0());
		//	return true;
		//}
		auto eh = panelRedrawMap_.find(id);
		if (eh != panelRedrawMap_.end())		return eh->second->OnPanelRedrawEvent(id, event, surf);

		// OLD New mode...
		auto c = idComponentMap_.find(id);
		if (c != idComponentMap_.end())
		{
			return c->second->OnPanelRedrawEvent(id, event, surf);
		}

		// NEW mode
		auto pe = mapPNLTargets_.find(id);
		if (pe != mapPNLTargets_.end()) {
			pe->second->on_panel_redraw(GetpanelMeshHandle0());
		}

		return true;
	}

	inline bool BaseVessel::clbkPanelMouseEvent(int id, int event, int mx, int my)
	{
		//auto el = pnlEventTargetMap_.find(id);
		//if (el != pnlEventTargetMap_.end())	return el->second->HandleMouse(event);

		// Old New mode...
		auto c = idComponentMap_.find(id);
		if (c != idComponentMap_.end())
		{
			return c->second->OnPanelMouseEvent(id, event);
		}

		// NEW mode
		auto pe = mapPNLTargets_.find(id);
		if (pe != mapPNLTargets_.end()) {
			pe->second->on_event();
		}

		return true;
	}
}