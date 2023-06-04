//	PanelElement - bco Orbiter Library
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

#include "Orbitersdk.h"

namespace bc_orbiter
{
	class PanelElement
	{
	public:
		PanelElement(const NTVERTEX* verts, const RECT rect, UINT groupId) :
			verts_(verts),
			rc_(rect),
			groupId_(groupId)
		{}

		/** Reset
		* This happens every time a panel is loaded, so, since we don't have an unload
		* event, nothing should be allocated in this call.  Just register the area with
		* the vessel.
		* This can maybe be moved to a vessel helper class, since it requires VESSEL4.
		* 
		* Called from clbkLoadPanel2D
		*/
		virtual void Reset(VESSEL3* vessel, PANELHANDLE pHandle, MESHHANDLE panelMesh)
		{
			hPanel_ = pHandle;
			panelMesh_ = panelMesh;

			oapiRegisterPanelArea(areaId_, rc_, PANEL_REDRAW_USER);

			vessel->RegisterPanelArea(
				pHandle,
				areaId_,
				rc_,
				PANEL_REDRAW_MOUSE,
				PANEL_MOUSE_LBDOWN);

			groupMesh_ = oapiMeshGroup(panelMesh, groupId_);
		}

		void SetTargetId(int id) { areaId_ = id; }
		void RedrawArea()
		{
			vessel_->TriggerRedrawArea(0, 0, areaId_);
		}

		virtual void RotateMesh(MESHHANDLE mesh) {}

	protected:
		const MESHGROUP* GroupMesh() { return groupMesh_; }
		const NTVERTEX* Verts() { return verts_; }

	private:
		VESSEL4*		vessel_{ nullptr };
		PANELHANDLE		hPanel_{ nullptr };
		MESHHANDLE		panelMesh_{ nullptr };	// Panel mesh handle, use this to get the group mesh handle.
		MESHGROUP*		groupMesh_{ nullptr };	// Group mesh.
		int				areaId_{ 0 };			// Event id that will be assigned to this group (may not be needed)
		const NTVERTEX* verts_{ nullptr };		// Mesh verts.
		UINT			groupId_{ 0 };			// Mesh group id.
		const RECT		rc_;					// Group rectangle.

	};
}