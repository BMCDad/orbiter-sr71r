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

namespace bc_orbiter {
	class Vessel;
}

#include <vector>
#include <functional>
#include <memory>

#include "OrbiterAPI.h"
#include "Animation.h"
#include "Tools.h"
#include "signals.h"

namespace bc_orbiter {
/**
  VCAnimation
  Implemented by a control that needs to take part in the VC cockpit animation step.
  The implementing class will provide the group to animate, as well as the speed.  It
  will also implement the actual step that will update the animation.  The presence of 
  this base class instructs vessel to add it to a collection that will be called during
  the VC step.
*/
class VCAnimation {
 public:
  virtual AnimationGroup* VCAnimationGroup() = 0;
  virtual double VCAnimationSpeed() const = 0;
  virtual double VCStep(double simdt) { return 0.0; }
};

/**
  PanelAnimation
  Implemented by a control that needs to animate as part of the panel animation step.
  Remember:  Panel animations are just mesh transforms, and are not part of Orbiter animations.
*/
class PanelAnimation {
 public:
  virtual void PanelStep(MESHHANDLE mesh, double simdt) = 0;
};

/**
* VCTextureAnimation
* Similar to panel_animation where the animation is a texture, and not a mesh group.  vc_animation
* animates a group.  Better naming is needed to avoid confusion.  Note that panels use MESHHANDLE where
* VCs use DEVMESHHANDLEs.
*/
class VCTextureAnimation {
 public:
  virtual void VCStep(DEVMESHHANDLE mesh, double simdt) = 0;
};

/**
* event_target
* Base class for any control that will be an event target.
*/
class EventTarget {
 public:
  virtual bool OnEvent(int id, int event) { return false; }
};

/**
  VCEventTarget
  Implemented to indicate that a VC control is a target for either mouse events, or redraw
  events, or both.  If you need mouse events, override vc_mouse_flags to enable mouse events.
  For redraw events, override vc_redraw_flags.
*/
class VCEventTarget : public EventTarget {
 public:
  virtual VECTOR3 VCEventLocation() { return _V(0.0, 0.0, 0.0); }
  virtual double VCEventRadius() { return 0.0; }
  virtual int VCMouseFlags() { return PANEL_MOUSE_IGNORE; }
  virtual int VCRedrawFlags() { return PANEL_REDRAW_NEVER; }
  virtual void OnVCRedraw(DEVMESHHANDLE meshVC) { }
};

/**
  PanelEventTarget
  Implemented to indicate that a panel control is a target for either mouse events, or redraw
  events, or both.  If you need mouse events, override vc_mouse_flags to enable mouse events.
  For redraw events, override vc_redraw_flags.
*/
class PanelEventTarget : public EventTarget {
 public:
  virtual RECT PanelRect() { return _R(0, 0, 0, 0); }
  virtual int PanelMouseFlags() { return PANEL_MOUSE_IGNORE; }
  virtual int PanelRedrawFlags() { return PANEL_REDRAW_NEVER; }
  virtual void OnPanelRedraw(MESHHANDLE meshPanel) {}
};

class PowerConsumer {
 public:
  virtual void OnChange(double v) { };  // A class that has a time step may not need change notification.
  virtual double AmpDraw() const = 0;
};

class PowerProvider {
 public:
  virtual void AttachConsumer(PowerConsumer* consumer) = 0;
  virtual double VoltsAvailable() const = 0;
  virtual double AmpLoad() const = 0;
};

class OneWaySwitch {
 public:
  virtual bool IsOn() const = 0;
  virtual void AttachOnChange(const std::function<void()>& func) = 0;
};

/**
  consumable
  Implemented but a component that manages a consumable that can be drawn from
  by other components.
*/
class Consumable {
 public:
  // Current level espressed a 0-empty, 1-full.
  virtual double Level() const = 0;
		
  // Removes 'amount' from the tank, returns the amount actually drawn.
  virtual double Draw(double amount) = 0;
};

class HydraulicProvider {
 public:
  virtual double Level() const = 0;
};

class AvionicsProvider {
 public:
  virtual double AvGetAltitude() const = 0;
  virtual void AvGetAngularVelocity(VECTOR3& v) = 0;
  virtual double AvGetBank() const = 0;
  virtual double AvGetHeading() const = 0;
  virtual double AvGetKEAS() const = 0;
  virtual double AvGetMach() const = 0;
  virtual double AvGetPitch() const = 0;
  virtual double AvGetVerticalSpeed() const = 0;
};

enum class Axis { Pitch = 0, Yaw = 1, Roll = 2 };

class PropulsionControl {
public:
  virtual double GetMainThrustLevel() const = 0;
  virtual void SetMainThrustLevel(double l) = 0;
  virtual void SetAttitudeRotation(Axis axit, double Level) = 0;
};

class SurfaceControl {
public:
  virtual void SetAileronLevel(double l) = 0;
  virtual void SetElevatorLevel(double l) = 0;
};

/**
* Base class for a control.
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
} // namespace bc_orbiter