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

#include "control.h"

namespace bc_orbiter {

	using anim_ids = std::initializer_list<UINT>;

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
	on_off_input
	Provides an input only control, such as a physical toggle switch.  VC UI is an animation group, so
	a rotation axis and angle must be provided.  The Panel input is a texture, so the vertex and offset
	are required.  
	Since much of the metadata will be the same, many of the metrics are provided via a control_data structure
	that can be reused for many control. A bank of switches for example.
	*/
	class on_off_input :
		  public control
		, public vc_animation
		, public vc_event_target
		, public panel_event_target
		, public one_way_switch
		, public signaller
	{
	public:
		on_off_input(
			  anim_ids const&			vcAnimGroupIds
			, const VECTOR3&			vcLocation
			, const VECTOR3&			vcAxisLocation
			, const on_off_input_meta&	vcData
			, const UINT				pnlGroup
			, const NTVERTEX*			pnlVerts
			, const RECT&				pnl
		) :
			vcData_(vcData),
			vcAnimGroup_(
				vcAnimGroupIds,
				vcLocation, vcAxisLocation,
				vcData.animRotation,
				vcData.animStart, vcData.animEnd),
			pnlGroup_(pnlGroup),
			pnlVerts_(pnlVerts),
			pnlRect_(pnl),
			pnlOffset_(vcData.pnlOffset),
			animVC_(vcData_.animSpeed)
		{ }

		bool IsOn() const { return state_; }
		bool is_on() const override { return state_; }

		// vc_animation
		animation_group*		vc_animation_group()		override { return &vcAnimGroup_; }
		double				vc_animation_speed() const	override { return vcData_.animSpeed; }
		double vc_step(double simdt) override {
			animVC_.Step(state_ ? 1.0 : 0.0, simdt);
			return animVC_.GetState();
		}

		// vc_event_target
		VECTOR3				vc_event_location()			override { return vcAnimGroup_.location_; }
		double				vc_event_radius()			override { return vcData_.hitRadius; }
		int					vc_mouse_flags()			override { return vcData_.vcMouseFlags; }
		int					vc_redraw_flags()			override { return vcData_.vcRedrawFlags; }

		// panel_event_target
		RECT				panel_rect()				override { return pnlRect_; }
		int					panel_mouse_flags()			override { return vcData_.pnlMouseFlags; }
		int					panel_redraw_flags()		override { return vcData_.pnlRedrawFlags; }

		void on_panel_redraw(MESHHANDLE meshPanel) override {
			DrawPanelOnOff(meshPanel, pnlGroup_, pnlVerts_, IsOn(), pnlOffset_);
		}

		// event_target
		bool on_event(int id, int event) override {
			state_ = !state_;
			fire();
			return true;
		}

		void attach_on_change(const std::function<void()>& func) override {
			attach(func);
		}

		friend std::istream& operator>>(std::istream& input, on_off_input& obj) {
			bool isEnabled;

			input >> isEnabled;
			obj.state_ = isEnabled;
			obj.animVC_.SetState(isEnabled ? 1.0 : 0.0);
			obj.fire();

			return input;
		}

		friend std::ostream& operator<<(std::ostream& output, on_off_input& obj) {
			output << obj.state_ ? "1" : "0";
			return output;
		}

	private:
		animation_group		vcAnimGroup_;
		bool				state_{ false };
		on_off_input_meta	vcData_;
		UINT				pnlGroup_;
		const NTVERTEX*		pnlVerts_;
		RECT				pnlRect_;
		double				pnlOffset_;
		animation_target			animVC_;
	};
}