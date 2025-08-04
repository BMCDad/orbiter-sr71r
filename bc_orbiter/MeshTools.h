/*
MeshTools - Orbiter Addon
Copyright(C) 2025  Blake Christensen

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "Orbitersdk.h"

namespace bc_orbiter
{
    /**
    Transforms the x and y members of the source NTVERTEX array based on the translate and angle parameters
    and returns the transformed array in the vectors parameter.  This is useful for translating or rotating a mesh for
    animating a panel surface.
    @param source The NTVERTEX array to transform.
    @param vectors The resulting NTVERTEX array.
    @param numVectors The number of vectors in both source and vectors.
    @param translate A vector whose x and y members indicate the translation to apply.
    @param angle The angle in radians to rotate the x and y coordinates.  The center is calculated average
    of the x and y values in source.
    */
    inline void TransformXY2d(const NTVERTEX* source, NTVERTEX* vectors, int numVectors, const VECTOR3& translate, double angle)
    {
        double sumx = 0;
        double sumy = 0;

        for (int i = 0; i < numVectors; i++)
        {
            vectors[i] = source[i];
            sumx += source[i].x;
            sumy += source[i].y;
        }

        float centerX = (float)((sumx / numVectors) + translate.x);
        float centerY = (float)((sumy / numVectors) + translate.y);

        // Apply translation.
        for (int i = 0; i < numVectors; i++)
        {
            vectors[i].x += (float)translate.x;
            vectors[i].y += (float)translate.y;
        }

        // Move to origin.
        for (int i = 0; i < numVectors; i++)
        {
            vectors[i].x -= centerX;
            vectors[i].y -= centerY;
        }

        double sina = sin(angle), cosa = cos(angle);

        // Rotate.
        for (int i = 0; i < numVectors; i++)
        {
            float y = (float)(vectors[i].y * cosa - vectors[i].x * sina);
            float x = (float)(vectors[i].y * sina + vectors[i].x * cosa);
            vectors[i].y = y;
            vectors[i].x = x;
        }

        // Move back.
        for (int i = 0; i < numVectors; i++)
        {
            vectors[i].x += centerX;
            vectors[i].y += centerY;
        }
    }

    template<typename T>
    inline void RotateMesh(T mesh, const UINT group, const NTVERTEX* verts, const double angle)
    {
        if (NULL == mesh) return;

        GROUPEDITSPEC change{};
        NTVERTEX delta[4];

        TransformXY2d(verts, delta, 4, _V(0.0, 0.0, 0.0), angle);

        change.flags = GRPEDIT_VTXCRD;
        change.nVtx = 4;
        change.vIdx = NULL; //Just use the mesh order
        change.Vtx = delta;
        oapiEditMeshGroup(mesh, group, &change);
    }
}