//	OnOffSwitch - bco Orbiter Library
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
#include "IAnimationState.h"
#include "EventTarget.h"

#include "Control.h"
#include "Tools.h"

#include <functional>

namespace bc_orbiter
{
	/**	OnOffSwitch
		A switch with two states, on and off.

        You can define on and off functions to call, or simply
        use the OnOffSwitch instance itself as a true of false
        state holder.
	*/
	class OnOffSwitch : public VCEventTarget, public IAnimationState
	{
	public:
		OnOffSwitch(VECTOR3 target = _V(0.0, 0.0, 0.0), double radius = 0.0) :
            VCEventTarget(target, radius),
            funcOn_([] {}),
            funcOff_([] {}),
			state_(0.0)
		{
            SetLeftMouseDownFunc([this] { Toggle(); });
        }

		void SetOn()        { state_ = 1.0; if (nullptr != funcOn_) { funcOn_(); }	}
		void SetOff()       { state_ = 0.0; if (nullptr != funcOff_) { funcOff_(); } }
		void Toggle()       { (state_ == 0.0) ? SetOn() : SetOff(); }

		bool IsOn() const                       { return (state_ != 0.0); }

		void OnFunction(SwitchStopFunc func)    { funcOn_ = func; }
		void OffFunction(SwitchStopFunc func)   { funcOff_ = func; }

		void SetState(double state)             { (state == 0.0) ? SetOff() : SetOn(); }

		virtual double GetState() const override { return state_; }
	private:
		double			state_;

        SwitchStopFunc funcOn_;
        SwitchStopFunc funcOff_;
	};

	class VCOnOffSwitch : public VCEventTarget, public IAnimationState
	{
	public:
		VCOnOffSwitch(VECTOR3 target = _V(0.0, 0.0, 0.0), double radius = 0.0) :
			VCEventTarget(target, radius),
			funcOn_([] {}),
			funcOff_([] {}),
			state_(0.0)
		{
			SetLeftMouseDownFunc([this] { Toggle(); });
		}

		void SetOn() { state_ = 1.0; if (nullptr != funcOn_) { funcOn_(); } }
		void SetOff() { state_ = 0.0; if (nullptr != funcOff_) { funcOff_(); } }
		void Toggle() { (state_ == 0.0) ? SetOn() : SetOff(); }

		bool IsOn() const { return (state_ != 0.0); }

		void OnFunction(SwitchStopFunc func) { funcOn_ = func; }
		void OffFunction(SwitchStopFunc func) { funcOff_ = func; }

		void SetState(double state) { (state == 0.0) ? SetOff() : SetOn(); }

		virtual double GetState() const override { return state_; }
	private:
		double			state_;

		SwitchStopFunc funcOn_;
		SwitchStopFunc funcOff_;
	};


	/*  THIS WILL REPLACE THE CLASSES ABOVE EVENTUALLY */

	class OnOffToggle : public Control<double>, public IVCAnimate, public IAnimationState, public IVCTarget, public IPNLTarget {
	public:
		OnOffToggle(
			int const ctrlId,
			std::initializer_list<UINT> const& vcAnimGroupIds,
			const VECTOR3& vcLocation, const VECTOR3& vcAxisLocation,
			const ControlData& vcData,
			const UINT pnlGroup,
			const NTVERTEX* pnlVerts,
			const RECT& pnl) :
			Control<double>(ctrlId),
			vcData_(vcData),
			vcAnimGroup_(
				vcAnimGroupIds,
				vcLocation, vcAxisLocation,
				vcData.animRotation,
				vcData.animStart, vcData.animEnd),
			pnlGroup_(pnlGroup),
			pnlVerts_(pnlVerts),
			pnlRect_(pnl),
			pnlOffset_(vcData.pnlOffset)
		{
		}

		bool IsOn() const { return state_ != 0.0; }

		// IVCAnimate
		AnimationGroup* GetAnimationGroup() override { return &vcAnimGroup_; }
		IAnimationState* GetAnimationStateController() override { return this; }
		double GetAnimationSpeed() const override { return vcData_.animSpeed; }

		// IAnimationState
		double GetState() const override { return state_; }

		// IVCTarget
		VECTOR3& GetVCEventLocation() override { return vcAnimGroup_.location_; }
		double GetVCEventRadius() override { return vcData_.hitRadius; }

		// IPNLTarget
		RECT& GetPanelRect() override { return pnlRect_; }

		void OnPNLRedraw(MESHHANDLE meshPanel) override {
			DrawPanelOnOff(meshPanel, pnlGroup_, pnlVerts_, IsOn(), pnlOffset_);
		}

		// ITarget
		bool OnEvent() override { 
			state_ = (state_ != 0.0) ? 0.0 : 1.0;
			Emit(state_);
			return true; 
		}

	private:
		AnimationGroup	vcAnimGroup_;
		double			state_{ 0.0 };
		ControlData		vcData_;
		UINT			pnlGroup_;
		const NTVERTEX*	pnlVerts_;
		RECT			pnlRect_;
		double			pnlOffset_;
	};
}