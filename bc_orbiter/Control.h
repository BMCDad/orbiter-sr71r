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
	class vessel;
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
	this base class instructs vessel to add it to a collection that will be called during
	the VC step.
	*/
	struct vc_animation {
		virtual animation_group*		vc_animation_group() = 0;
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
        virtual int panel_id() = 0;
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
        virtual bool on_event(int id, int event) { return false; }
    };

    /**
    vc_event_target
    Implemented to indicate that a VC control is a target for either mouse events, or redraw
    events, or both.  If you need mouse events, override vc_mouse_flags to enable mouse events.
    For redraw events, override vc_redraw_flags.
    */
    struct vc_event_target : public event_target {
        virtual VECTOR3     vc_event_location()                 { return _V(0.0, 0.0, 0.0); }
        virtual double      vc_event_radius()                   { return 0.0; }
        virtual int         vc_mouse_flags()                    { return PANEL_MOUSE_IGNORE; }
        virtual int         vc_redraw_flags()                   { return PANEL_REDRAW_NEVER; }
        virtual void        on_vc_redraw(DEVMESHHANDLE meshVC)  {}
        virtual int         vc_id()                             { return 0; }
    };

    /**
    panel_event_target
    Implemented to indicate that a panel control is a target for either mouse events, or redraw
    events, or both.  If you need mouse events, override vc_mouse_flags to enable mouse events.
    For redraw events, override vc_redraw_flags.
    */
    struct panel_event_target : public event_target {
        virtual RECT        panel_rect()                            { return _R(0, 0, 0, 0); }
        virtual int         panel_mouse_flags()                     { return PANEL_MOUSE_IGNORE; }
        virtual int         panel_redraw_flags()                    { return PANEL_REDRAW_NEVER; }
        virtual void        on_panel_redraw(MESHHANDLE meshPanel)   {}
        virtual int         panel_id()                              { return 0; }
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

	/**
	consumable
	Implemented but a component that manages a consumable that can be drawn from
	by other components.
	*/
	struct consumable {
		// Current level espressed a 0-empty, 1-full.
		virtual double level() const = 0;
		
		// Removes 'amount' from the tank, returns the amount actually drawn.
		virtual double draw(double amount) = 0;
	};

	struct hydraulic_provider {
		virtual double level() const = 0;
	};

	struct avionics_provider {
		virtual double get_altitude() const = 0;
		virtual void   get_angular_velocity(VECTOR3& v) = 0;
		virtual double get_bank() const = 0;
		virtual double get_heading() const = 0;
		virtual double get_keas() const = 0;
		virtual double get_mach() const = 0;
		virtual double get_pitch() const = 0;
		virtual double get_vertical_speed() const = 0;
	};

	enum class Axis { Pitch = 0, Yaw = 1, Roll = 2 };

	struct propulsion_control {
		virtual double get_main_thrust_level() const = 0;
		virtual void set_main_thrust_level(double l) = 0;
		virtual void set_attitude_rotation(Axis axit, double level) = 0;
	};

	struct surface_control {
		virtual void set_aileron_level(double l) = 0;
		virtual void set_elevator_level(double l) = 0;
	};

    /**
    * Base class for a control.
    */
    class control {
    public:
        control(int ctrlId) : ctrlId_(ctrlId) {}
        control() : ctrlId_(-1) {}

        virtual void set_id(int id) { ctrlId_ = id; }
        virtual int get_id() const { return ctrlId_; }

    private:
        int ctrlId_;
    };

    using funcEvent = std::function<void()>;
    using funcState = std::function<double()>;

    template<typename T>
    class display_control_base :
        public control,
        public T
    {
    public:
        display_control_base(
            const UINT pnlGroupId,
            const NTVERTEX* pnlVerts,
            const int panelId
        ) : T(
            pnlGroupId,
            pnlVerts,
            panelId,
            [&] {return state_; }
        )
        {
        }

        void set_state(VESSEL4& vessel, bool s) {
            if (set_state(s ? 1.0 : 0.0))
                T::trigger_redraw(vessel, get_id());
        }

        void set_state(VESSEL4& vessel, double s) {
            if (set_state(s))
                T::trigger_redraw(vessel, get_id());
        }

        void set_state(VESSEL4& vessel, int s) {
            if (set_state((double)s))
                T::trigger_redraw(vessel, get_id());
        }

    protected:

        /// <summary>
        /// Set the state of the control.
        /// </summary>
        /// <param name="state">New state</param>
        /// <returns>True if the state changed</returns>
        bool set_state(double state) {
            if (state_ != state) {
                state_ = state;
                return true;
            }
            state_ = state;
            return false;
        }

    private:
        double  state_{ 0.0 };
    };
}