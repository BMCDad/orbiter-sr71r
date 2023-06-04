//	PanelButton - bco Orbiter Library
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

#include <functional>
#include "PanelElement.h"

namespace bc_orbiter
{
	class PanelButton : public PanelElement
	{
	public:
		PanelButton(
			const NTVERTEX* verts,
			const RECT rect,
			UINT group,
			double trans,
			std::function<void()> fnClick,
			std::function<bool()> fnState) :
			PanelElement(verts, rect, group),
			translate_(trans),
			funcClick_(fnClick),
			funcState_(fnState)
		{}

		void RotateMesh(MESHHANDLE mesh) override;
		void DoClick() { if (nullptr != funcClick_) funcClick_(); }

	private:
		double			translate_;		// Translation to perform, assumes texture uv only

		std::function<void()>	funcClick_{ nullptr };
		std::function<bool()>	funcState_{ nullptr };
	};

	inline void PanelButton::RotateMesh(MESHHANDLE mesh)
	{
		double trans = 0.0;

		auto grp = GroupMesh();
		auto vrt = Verts();

		trans = funcState_() ? translate_ : 0.0;
		grp->Vtx[0].tu = vrt[0].tu + trans;
		grp->Vtx[1].tu = vrt[1].tu + trans;
		grp->Vtx[2].tu = vrt[2].tu + trans;
		grp->Vtx[3].tu = vrt[3].tu + trans;
	}
}