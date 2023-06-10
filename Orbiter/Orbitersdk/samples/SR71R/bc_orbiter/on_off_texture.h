//	on_off_texture - bco Orbiter Library
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

#include <functional>

namespace bc_orbiter {

	/*
	* on_off_texture
	* Creates a visual only binary state control that can be used in VC and panel UIs.  
	* For both VC and Panel, the class takes an NTVERTEX array that is assumed to have 4 entries
	* and represents a rectangle.  The UV texture for that rectangle is shifted to the right in the 
	* 'x' axis 'offset' amount to move from OFF to ON.
	* The state of the UI is control via a slot input.
	**/
	class on_off_texture : public control, public IVCTarget, public IPNLTarget {
	public:
		on_off_texture(
			int ctrlId,
			const UINT vcGroupId,
			const NTVERTEX* vcVerts,
			const UINT pnlGroupId,
			const NTVERTEX* pnlVerts,
			double offset)
			:
			control(ctrlId),
			vcGroupId_(vcGroupId),
			vcVerts_(vcVerts),
			pnlGroupId_(pnlGroupId),
			pnlVerts_(pnlVerts),
			offset_(offset),
			slotState_([&](double v) {
			auto bv = bool(v != 0.0);
			if (state_ != bv) {
				state_ = bv;
				oapiTriggerRedrawArea(0, 0, get_id());
			}})
		{
		}

			void OnVCRedraw(DEVMESHHANDLE vcMesh) override {
				NTVERTEX* delta = new NTVERTEX[4];

				TransformUV2d(
					vcVerts_,
					delta, 4,
					_V(state_ ? offset_ : 0.0,
						0.0,
						0.0),
					0.0);

				GROUPEDITSPEC change{};
				change.flags = GRPEDIT_VTXTEX;
				change.nVtx = 4;
				change.vIdx = NULL; //Just use the mesh order
				change.Vtx = delta;
				auto res = oapiEditMeshGroup(vcMesh, vcGroupId_, &change);
				delete[] delta;
			}

			void OnPNLRedraw(MESHHANDLE meshPanel) override {
				DrawPanelOnOff(meshPanel, pnlGroupId_, pnlVerts_, state_, offset_);
			}

			int GetVCMouseFlags() { return PANEL_MOUSE_IGNORE; }
			int GetVCRedrawFlags() { return PANEL_REDRAW_USER; }
			int GetPanelMouseFlags() { return PANEL_MOUSE_IGNORE; }
			int GetPanelRedrawFlags() { return PANEL_REDRAW_USER; }

			slot<double>& Slot() { return slotState_; }
	private:
		UINT					vcGroupId_;
		const NTVERTEX* vcVerts_;
		UINT					pnlGroupId_;
		const NTVERTEX* pnlVerts_;
		bool					state_{ false };
		double					offset_{ 0.0 };
		slot<double>			slotState_;
	};
}