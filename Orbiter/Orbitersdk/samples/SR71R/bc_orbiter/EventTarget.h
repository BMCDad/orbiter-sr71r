//	EventTarget - bco Orbiter Library
//	Copyright(C) 2017  Blake Christensen
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
#include "bco.h"
#include <vector>
#include <functional>

namespace bc_orbiter
{
    class EventTarget
    {
    public:
        EventTarget()
        {}

        bool HandleMouse(int event)
        {
            bool result = true;
            switch (event)
            {
            case PANEL_MOUSE_LBDOWN:	funcLeftMouseDown_();		break;
            case PANEL_MOUSE_RBDOWN:	funcRightMouseDown_();		break;
            case PANEL_MOUSE_LBPRESSED:	funcLeftMousePressed_();	break;
            case PANEL_MOUSE_RBPRESSED:	funcRightMousePressed_();	break;

            default:					result = false;				break;
            }

            return result;
        }

        void SetLeftMouseDownFunc(SwitchStopFunc f)     
        {
            funcLeftMouseDown_ = f; 
            mouseEvents_.push_back(PANEL_MOUSE_DOWN);
        }

        void SetRightMouseDownFunc(SwitchStopFunc f)    
        {
            funcRightMouseDown_ = f; 
            mouseEvents_.push_back(PANEL_MOUSE_RBDOWN);
        }

        void SetLeftMousePressedFunc(SwitchStopFunc f)  
        {
            funcLeftMousePressed_ = f; 
            mouseEvents_.push_back(PANEL_MOUSE_PRESSED);
        }

        void SetRightMousePressedFunc(SwitchStopFunc f) 
        {
            funcRightMousePressed_ = f; 
            mouseEvents_.push_back(PANEL_MOUSE_RBPRESSED);
        }

        void DoLeftMouseClick()     { funcLeftMouseDown_(); }
        void DoRightMouseClick()    { funcRightMouseDown_(); }

        void SetEventId(int id) { eventId_ = id; }
        int GetEventId() const { return eventId_; }

        virtual void RegisterMouseEvents() 
        {
            for (auto& e : mouseEvents_)
            {
                RegisterMouseEvent(e);
            }
        }

    protected:
        virtual void RegisterMouseEvent(int evt) {}

    private:
        int                 eventId_    { 0 };

        std::vector<int>    mouseEvents_;

        SwitchStopFunc		funcLeftMouseDown_      { [] {} };
        SwitchStopFunc		funcRightMouseDown_     { [] {} };

        SwitchStopFunc		funcLeftMousePressed_   { [] {} };
        SwitchStopFunc		funcRightMousePressed_  { [] {} };
    };

    class VCEventTarget : public EventTarget
    {
    public:
        VCEventTarget(VECTOR3 location, double radius) :
            location_(location),
            radius_(radius)
        {}

        void RegisterMouseEvent(int evt) override
        {
            oapiVCRegisterArea(GetEventId(), PANEL_REDRAW_NEVER, evt);
            oapiVCSetAreaClickmode_Spherical(GetEventId(), location_, radius_);
        }

    private:
        double              radius_;
        VECTOR3             location_;
    };

    class PanelEventTarget : public EventTarget
    {
    public:
        PanelEventTarget(const RECT rc) :
            rect_(rc)
        {}

        void OnLoad(int panelId, PANELHANDLE pHandle, VESSEL4* vessel)
        {
            panelId_ = panelId;
            pHandle_ = pHandle;
            vessel_ = vessel;
        }

        void RegisterMouseEvent(int evt) override
        {
            oapiRegisterPanelArea(GetEventId(), rect_, PANEL_REDRAW_NEVER, evt);
            vessel_->RegisterPanelArea(
                pHandle_,
                GetEventId(),
                rect_,
                PANEL_REDRAW_MOUSE,
                PANEL_MOUSE_LBDOWN);
        }

    private:
        RECT    rect_;
        int         panelId_;
        VESSEL4*    vessel_;
        PANELHANDLE pHandle_;
    };
}
