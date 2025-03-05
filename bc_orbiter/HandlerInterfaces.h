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

// These interfaces are implemented by Vessel components to indicate to the base Vessel that then need
// to handle specific messages.

#include <OrbiterAPI.h>
#include <functional>

namespace bc_orbiter {

    class Vessel;   // forward declare.

    /**
    VesselComponent
    Every component managed by Vessel must derive from VesselComponent.
    */
    struct VesselComponent
    {
    public:
        virtual ~VesselComponent() = default;

        virtual void HandleDrawHud(Vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) {}
        virtual void HandlePostStep(Vessel& vessel, double simt, double simdt, double mjd) {}
        virtual void HandleSetClassCaps(Vessel& vessel) {}
        virtual bool HandleLoadState(Vessel& vessel, const std::string& line) { return false; }
        virtual std::string HandleSaveState(Vessel& vessel) { return ""; }
        virtual bool HandleLoadVC(Vessel& vessel, int vcid) { return false; }
        virtual bool HandleRedrawVC(Vessel& vessel, int id, int event, SURFHANDLE surf) { return false; }
        virtual bool HandleMouseVC(Vessel& vessel, int id, int event) { return false; }
        virtual bool HandleLoadPanel(Vessel& vessel, int id, PANELHANDLE hPanel) { return false; }
        virtual bool HandleRedrawPanel(Vessel& vessel, int id, int event, SURFHANDLE surf) { return false; }
        virtual bool HandleMousePanel(Vessel& vessel, int id, int event) { return false; }
    };

    /**
    SetClassCaps
    Indicates the class participates in setClassCaps.  The class must implement the
    call void HandleSetClassCaps(Vessel&).
    */
    struct SetClassCaps {
        virtual void HandleSetClassCaps(Vessel& vessel) = 0;
        virtual ~SetClassCaps() {};
    };

    /**
    PostStep
    Indicates the class participates in postStep callbacks.  The class must implement
    the post step handler.
    */
    struct PostStep {
        virtual void HandlePostStep(Vessel& vessel, double simt, double simdt, double mjd) = 0;
        virtual ~PostStep() {};
    };

    /**
    DrawHud
    Indicates the class participates in clbkDrawHud.  The class must implement the call
    HandleDrawHud(Vessel& Vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp).
    */
    struct DrawHud {
        virtual void HandleDrawHud(Vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) = 0;
        virtual ~DrawHud() {};
    };

    /**
    ManageState
    Indicates the class can provide its internal state as a string, and that it can take that string back
    in order to set the internal state.  The string provided should be appropriate for use in an Orbiter
    config file, but does not provide the key, which will be provided by whatever is actually managing the config.
    */
    struct ManageState {
        /**
        HandleLoadState
        param line A single line of text representing the state.
        return true if the state was succesfully restored.
        */
        virtual bool HandleLoadState(Vessel& vessel, const std::string& line) = 0;

        /**
        handle_save
        Return a single string that represents the internal state of the component.
              */
        virtual std::string HandleSaveState(Vessel& vessel) = 0;

        virtual ~ManageState() {};
    };

    /**
    LoadVC
    Indicates the class has special handling when loading a virtual cockpit.
    */
    struct LoadVC {
        /**
        HandleLoadVC
        Do work required to setup vc components.
        */
        virtual bool HandleLoadVC(Vessel& vessel, int vcid) = 0;
        virtual bool HandleRedrawVC(Vessel& vessel, int id, int event, SURFHANDLE surf) { return false; }
        virtual bool HandleMouseVC(Vessel& vessel, int id, int event) { return false; }
        virtual ~LoadVC() {};
    };

    /**
    LoadPanel
    Indicates the class has special handling when load a 2D panel.
    */
    struct LoadPanel {
        virtual bool HandleLoadPanel(Vessel& vessel, int id, PANELHANDLE hPanel) = 0;
        virtual bool HandleRedrawPanel(Vessel& vessel, int id, int event, SURFHANDLE surf) { return false; }
        virtual bool HandleMousePanel(Vessel& vessel, int id, int event) { return false; }
        virtual ~LoadPanel() {};
    };



    // Func defs.

    using funcVCMouseEvent = std::function<void(Vessel& vessel, int id, int event, VECTOR3& location)>;
    using funcVCRedrawEvent = std::function<void(Vessel & vessel, int id, int event, SURFHANDLE surf, DEVMESHHANDLE devMesh)>;
    using funcPanelMouseEvent = std::function<void(Vessel& vessel, int id, int event, int mx, int my)>;
    using funcPanelRedrawEvent = std::function<void(Vessel& vessel, int id, int event, SURFHANDLE surf)>;

    struct VCMouseEventData {
        const VECTOR3&      location_;
        const double        radius_;
        funcVCMouseEvent    func_;

        VCMouseEventData(const VECTOR3& loc, const double rad, funcVCMouseEvent func) : location_(loc), radius_(rad), func_(func){}
    };

    struct VCRedrawEventData {
        const RECT&         rect_;
        funcVCRedrawEvent   func_;

        VCRedrawEventData(const RECT& rc, funcVCRedrawEvent func) : rect_(rc), func_(func) {}
    };

    struct PanelMouseEventData {
        const RECT&         rect_;
        funcPanelMouseEvent func_;

        PanelMouseEventData(const RECT& rect, funcPanelMouseEvent func) : rect_(rect), func_(func) {}
    };

    struct PanelRedrawEventData {
        funcPanelRedrawEvent    func_;

        PanelRedrawEventData(funcPanelRedrawEvent func) : func_(func) {}
    };

    struct VCMFDEventData {
        RECT                    rect_;
        const VECTOR3&          location_;
        double                  radius_;
        DWORD                   texId_;

        funcPanelMouseEvent     funcMouse_;
        funcPanelRedrawEvent    funcRedraw_;

        VCMFDEventData(const RECT& rect, const VECTOR3& loc, double r, DWORD txid, funcPanelMouseEvent fMouse, funcPanelRedrawEvent fRedraw) 
            : rect_(rect), location_(loc), radius_(r), texId_(txid), funcMouse_(fMouse), funcRedraw_(fRedraw){}
    };
};