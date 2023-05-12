//	PanelTextureVisual - bco Orbiter Library
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

#include "Visual.h"
#include "Tools.h"

namespace bc_orbiter
{
	/**	PanelTextureVisual
	Base class for a visual element that changes by altering the texture UV coordinates.
	*/
	class PanelTextureVisual : public Visual
	{
		int numVerts_;
	public:
		PanelTextureVisual(const NTVERTEX* verts, UINT group, int numVerts = 4) 
			: Visual(verts, group), numVerts_(numVerts)
		{ }

		/**	Draw
		Called to update the visual.  Implementing class needs to define how the visual
		will be manipulated.
		*/
		virtual void Draw(MESHHANDLE devMesh)
		{
			VECTOR3 trans;

			auto grp = oapiMeshGroup(devMesh, GetGroup());
			auto vrt = GetVerts();

			trans = GetTranslate();
			grp->Vtx[0].tu = vrt[0].tu + trans.x;
			grp->Vtx[1].tu = vrt[1].tu + trans.x;
			grp->Vtx[2].tu = vrt[2].tu + trans.x;
			grp->Vtx[3].tu = vrt[3].tu + trans.x;
		}
	};
}
