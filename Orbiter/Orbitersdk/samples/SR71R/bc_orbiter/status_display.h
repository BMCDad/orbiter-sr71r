//	status_display - bco Orbiter Library
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
	status_display
	A four state status display control intended for the status display panel.
	**/
	class status_display :
		public Control
		, public VCEventTarget
		, public PanelEventTarget
	{
	public:
		enum status {
			on		= 0,
			off		= 1,
			error	= 2,
			warn	= 3
		};

		status_display(
			const UINT vcGroupId
			, const NTVERTEX* vcVerts
			, const UINT pnlGroupId
			, const NTVERTEX* pnlVerts
			, double offset
		) :
			Control(-1)
			, vcGroupId_(vcGroupId)
			, vcVerts_(vcVerts)
			, pnlGroupId_(pnlGroupId)
			, pnlVerts_(pnlVerts)
			, offset_(offset)
		{
		}

		void OnVCRedraw(DEVMESHHANDLE vcMesh) override {
			NTVERTEX* delta = new NTVERTEX[4];

			TransformUV2d(
				vcVerts_,
				delta, 4,
				_V(state_ * offset_,
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

		void OnPanelRedraw(MESHHANDLE meshPanel) override {
			DrawPanelOffset(meshPanel, pnlGroupId_, pnlVerts_, offset_ * state_);
		}

		int VCMouseFlags() { return PANEL_MOUSE_IGNORE; }
		int VCRedrawFlags() { return PANEL_REDRAW_USER; }
		int PanelMouseFlags() { return PANEL_MOUSE_IGNORE; }
		int PanelRedrawFlags() { return PANEL_REDRAW_USER; }

		void set_state(status s) {
			state_ = s;
			oapiTriggerRedrawArea(0, 0, GetId());
		}

	private:
		UINT				vcGroupId_;
		const NTVERTEX*		vcVerts_;
		UINT				pnlGroupId_;
		const NTVERTEX*		pnlVerts_;
		status				state_{ status::on };
		double				offset_{ 0.0 };
	};
}
