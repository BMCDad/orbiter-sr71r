#pragma once
/*
	OnOffToggle:

	clbkSetClassCaps:
		- If is IVCAnimate	: call method to create that animation and add to vecVCAnimations
								AnimationGroup*
								StateController
								Speed
		- If is IExtAnimate	: call method to create that animation and add to vecExtAnimations
								AnimationGroup*
								StateController
								Speed
		- If is IVCTarget	: add to mapVCTargets
		- If is IPnlTarget	: add to mapPNLTargets
		- If is IVCRedraw	: add to mapVCRedraw
		- If is IPNLRedraw	: add to mapPNLRedraw
		- If is IStep		: add to vecStep
		- If is IInit		: call Init(BaseVessel& v)

	clbkLoadVC:
		- Loop through mapVCTargets and register mouse events.
								VECTOR3 location
								double radius
		- Loop through mapVCRedraw and register redraw events.
								(** no redraw for OnOffToggle, which is the prototype for this arch **)
		- Loop through members of vecVCAnimations and update direct state.

	clbkLoadPanel:
			(** for panel these are likely to be combined into one mouse/redraw call **)
		- Loop through mapPNLTargets and register mouse events.
								RECT location
		- Loop through mapPNLRedraw and register redraw events.
								RECT location

	clbkVCMouseEvent:
		- Find event in mapVCTargets and call OnEvent().

	clbkPanelMouseEvent:
		- Find event in mapPNLTargets and call OnEvent().

	clbkOnVCRedraw:
		- Find event in mapVCRedraw and call OnRedraw().

	clbkPanelRedrawEvent:
		- Find event in mapPNLRedraw and call OnRedraw().

	clbkPostStep:
		- Loop members of vecStep and call OnStep().
		- if ( VC Mode ) loop members of vecVCAnimations
		- if ( Ext Mode ) loop members of vecExtAnimations


	NonUI components
	IComponent:
	ITimeStep:
*/

namespace bc_orbiter
{
	class BaseVessel;
}


#include "OrbiterAPI.h"
#include "Animation.h"
#include "Tools.h"

#include <vector>
#include <functional>
#include <memory>

namespace bc_orbiter {
	/**
	* IVCAnimate
	* Implemented by a control that needs to take part in the VC cockpit animation step.
	*/
	struct IVCAnimate {
		virtual AnimationGroup* GetVCAnimationGroup() = 0;
		virtual IAnimationState* GetVCAnimationStateController() = 0;
		virtual double GetVCAnimationSpeed() const = 0;
	};

	struct IExtAnimate {};

	/**
	* IPNLAnimate
	* Implemented by a control that needs to animate as part of the panel animation step.
	* Remember:  Panel animations are just mesh transforms, and are not part of Orbiter animations.
	*/
	struct IPNLAnimate {
		virtual void PanelStep(MESHHANDLE mesh, double simdt) = 0;
	};

	struct ITarget {
		virtual bool OnEvent() { return false; }

//		virtual ~ITarget() = 0;
	};

	/**
	* IVCTarget
	* Implemented to indicate that a control must repond to user events in the VC.
	*/
	struct IVCTarget : public ITarget {
		virtual VECTOR3& GetVCEventLocation()			{ return _V(0.0, 0.0, 0.0); }
		virtual double	 GetVCEventRadius()				{ return 0.0; }
		virtual void	 OnVCRedraw(DEVMESHHANDLE meshVC)	{}
		virtual int		 GetVCMouseFlags()				{ return PANEL_MOUSE_IGNORE; }
		virtual int		 GetVCRedrawFlags()				{ return PANEL_REDRAW_NEVER; }
//		virtual ~IVCTarget() = 0;
	};

	/**
	* IPNLTarget
	* Implemented to indicate that a control responds to event from a 2D panel.
	*/
	struct IPNLTarget : public ITarget {
		virtual RECT&	 GetPanelRect()						{ return _R(0, 0, 0, 0); }
		virtual void	 OnPNLRedraw(MESHHANDLE meshPanel)	{}
		virtual int		 GetPanelMouseFlags()				{ return PANEL_MOUSE_IGNORE; }
		virtual int		 GetPanelRedrawFlags()				{ return PANEL_REDRAW_NEVER; }

		//		virtual ~IPNLTarget() = 0;
	};

	/**
	IInit
	Indicates the class participates in setClassCaps.  The class must implement the
	call void Init(BaseVessel&).
	*/
	struct IInit
	{
		virtual void Init(BaseVessel& vessel) = 0;
	};

	template <typename T>
	struct ISink {
		virtual void Notify(T value) = 0;
	};

	template<typename T>
	class Notify {
	public:
		void Subscribe(const std::function<void(const T&)>& sub) {
			subs_.emplace_back(sub);
		}

	protected:
		void Emit(T value) const {
			for (const auto& v : subs_) {
				v(value);
			}
		}

