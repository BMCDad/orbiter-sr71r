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
#include "signals.h"

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
		virtual void notify(T value) = 0;
	};

	template<typename T>
	class notify {
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

	/**
	* Base class for a control.
	*/
	class control {// : public IControl {
	public:
		control(int ctrlId) : ctrlId_(ctrlId) {}
		virtual int get_id() const { return ctrlId_; }

	private:
		const int ctrlId_;
	};

}