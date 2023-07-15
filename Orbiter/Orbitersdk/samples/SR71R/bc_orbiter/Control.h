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
	vc_animation
	Implemented by a control that needs to take part in the VC cockpit animation step.
	The implementing class will provide the group to animate, as well as the speed.  It
	will also implement the actual step that will update the animation.  The presence of 
	this base class instructs BaseVessel to add it to a collection that will be called during
	the VC step.
	*/
	struct vc_animation {
		virtual AnimationGroup*		vc_animation_group() = 0;
		virtual double				vc_animation_speed() const = 0;
		virtual double				vc_step(double simdt) { return 0.0; }
	};

	/**
	panel_animation
	Implemented by a control that needs to animate as part of the panel animation step.
	Remember:  Panel animations are just mesh transforms, and are not part of Orbiter animations.
	*/
	struct panel_animation {
		virtual void panel_step(MESHHANDLE mesh, double simdt) = 0;
	};

	/**
	* vc_tex_animation
	* Similar to panel_animation where the animation is a texture, and not a mesh group.  vc_animation
	* animates a group.  Better naming is needed to avoid confusion.  Note that panels use MESHHANDLE where
	* VCs use DEVMESHHANDLEs.
	*/
	struct vc_tex_animation {
		virtual void vc_step(DEVMESHHANDLE mesh, double simdt) = 0;
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
		virtual RECT&		panel_rect()						{ return _R(0, 0, 0, 0); }
		virtual int			panel_mouse_flags()					{ return PANEL_MOUSE_IGNORE; }
		virtual int			panel_redraw_flags()				{ return PANEL_REDRAW_NEVER; }
		virtual void		on_panel_redraw(MESHHANDLE meshPanel) {}
	};

	struct vessel_component
	{
	public:
		virtual ~vessel_component() = default;
	};

	/**
	set_class_caps
	Indicates the class participates in setClassCaps.  The class must implement the
	call void handle_set_class_caps(BaseVessel&).
	*/
	struct set_class_caps {
		virtual void handle_set_class_caps(BaseVessel& vessel) = 0;
		virtual ~set_class_caps() {};
	};

	/**
	post_step
	Indicates the class participates in postStep callbacks.  The class must implement
	the post step handler.
	*/
	struct post_step {
		virtual void handle_post_step(BaseVessel& vessel, double simt, double simdt, double mjd) = 0;
	};

	/**
	draw_hud
	Indicates the class participates in clbkDrawHud.  The class must implement the call
	handle_draw_hud(BaseVessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp).
	*/
	struct draw_hud {
		virtual void handle_draw_hud(BaseVessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) = 0;
	};

	/**
	manage_state
	Indicates the class can provide its internal state as a string, and that it can take that string back
	in order to set the internal state.  The string provided should be appropriate for use in an Orbiter
	config file, but does not provide the key, which will be provided by whatever is actually managing the config.
	*/
	struct manage_state {
		/**
		handle_load_state
		param line A single line of text representing the state.
		return true if the state was succesfully restored.
		*/
		virtual bool handle_load_state(const std::string& line) = 0;

		/**
		handle_save
		Return a single string that represents the internal state of the component.
				*/
		virtual std::string handle_save_state() = 0;
	};

	/**
	load_vc
	Indicates the class has special handling when loading a virtual cockpit.
	*/
	struct load_vc {
		/**
		handle_load_vc
		Do work required to setup vc components.
		*/
		virtual bool handle_load_vc(BaseVessel& vessel, int vcid) = 0;
	};


	struct power_consumer {
		virtual void on_change(double v) { };  // A class that has a time step may not need change notification.
		virtual double amp_draw() const = 0;
	};

	struct power_provider {
		virtual void attach_consumer(power_consumer* consumer) = 0;
		virtual double volts_available() const = 0;
		virtual double amp_load() const = 0;
	};

	struct one_way_switch {
		virtual bool is_on() const = 0;
		virtual void attach_on_change(const std::function<void()>& func) = 0;
	};

	struct consumable {
		virtual double level() const = 0;
		virtual double draw(double amount) = 0;
	};

	/**
	* Base class for a control.
	*/
	class control {// : public IControl {
	public:
		control(int ctrlId) : ctrlId_(ctrlId) {}
		control() : ctrlId_(-1) {}

		virtual void set_id(int id) { ctrlId_ = id; }
		virtual int get_id() const { return ctrlId_; }

	private:
		int ctrlId_;
	};

}