	private:
		std::vector<std::function<void(const T&)>> subs_;
	};

	template<typename T>
	class Slot {
	public:
		Slot(const std::function<void(T)> func) 
		:
			func_(func)
		{}

		virtual ~Slot(){}

		virtual void Notify(T value) {
			if (value != value_) {
				value_ = value;
				if(nullptr != func_) func_(value_);
			}
		}

		virtual T Value() const { return value_; }
	private:
		T value_{};
		const std::function<void(T)> func_{ nullptr };
	};

	template<typename T>
	class Signal {
	public:
		Signal() {}
		virtual ~Signal(){}

		void Attach(Slot<T>* s) {
			slot_ = s;
		}

		void Fire(T val) {
			if (dirty_ || (val != prevValue_)) {
				prevValue_ = val;
				dirty_ = true;
				if (nullptr != slot_) slot_->Notify(val);
			}
		}

		T Current() const { return prevValue_; }

	private:
		Slot<T>* slot_{ nullptr };
		T prevValue_{  };
		bool dirty_{ true };
	};

	template<typename TSignal, typename TSlot>
	class Connector {
	public:
		Connector(TSignal& sig, TSlot& slot)
			:
			signal_(sig),
			slot_(slot)
		{
			signal_.Attach(&slot_);
		}

	private:
		TSignal& signal_;
		TSlot& slot_;
	};

	/**
	* IControl
	* Defined a class that has, and can provide a control id.  All classes
	* that require a unique id must derive from this interface.
	*/
	struct IControl {
		virtual int GetID() const { return 0; }
	};

	/**
	* Base class for a control.  Implements IControl and provides a type for the value managed.
	*/
	class Control : public IControl {
	public:
		Control(int ctrlId) : ctrlId_(ctrlId) {}
		int GetID() const override { return ctrlId_; }

	private:
		const int ctrlId_;
	};

	template<typename T>
	class StateProvider : public Notify<T> {
	public:
		StateProvider() {}

		void SetState(T v) {
			Emit(v);
		}
	private:
	};

	////////////////////////

	struct ControlData {
		double animRotation;
		double animSpeed;
		double animStart;
		double animEnd;
		double hitRadius;
		double pnlOffset;
		int	vcRedrawFlags;
		int vcMouseFlags;
		int pnlRedrawFlags;
		int pnlMouseFlags;
	};


	// Status light - no animation, texture only
	class StatusLight : public Control, public IVCTarget, public IPNLTarget {
	public:
		StatusLight(
			int ctrlId,
			const UINT vcGroupId,
			const NTVERTEX* vcVerts,
			const UINT pnlGroupId,
			const NTVERTEX* pnlVerts,
			double offset) 
			:
			Control(ctrlId),
			vcGroupId_(vcGroupId),
			vcVerts_(vcVerts),
			pnlGroupId_(pnlGroupId),
			pnlVerts_(pnlVerts),
			offset_(offset),
			slotState_([&](double v) {
			auto bv = bool(v != 0.0);
			if (state_ != bv) {
				state_ = bv;
				oapiTriggerRedrawArea(0, 0, GetID());
			}})
		{
		}

		void OnVCRedraw(DEVMESHHANDLE vcMesh) override {
			NTVERTEX* delta = new NTVERTEX[4];

			TransformUV2d(
				vcVerts_,
				delta, 4,
				_V(state_ ? offset_ : 0.0,
					0.0,
					0.0),
				0.0);

			GROUPEDITSPEC change{};
			change.flags = GRPEDIT_VTXTEX;
			change.nVtx = 4;
			change.vIdx = NULL; //Just use the mesh order
			change.Vtx = delta;
			auto res = oapiEditMeshGroup(vcMesh, vcGroupId_, &change);
			delete[] delta;
		}
		
		void OnPNLRedraw(MESHHANDLE meshPanel) override {
			UpdateMesh(meshPanel, pnlGroupId_, pnlVerts_);
		}

		int GetVCMouseFlags()		{ return PANEL_MOUSE_IGNORE; }
		int GetVCRedrawFlags()		{ return PANEL_REDRAW_USER; }
		int GetPanelMouseFlags()	{ return PANEL_MOUSE_IGNORE; }
		int GetPanelRedrawFlags()	{ return PANEL_REDRAW_USER; }

		Slot<double>& SlotState()	{ return slotState_; }
	private:
		UINT					vcGroupId_;
		const NTVERTEX*			vcVerts_;
		UINT					pnlGroupId_;
		const NTVERTEX*			pnlVerts_;
		bool					state_{ false };
		double					offset_{ 0.0 };
		Slot<double>			slotState_;

		void UpdateMesh(MESHHANDLE mesh, UINT id, const NTVERTEX* verts) {
			DrawPanelOnOff(mesh, id, verts, state_, offset_);
		}
	};
}