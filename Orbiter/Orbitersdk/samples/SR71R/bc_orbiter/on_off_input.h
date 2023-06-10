//	on_off_input - bco Orbiter Library
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

#include "Control.h"

namespace bc_orbiter {

	/**
	* on_off_input_meta
	* Meta data for on_off_input.  This facilitates setting up a bunch of similar input controls.
	*/
	struct on_off_input_meta {
		double animRotation;
		double animSpeed;
		double animStart;
		double animEnd;
		double hitRadius;
		double pnlOffset;
		int	vcRedrawFlags;
		int vcMouseFlags;
		int pnlRedrawFlags;
		int pnlMouseFlags;
	};

	/**
	* on_off_input
	* Provides and input only control, such as a physical toggle switch.  VC UI is an animation group, so
	* a rotation axis and angle must be provided.  The Panel input is a texture, so the vertex and offset
	* are required.  
	* Since much of the metadata will be the same, many of the metrix are provided via a control_data structure
	* that can be reused for many control. A bank of switches for example.
	*/
	class on_off_input :
		public control,
		public IVCAnimate,
		public IAnimationState,
		public IVCTarget,
		public IPNLTarget {
	public:
		on_off_input(
			int const ctrlId,
			std::initializer_list<UINT> const& vcAnimGroupIds,
			const VECTOR3& vcLocation, const VECTOR3& vcAxisLocation,
			const on_off_input_meta& vcData,
			const UINT pnlGroup,
			const NTVERTEX* pnlVerts,
			const RECT& pnl
		) :
			control(ctrlId),
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
		{ }

		bool IsOn() const { return state_ != 0.0; }

		// IVCAnimate
		AnimationGroup* GetVCAnimationGroup()			override { return &vcAnimGroup_; }
		IAnimationState* GetVCAnimationStateController()	override { return this; }
		double				GetVCAnimationSpeed() const		override { return vcData_.animSpeed; }

		// IAnimationState
		double				GetState() const				override { return state_; }

		// IVCTarget
		VECTOR3& GetVCEventLocation()			override { return vcAnimGroup_.location_; }
		double				GetVCEventRadius()				override { return vcData_.hitRadius; }
		int					GetVCMouseFlags()				override { return vcData_.vcMouseFlags; }
		int					GetVCRedrawFlags()				override { return vcData_.vcRedrawFlags; }

		// IPNLTarget
		RECT& GetPanelRect()					override { return pnlRect_; }
		int					GetPanelMouseFlags()			override { return vcData_.pnlMouseFlags; }
		int					GetPanelRedrawFlags()			override { return vcData_.pnlRedrawFlags; }

		void OnPNLRedraw(MESHHANDLE meshPanel) override {
			DrawPanelOnOff(meshPanel, pnlGroup_, pnlVerts_, IsOn(), pnlOffset_);
		}

		// ITarget
		bool OnEvent() override {
			state_ = (state_ != 0.0) ? 0.0 : 1.0;
			signal_.fire(state_);
			return true;
		}

		// signal
		signal<bool>& Signal() { return signal_; }
	private:
		AnimationGroup	vcAnimGroup_;
		double			state_{ 0.0 };
		on_off_input_meta		vcData_;
		UINT			pnlGroup_;
		const NTVERTEX* pnlVerts_;
		RECT			pnlRect_;
		double			pnlOffset_;

		signal<bool>	signal_;
	};
}