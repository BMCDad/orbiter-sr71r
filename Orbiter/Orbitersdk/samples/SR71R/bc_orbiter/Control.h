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

	struct IVCTarget : public ITarget {
		virtual VECTOR3& GetVCEventLocation() = 0;
		virtual double GetVCEventRadius() = 0;

		virtual void OnVCRedraw() {}

//		virtual ~IVCTarget() = 0;
	};

	struct IPNLTarget : public ITarget {
		virtual RECT& GetPanelRect() = 0;

		virtual void OnPNLRedraw(MESHHANDLE meshPanel) {}

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
		void Subscribe(std::function<void(T)> sub) { subs_.push_back(sub); }

	protected:
		void Emit(T value) {
			for each (auto & v in subs_) v(value);
		}

	private:
		std::vector<std::function<void(T)>> subs_;
	};

	struct IControl {
		virtual int GetID() const = 0;
	};

	template<typename T>
	class Control : public IControl, public Notify<T> {
	public:
		Control(int ctrlId) : ctrlId_(ctrlId)
		{ }

		int GetID() const override { return ctrlId_; }
	private:
		int ctrlId_{ 0 };
	};

	class StateProvider : public Notify<double> {
	public:
		StateProvider() {}

		void SetState(double v) {
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
	};

}