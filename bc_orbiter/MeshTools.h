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
    Contains the place values for a number.  Useful for things like altimeters.
    */
    struct TensParts
    {
        double Hundredths;
        double Tenths;
        double Tens;
        double Hundreds;
        double Thousands;
        double TenThousands;
        double HundredThousands;
        double Millions;
    };

    struct IntParts {
        double Ones;
        double Tens;
        double Hundreds;
        double Thousands;
        double TenThousands;
        double HundredThousands;
        double Millions;
    };

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

    inline void DrawPanelOnOff(MESHHANDLE mesh, UINT group, const NTVERTEX* verts, bool isOn, double offset)
    {
        auto grp = oapiMeshGroup(mesh, group);

        float trans = (float)(isOn ? (float)offset : 0.0);
        grp->Vtx[0].tu = verts[0].tu + trans;
        grp->Vtx[1].tu = verts[1].tu + trans;
        grp->Vtx[2].tu = verts[2].tu + trans;
        grp->Vtx[3].tu = verts[3].tu + trans;
    }

    template<typename T>
    inline void TranslateMesh(T mesh, const UINT group, const NTVERTEX* verts, const VECTOR3& trans)
    {
        if (NULL == mesh)
        {
            return;
        }

        GROUPEDITSPEC change{};
        NTVERTEX delta[4];

        TransformXY2d(verts, delta, 4, trans, 0.0);

        change.flags = GRPEDIT_VTXCRD;
        change.nVtx = 4;
        change.vIdx = NULL; //Just use the mesh order
        change.Vtx = delta;
        oapiEditMeshGroup(mesh, group, &change);
    }

    /**
    Transforms the tu and tv members of the source NTVERTEX array based on the translate and angle parameters
    and returns the transformed array in the vectors parameter.  This is useful for translating or rotating a mesh's
    texture for	animating a panel surface.
    @param source The NTVERTEX array to transform.
    @param vectors The resulting NTVERTEX array.
    @param numVectors The number of vectors in both source and vectors.
    @param translate A vector whose x and y members indicate the translation to apply.
    @param angle The angle in radians to rotate the x and y coordinates.  The center is calculated average
    of the x and y values in source.
    */
    inline void TransformUV2d(const NTVERTEX* source, NTVERTEX* vectors, int numVectors, const VECTOR3& translate, double angle)
    {
        double sumu = 0;
        double sumv = 0;

        for (int i = 0; i < numVectors; i++)
        {
            vectors[i] = source[i];
            sumu += source[i].tu;
            sumv += source[i].tv;
        }

        float centerU = (float)((sumu / numVectors) + translate.x);
        float centerV = (float)((sumv / numVectors) + translate.y);

        // Apply translation.
        for (int i = 0; i < numVectors; i++)
        {
            vectors[i].tu += (float)translate.x;
            vectors[i].tv += (float)translate.y;
        }

        // Move to origin.
        for (int i = 0; i < numVectors; i++)
        {
            vectors[i].tu -= (float)centerU;
            vectors[i].tv -= (float)centerV;
        }

        double sina = sin(angle), cosa = cos(angle);

        // Rotate.
        for (int i = 0; i < numVectors; i++)
        {
            float v = (float)(vectors[i].tv * cosa - vectors[i].tu * sina);
            float u = (float)(vectors[i].tv * sina + vectors[i].tu * cosa);
            vectors[i].tv = v;
            vectors[i].tu = u;
        }

        // Move back.
        for (int i = 0; i < numVectors; i++)
        {
            vectors[i].tu += centerU;
            vectors[i].tv += centerV;
        }
    }

    template<typename T>
    inline void TransformUV(T mesh, const UINT group, const NTVERTEX* verts, const double angle, const VECTOR3& trans)
    {
        if (NULL == mesh)
        {
            return;
        }

        GROUPEDITSPEC change{};
        NTVERTEX delta[4];

        TransformUV2d(verts, delta, 4, trans, angle);

        change.flags = GRPEDIT_VTXTEX;
        change.nVtx = 4;
        change.vIdx = NULL; //Just use the mesh order
        change.Vtx = delta;
        oapiEditMeshGroup(mesh, group, &change);
    }

    /**
    * Translates the UV coordinates of a quad in a mesh group by the specified x and y offsets.
    * This method assumes the quad is defined by 4 vertices in the NTVERTEX array.
    */
    inline void TranslateUVQuad(MESHHANDLE mesh, UINT group, const NTVERTEX* verts, double x, double y)
    {
         if (NULL == mesh)
         {
               return;
         }
   
         GROUPEDITSPEC change{};
         NTVERTEX delta[4];
   
         for (int i = 0; i < 4; i++)
         {
               delta[i] = verts[i];
               delta[i].tu += (float)x;
               delta[i].tv += (float)y;
         }
   
         change.flags = GRPEDIT_VTXTEX;
         change.nVtx = 4;
         change.vIdx = NULL; //Just use the mesh order
         change.Vtx = delta;
         oapiEditMeshGroup(mesh, group, &change);
    }

    /**
    Breaks the number into its tenparts components.  Each
    part will be a whole number plus a hundredths part.  This is useful
    for 'barrel' numbers.
    */
    inline void GetDigits(double number, TensParts& out)
    {
#pragma warning(push)
#pragma warning(disable : 6031)
        // fmod(num, den) returns the remainder of num/den rounded to 0.
        // so 123, for example, is .3.

        // Start by multiplying by 100.  This moves hundreths into
        // the whole number range.  So, for example, the incoming number
        // is 234.56, that becomes 23456.
        double numberIn = number * 100;	// Move hundreths into the whole number.

        // Now get fmod.  In our example, this is '6'.  23456 / 10 = 2345.[6]
      //out.Hundredths = fmod(numberIn, 10.0);
        modf(fmod(numberIn, 10.0), &out.Hundredths);

        // Divide by 10 and keep going.
        numberIn /= 10;
        //out.Tenths = fmod(numberIn, 10.0);
        modf(fmod(numberIn, 10.0), &out.Tenths);

        numberIn /= 10;
        //out.Tens = fmod(numberIn, 10.0);
        modf(fmod(numberIn, 10.0), &out.Tens);

        numberIn /= 10;
        //out.Hundreds = fmod(numberIn, 10.0);
        modf(fmod(numberIn, 10.0), &out.Hundreds);

        numberIn /= 10;
        //out.Thousands = fmod(numberIn, 10.0);
        modf(fmod(numberIn, 10.0), &out.Thousands);

        numberIn /= 10;
        modf(fmod(numberIn, 10.0), &out.TenThousands);

        numberIn /= 10;
        modf(fmod(numberIn, 10.0), &out.HundredThousands);

        numberIn /= 10;
        modf(fmod(numberIn, 10.0), &out.Millions);

#pragma warning(pop)
    }

    /**
    Breaks a number into its component 10s parts.
    @param number The input number
    @param out Returns the number's tens parts.
    */
    inline void BreakTens(double number, IntParts& out) {
        // Convert the number to an integer to work with whole parts
        int integerPart = static_cast<int>(number);

        // Calculate millions
        out.Millions = integerPart / static_cast<double>(1000000);
        integerPart %= 1000000;

        // Calculate hundred thousands
        out.HundredThousands = integerPart / static_cast<double>(100000);
        integerPart %= 100000;

        // Calculate ten thousands
        out.TenThousands = integerPart / static_cast<double>(10000);
        integerPart %= 10000;

        // Calculate thousands
        out.Thousands = integerPart / static_cast<double>(1000);
        integerPart %= 1000;

        // Calculate hundreds
        out.Hundreds = integerPart / static_cast<double>(100);
        integerPart %= 100;

        // Calculate tens
        out.Tens = integerPart / static_cast<double>(10);
    }

    // Indicates how to draw text.  See DrawSurfaceText.
    enum DrawTextFormat { Left, Right, Center };

    /**
    Blank surface text.  Clears out MFD text.
    */
    inline void DrawBlankText(int x, int y, SURFHANDLE surfTarget, FontInfo& font)
    {
        int xCurrent = x;

        oapiBlt(
            surfTarget,
            font.surfSource,
            xCurrent, y,
            font.blankX, font.blankY,
            3 * font.charWidth,
            font.charHeight);
    }

    /**
    \brief Draws the text string in 'text' onto a SURFHANDLE target at x, y position using font info from the 'font'.
    \param x The horizontal location for the text.
    \param y The veritcal location for the text.
    \param text The text string to draw on the surface.
    \param format How to align the text: left|center|right.
    \param surfTarget The SURFHANDLE to draw the string on.
    \param font The FontInfo used for drawing.
    */
    inline void DrawSurfaceText(int x, int y, const char* text, DrawTextFormat format, SURFHANDLE surfTarget, FontInfo& font)
    {
        int len = strlen(text);
        int blockLen = len * font.charWidth;
        int xCurrent = x;

        switch (format)
        {
        case DrawTextFormat::Right:
            xCurrent = x - blockLen;
            break;

        case DrawTextFormat::Center:
            xCurrent = x - (blockLen / 2);
            break;
        }

        oapiBlt(
            surfTarget,
            font.surfSource,
            xCurrent, y,
            font.blankX, font.blankY,
            len * font.charWidth,
            font.charHeight);

        for (int i = 0; i < len; i++)
        {
            char l = text[i] - 32;

            if (l >= 0)
            {
                int fo = (l * font.charWidth) + font.sourceX;
                oapiBlt(
                    surfTarget,
                    font.surfSource,
                    xCurrent, y,
                    fo, font.sourceY,
                    font.charWidth, font.charHeight);

                xCurrent += font.charWidth;
            }
        }
    }
}