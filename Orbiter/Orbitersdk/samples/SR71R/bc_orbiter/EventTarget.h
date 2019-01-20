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
        EventTarget(VECTOR3 location, double radius) :
            location_(location),
            radius_(radius),
            funcLeftMouseDown_([] {}),		// Default null functions.
            funcRightMouseDown_([] {}),
            funcLeftMousePressed_([] {}),
            funcRightMousePressed_([] {})
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

        void RegisterMouseEvents()
        {
            for (auto& e : mouseEvents_)
            {
                oapiVCRegisterArea(eventId_, PANEL_REDRAW_NEVER, e);
                oapiVCSetAreaClickmode_Spherical(eventId_, location_, radius_);
            }
        }

    private:
        int                 eventId_;

        double              radius_;
        VECTOR3             location_;

        std::vector<int>    mouseEvents_;

        SwitchStopFunc		funcLeftMouseDown_;
        SwitchStopFunc		funcRightMouseDown_;

        SwitchStopFunc		funcLeftMousePressed_;
        SwitchStopFunc		funcRightMousePressed_;
    };
}
