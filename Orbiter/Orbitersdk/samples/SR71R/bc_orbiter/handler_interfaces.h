//	handler_interfaces - bco Orbiter Library
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

// These interfaces are implemented by vessel components to indicate to the base vessel that then need
// to handle specific messages.

#include <OrbiterAPI.h>

namespace bc_orbiter {

class Vessel;		// forward declare.

/**
  VesselComponent
  Every component managed by vessel must derive from vessel_component.
*/
class VesselComponent {
 public:
  virtual ~VesselComponent() = default;
};

/**
  HandlesSetClassCaps
  Indicates the class participates in setClassCaps.  The class must implement the
  call void handle_set_class_caps(vessel&).
*/
class HandlesSetClassCaps {
 public:
  virtual void HandleSetClassCaps(Vessel& Vessel) = 0;
  virtual ~HandlesSetClassCaps() {};
};

/**
  post_step
  Indicates the class participates in postStep callbacks.  The class must implement
  the post step handler.
*/
class HandlesPostStep {
 public:
  virtual void HandlePostStep(Vessel& Vessel, double simt, double simdt, double mjd) = 0;
  virtual ~HandlesPostStep() {};
};

/**
  draw_hud
  Indicates the class participates in clbkDrawHud.  The class must implement the call
  handle_draw_hud(vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp).
*/
class HandlesDrawHud {
 public:
  virtual void HandleDrawHUD(Vessel& Vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) = 0;
  virtual ~HandlesDrawHud() {};
};

/**
  manage_state
  Indicates the class can provide its internal state as a string, and that it can take that string back
  in order to set the internal state.  The string provided should be appropriate for use in an Orbiter
  config file, but does not provide the key, which will be provided by whatever is actually managing the config.
*/
class HandlesState {
 public:
  /**
    handle_load_state
    param line A single line of text representing the state.
    return true if the state was succesfully restored.
  */
  virtual bool HandleLoadState(Vessel& Vessel, const std::string& line) = 0;

  /**
    handle_save
    Return a single string that represents the internal state of the component.
  */
  virtual std::string HandleSaveState(Vessel& Vessel) = 0;

  virtual ~HandlesState() {};
};

/**
  load_vc
  Indicates the class has special handling when loading a virtual cockpit.
*/
class HandlesVCLoading {
 public:
  /**
    handle_load_vc
    Do work required to setup vc components.
  */
  virtual bool HandleLoacVC(Vessel& Vessel, int vcid) = 0;
  virtual bool HandleRedrawVC(Vessel& Vessel, int id, int event, SURFHANDLE surf) { return false; }
  virtual bool HandleMouseVC(Vessel& Vessel, int id, int event) { return false; }
  virtual ~HandlesVCLoading() {};
};

/**
  load_panel
  Indicates the class has special handling when load a 2D panel.
*/
class HandlesPanelLoading {
 public:
  virtual bool HandleLoadPanel(Vessel& Vessel, int id, PANELHANDLE hPanel) = 0;
  virtual bool HandleRedrawPanel(Vessel& Vessel, int id, int event, SURFHANDLE surf) { return false; }
  virtual bool HandleMousePanel(Vessel& Vessel, int id, int event) { return false; }
  virtual ~HandlesPanelLoading() {};
};
};