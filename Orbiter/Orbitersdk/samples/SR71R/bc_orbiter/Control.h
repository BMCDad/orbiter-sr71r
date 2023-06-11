//	controls - bco Orbiter Library
//	Copyright(C) 2023  Blake Christensen
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
	* vc_animation
	* Implemented by a control that needs to take part in the VC cockpit animation step.
	*/
	struct vc_animation {
		virtual AnimationGroup*		vc_animation_group() = 0;
		virtual IAnimationState*	vc_animation_state() = 0;
		virtual double				vc_animation_speed() const = 0;
	};

	/**
	* panel_animation
	* Implemented by a control that needs to animate as part of the panel animation step.
	* Remember:  Panel animations are just mesh transforms, and are not part of Orbiter animations.
	*/
	struct panel_animation {
		virtual void panel_step(MESHHANDLE mesh, double simdt) = 0;
	};

	/**
	* event_target
	* Base class for any control that will be an event target.
	*/
	struct event_target {
		virtual bool on_event() { return false; }
	};

	/**
	vc_event_target
	Implemented to indicate that a VC control is a target for either mouse events, or redraw
	events, or both.  If you need mouse events, override vc_mouse_flags to enable mouse events.
	For redraw events, override vc_redraw_flags.
	*/
	struct vc_event_target : public event_target {
		virtual VECTOR3&	vc_event_location()					{ return _V(0.0, 0.0, 0.0); }
		virtual double		vc_event_radius()					{ return 0.0; }
		virtual int			vc_mouse_flags()					{ return PANEL_MOUSE_IGNORE; }
		virtual int			vc_redraw_flags()					{ return PANEL_REDRAW_NEVER; }
		virtual void		on_vc_redraw(DEVMESHHANDLE meshVC)	{}
	};

	/**
	panel_event_target
	Implemented to indicate that a panel control is a target for either mouse events, or redraw
	events, or both.  If you need mouse events, override vc_mouse_flags to enable mouse events.
	For redraw events, override vc_redraw_flags.
	*/
	struct panel_event_target : public event_target {
		virtual RECT&		panel_rect()							{ return _R(0, 0, 0, 0); }
		virtual int			panel_mouse_flags()						{ return PANEL_MOUSE_IGNORE; }
		virtual int			panel_redraw_flags()					{ return PANEL_REDRAW_NEVER; }
		virtual void		on_panel_redraw(MESHHANDLE meshPanel)	{}
	};

	/**
	set_class_caps
	Indicates the class participates in setClassCaps.  The class must implement the
	call void handle_set_class_caps(BaseVessel&).
	*/
	struct set_class_caps
	{
		virtual void handle_set_class_caps(BaseVessel& vessel) = 0;
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