//	PushButtonSwitch - bco Orbiter Library
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

#include "bco.h"
#include "EventTarget.h"
#include <functional>

namespace bc_orbiter
{
	/**	PushButtonSwitch
		A simple push button switch with a single action and no state.
	*/
	class PushButtonSwitch : public VCEventTarget
	{
	public:
		PushButtonSwitch(VECTOR3 target = _V(0.0, 0.0, 0.0), double radius = 0.0, SwitchStopFunc func = { [] {} }) :
            VCEventTarget(target, radius),
			funcPressed_(func)
		{
            SetLeftMouseDownFunc([this] { Push(); });
		}

		void Push() { if (nullptr != funcPressed_) funcPressed_();	}

		void SetPressedFunc(SwitchStopFunc func) { funcPressed_ = func; }

	public:
		SwitchStopFunc	funcPressed_;
	};

	class PanelPushButtonSwitch : public PanelEventTarget
	{
	public:
		PanelPushButtonSwitch(const RECT rc, SwitchStopFunc func = { [] {} }) :
			PanelEventTarget(rc),
			funcPressed_(func)
		{
			SetLeftMouseDownFunc([this] { Push();  });
		}

		void Push() { if (nullptr != funcPressed_) funcPressed_(); }

	private:
		SwitchStopFunc funcPressed_{ nullptr };
	};
}