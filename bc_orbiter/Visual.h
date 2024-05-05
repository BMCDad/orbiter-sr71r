//	Visual - bco Orbiter Library
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

#include "orbitersdk.h"

namespace bc_orbiter
{
	/**	Visual
	Base class for a visual element.
	*/
	class Visual
	{
	public:
		/**	Visual
		Constructor.
		\param verts Vertex array that defines the mesh that controls the visual.  This is assumed to be
		four NVERTEX structs that form a square.
		\param group The group index for the mesh object.
		*/
		Visual(const NTVERTEX* verts, UINT group) :
			verts_(verts),
			group_(group),
			angle_(0.0)
		{
			translate_.x = 0.0;
			translate_.y = 0.0;
			translate_.z = 0.0;
		}

		virtual ~Visual() = default;

		/** SetAngle.
		Directly set the angle to use.
		*/
		void SetAngle(double angle) { angle_ = angle; };

		/** SetTransform.
		Directly set the transform.
		*/
		void SetTranslate(const VECTOR3& transform) { translate_ = transform; }

	protected:
		/** GetTranslate
		Must be defined by the implementing class to control how the texture is moved to accomplish
		the visual update.
		*/
		virtual VECTOR3& GetTranslate() { return translate_; }

		/**	GetGangle
		Defined by the implementing class to control any rotation needed to update the visual.
		*/
		virtual double GetAngle() { return angle_; }

		/**	GetVerts.
		Returns the verts array used by the visual.
		*/
		const NTVERTEX* GetVerts() const { return verts_; }

		/**	GetGroup
		Returns the group id for the mesh.
		*/
		UINT GetGroup() const { return group_; }

	private:
		const NTVERTEX*		verts_;
		UINT				group_;

		VECTOR3				translate_;
		double				angle_;
	};
}