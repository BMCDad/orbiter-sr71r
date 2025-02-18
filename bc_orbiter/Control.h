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
	class Vessel;
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
    VCAnimation
    Implemented by a Control that needs to take part in the VC cockpit Animation step.
    The implementing class will provide the group to animate, as well as the speed.  It
    will also implement the actual step that will update the Animation.  The presence of 
    this base class instructs Vessel to add it to a collection that will be called during
    the VC step.
    */
    struct VCAnimation {
        virtual ~VCAnimation() = default;
        virtual AnimationGroup* VCAnimationGroup() = 0;
        virtual double          VCAnimationSpeed() const = 0;
        virtual double          VCStep(double simdt) { return 0.0; }
    };

    /**
    PanelAnimation
    Implemented by a Control that needs to animate as part of the panel Animation step.
    Remember:  Panel animations are just mesh transforms, and are not part of Orbiter animations.
    */
    struct PanelAnimation {
        virtual void    PanelStep(MESHHANDLE mesh, double simdt) = 0;
        virtual int     PanelID() = 0;
    };

    /**
    * VCTexAnimation
    * Similar to PanelAnimation where the Animation is a texture, and not a mesh group.  VCAnimation
    * animates a group.  Better naming is needed to avoid confusion.  Note that panels use MESHHANDLE where
    * VCs use DEVMESHHANDLEs.
    */
    struct VCTexAnimation {
        virtual void    VCStep(DEVMESHHANDLE mesh, double simdt) = 0;
    };

    /**
    * MouseEventTarget
    * Base class for a Control that will be the target of a mouse event.
    */
    struct MouseEventTarget {
        virtual ~MouseEventTarget() = default;
        virtual bool OnMouseClick(int id, int event) = 0;
    };

    /**
    VCEventTarget
    Implemented to indicate that a VC Control is a target for either mouse events, or redraw
    events, or both.  If you need mouse events, override VCMouseFlags to enable mouse events.
    For redraw events, override VCRedrawFlags.
    */
    struct VCEventTarget : public MouseEventTarget {
        virtual             ~VCEventTarget() = default;
        virtual VECTOR3     VCEventLocation()                   { return _V(0.0, 0.0, 0.0); }
        virtual double      VCEventRadius()                     { return 0.0; }
        virtual int         VCMouseFlags()                      { return PANEL_MOUSE_IGNORE; }
        virtual int         VCRedrawFlags()                     { return PANEL_REDRAW_NEVER; }
        virtual void        OnVCRedraw(DEVMESHHANDLE meshVC)    {}
        virtual int         VCId()                              { return 0; }
        virtual bool        OnMouseClick(int id, int event) override { return false; }
    };

    /**
    PanelEventTarget
    Implemented to indicate that a panel Control is a target for either mouse events, or redraw
    events, or both.  If you need mouse events, override VCMouseFlags to enable mouse events.
    For redraw events, override VCRedrawFlags.
    */
    struct PanelEventTarget : public MouseEventTarget {
        virtual             ~PanelEventTarget() = default;
        virtual RECT        PanelRect()                         { return _R(0, 0, 0, 0); }
        virtual int         PanelMouseFlags()                   { return PANEL_MOUSE_IGNORE; }
        virtual int         PanelRedrawFlags()                  { return PANEL_REDRAW_NEVER; }
        virtual void        OnPanelRedraw(MESHHANDLE meshPanel) {}
        virtual int         PanelId()                           { return 0; }
        virtual bool        OnMouseClick(int id, int event) override { return false; }
    };



    struct PowerConsumer {
        virtual void OnChange(double v) {};  // A class that has a time step may not need change notification.
        virtual double AmpDraw() const = 0;
    };

    struct PowerProvider {
        virtual void AttachConsumer(PowerConsumer* consumer) = 0;
        virtual double VoltsAvailable() const = 0;
        virtual double AmpLoad() const = 0;
    };

    struct OneWaySwitch {
        virtual bool IsOn() const = 0;
        virtual void AttachOnChange(const std::function<void()>& func) = 0;
    };

    /**
    Consumable
    Implemented but a component that manages a Consumable that can be drawn from
    by other components.
    */
    struct Consumable {
        // Current Level espressed a 0-empty, 1-full.
        virtual double Level() const = 0;

        // Removes 'amount' from the tank, returns the amount actually drawn.
        virtual double Draw(double amount) = 0;
    };

    struct HydraulicProvider {
        virtual double Level() const = 0;
    };

    struct AvionicsProvider {
        virtual double GetAvAltitude() const = 0;
        virtual void   GetAngularVelocity(VECTOR3& v) = 0;
        virtual double GetBank() const = 0;
        virtual double GetHeading() const = 0;
        virtual double GetKEAS() const = 0;
        virtual double GetMACH() const = 0;
        virtual double GetPitch() const = 0;
        virtual double GetVerticalSpeed() const = 0;
    };

    enum class Axis { Pitch = 0, Yaw = 1, Roll = 2 };

    struct PropulsionControl {
        virtual double  GetMainThrustLevel() const = 0;
        virtual void    SetMainThrustLevel(double l) = 0;
        virtual void    SetAttitudeRotation(Axis axit, double level) = 0;
    };

    struct SurfaceControl {
        virtual void    SetAileronLevel(double l) = 0;
        virtual void    SetElevatorLevel(double l) = 0;
    };

    /**
    * Base class for a Control.
    */
    class Control {
    public:
        Control(int ctrlId) : ctrlId_(ctrlId) {}
        Control() : ctrlId_(-1) {}

        virtual void SetId(int id) { ctrlId_ = id; }
        virtual int GetId() const { return ctrlId_; }

    private:
        int ctrlId_;
    };

    using funcEvent = std::function<void()>;
    using funcState = std::function<double()>;

    template<typename T>
    class ElementControlBase :
        public Control,
        public T
    {
    public:
        ElementControlBase(const UINT pnlGroupId, const NTVERTEX* pnlVerts, const int panelId) 
          : T(
            pnlGroupId,
            pnlVerts,
            panelId,
            [&] {return state_; }
        )
        { }

        void setState(VESSEL4& vessel, bool s) {
            if (setState(s ? 1.0 : 0.0))
                T::triggerRedraw(vessel, GetId());
        }

        void setState(VESSEL4& vessel, double s) {
            if (setState(s))
                T::triggerRedraw(vessel, GetId());
        }

        void setState(VESSEL4& vessel, int s) {
            if (setState((double)s))
                T::triggerRedraw(vessel, GetId());
        }

    protected:

        /// <summary>
        /// Set the state of the Control.
        /// </summary>
        /// <param name="state">New state</param>
        /// <returns>True if the state changed</returns>
        bool setState(double state) {
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