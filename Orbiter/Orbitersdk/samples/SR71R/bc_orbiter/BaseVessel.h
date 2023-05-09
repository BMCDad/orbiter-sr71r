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
#include "EventTarget.h"
#include "Animation.h"
#include "IAnimationState.h"

#include <vector>
#include <map>
#include <memory>

namespace bc_orbiter
{
	class Component;
	class ITimeStep;
	class IVirtualCockpit;
}

namespace bc_orbiter
{
	/**
	Used by the RegisterVCEvent method calls.  Indicates the type of
	event to register.
	*/
	enum VCIdMode
	{
		None		= 0x000,	// All off
		MouseEvent	= 0x001,	// Mouse Event
		RedrawEvent	= 0x002,	// Redraw Event
		All			= MouseEvent | RedrawEvent
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

        /**
        */
        virtual int RegisterVCEvent(Component* comp, VCIdMode mode);

        /**	RegisterVCRedrawEvent
        Registers a handler for a VC redraw event.  An event id is created and returned
        to the caller.  That id can then be used to request a redraw.
        */
        virtual int RegisterVCRedrawEvent(Component* comp) { return RegisterVCEvent(comp, VCIdMode::RedrawEvent); }

        /**	RegisterVCMouseEvent
        Registers a handler for a VC mouse event.  An event id is created and returned
        to the caller.  When orbiter sees this id it will call the passed component to
        handle it.
        */
        virtual int RegisterVCMouseEvent(Component* comp) { return RegisterVCEvent(comp, VCIdMode::MouseEvent); }

        virtual void RegisterVCEventTarget(EventTarget* target) 
        {
            auto id = nextVCEventId_;
            nextVCEventId_++;

            vcEventTargetMap_[id] = target;
            target->SetEventId(id);
        }
    

		// clbk overrides:
		virtual void clbkLoadStateEx(FILEHANDLE scn, void *vs) override;
		virtual bool clbkLoadVC(int id) override;
		virtual void clbkSaveState(FILEHANDLE scn) override;
		virtual void clbkSetClassCaps(FILEHANDLE cfg) override;
		virtual bool clbkVCMouseEvent(int id, int event, VECTOR3 &p) override;
		virtual bool clbkVCRedrawEvent(int id, int event, SURFHANDLE surf) override;
		virtual void clbkVisualCreated(VISHANDLE visHandle, int refCount) override;
		virtual void clbkVisualDestroyed(VISHANDLE vis, int refcount) override;
        virtual void clbkPostStep(double simt, double simdt, double mjd) override;
		virtual void clbkPostCreation() override;

		virtual ~BaseVessel() {}

		DEVMESHHANDLE   GetVirtualCockpitMesh0()        { return meshVirtualCockpit0_; }
        MESHHANDLE      GetVCMeshHandle0()              { return vcMeshHandle0_; }
        
        void            SetVCMeshHandle0(MESHHANDLE mh) { vcMeshHandle0_    = mh; }
		void            SetVCMeshIndex0(int index)      { vcIndex0_         = index; }
        void            SetMainMeshIndex(int index)     { mainIndex_        = index; }

        UINT            GetVCMeshIndex() const          { return vcIndex0_; }
        UINT            GetMainMeshIndex() const        { return mainIndex_; }

		void			SetPanelMeshHandle0(MESHHANDLE mh) { panelMeshHandle0_ = mh; }
		MESHHANDLE		GetpanelMeshHandle0()			{ return panelMeshHandle0_; }

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
	private:
		bool IsModeSet(VCIdMode test, VCIdMode mode) const
		{
			return ((test & mode) == test);
		}

		std::vector<Component*>		vesselComponents_;
		std::map<int, Component*>	redrawHandlers;

		std::map<int, Component*>	vcRedrawMap_;
		std::map<int, Component*>	vcMouseEventMap_;

        std::map<int, EventTarget*> vcEventTargetMap_;

        std::map<UINT, std::unique_ptr<IAnimation>>      animations_;

		int		nextVCEventId_;

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
		nextVCEventId_(1),
		visualHandle_(nullptr),
		meshVirtualCockpit0_(nullptr),
		vcMeshHandle0_(nullptr)
	{
		// Init vessel status.
		memset(&vesselStatus_, 0, sizeof(vesselStatus_));
		vesselStatus_.version = 2;
	}

	inline void BaseVessel::RegisterComponent(Component* comp)
	{
		vesselComponents_.push_back(comp);
	}

	inline int BaseVessel::RegisterVCEvent(Component* comp, VCIdMode mode)
	{
		auto id = nextVCEventId_;
		nextVCEventId_++;

		if (IsModeSet(VCIdMode::MouseEvent, mode))	vcMouseEventMap_[id] = comp;
		if (IsModeSet(VCIdMode::RedrawEvent, mode))	vcRedrawMap_[id] = comp;

		return id;
	}

	inline void BaseVessel::clbkLoadStateEx(FILEHANDLE scn, void *vs)
	{
		char *line;

		while (oapiReadScenario_nextline(scn, line))
		{
			bool handled = false;

			for (auto &p : vesselComponents_)
			{
				if (p->LoadConfiguration(line, scn, line))
				{
					handled = true;
					break;
				}
			}

			if (!handled)
			{
				ParseScenarioLineEx(line, vs);
			}
		}
	}

	inline bool BaseVessel::clbkLoadVC(int id)
	{
		for (auto& p : vesselComponents_)	p->LoadVC(id);

        for (auto& et : vcEventTargetMap_)	et.second->RegisterMouseEvents();

		return true;
	}

	inline void BaseVessel::clbkSaveState(FILEHANDLE scn)
	{
		VESSEL3::clbkSaveState(scn);	// Save default state.

		for (auto &p : vesselComponents_)	p->SaveConfiguration(scn);
	}

	inline void BaseVessel::clbkSetClassCaps(FILEHANDLE cfg)
	{
		for (auto &p : vesselComponents_)	p->SetClassCaps();
	}

	inline bool BaseVessel::clbkVCMouseEvent(int id, int event, VECTOR3 &p)
	{
		auto eh = vcMouseEventMap_.find(id);
		if (eh != vcMouseEventMap_.end())	return eh->second->MouseEvent(id, event);

        auto el = vcEventTargetMap_.find(id);
        if (el != vcEventTargetMap_.end())	return el->second->HandleMouse(event);

		return false;
	}

	inline bool BaseVessel::clbkVCRedrawEvent(int id, int event, SURFHANDLE surf)
	{
		if (nullptr == meshVirtualCockpit0_)	return false;

		auto eh = vcRedrawMap_.find(id);
		if (eh != vcRedrawMap_.end())		return eh->second->VCRedrawEvent(id, event, surf);

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

}