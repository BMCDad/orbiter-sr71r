//	Component - bco Orbiter Library
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

namespace bc_orbiter {
  class vessel;
}

#include <vector>

namespace bc_orbiter
{
class vessel;

/**
  Base class for modeling the behavior of a vessel's component parts.
*/
class Component {
 public:
  Component(vessel* baseVessel) : baseVessel_(baseVessel) {}

  /**
    Override to initialize things like animations etc. Called from clbkSetClassCaps.
    This is called once when the instance of this vessel is created.
  */
  virtual void OnSetClassCaps() {}
		
  /**
    Override to load component settings from the configuration file.  Called from clbkLoadStateEx.
  */
  virtual bool OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) { return false; }

  /**
    Override to save component settings in the configuration file.  Called from clbkSaveState.
  */
  virtual void OnSaveConfiguration(FILEHANDLE scn) const { }

  /**
    Override to setup the component Virtual Cockpit objects.  Called from clbkLoadVC.  This is called
    each time the cockpit mode is set to VC.  There is no unload event, so reset type calls can be
    called here, but any kind of allocation should not be done here.
  */
  virtual bool OnLoadVC(int id) { return false; }

  /**
    Override to handle VC mouse events.  Called from clbkVCMouseEvent.  Id is the control id which is
    the source of the event.  Event is the type of event, up, down etc.
  */
  virtual bool OnVCMouseEvent(int id, int event) { return false; }

  /**
    Override to handle virtual cockpit redraw events. Called from clbkVCRedrawEvent.  Called in response
    to a 'Trigger' redraw for the given id.
  */
  virtual bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf) { return false; }

  /**
    Override to setup the 2D panel.  Called from clbkLoadPanel2D every time the 2D panel is selected.
    Avoid allocation calls here as there is no unload event.
  */
  virtual bool OnLoadPanel2D(int id, PANELHANDLE hPanel) { return false; }

  /**
    Override to handle 2D panel redraw events.  Called from clbkPanelRedrawEvent.
  */
  virtual bool OnPanelRedrawEvent(int id, int event, SURFHANDLE surf) { return false; }

  virtual bool OnPanelMouseEvent(int id, int event) { return false; }

  void SetRedrawId(int id) { redrawId_ = id; }
  int GetRedrawId() const { return redrawId_; }

  vessel*	GetBaseVessel() const { return baseVessel_; }

 private:
  vessel*		baseVessel_;

  int redrawId_	{ 0 };
};
}