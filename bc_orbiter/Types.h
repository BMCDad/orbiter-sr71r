//	Types - SR-71r Orbiter Addon
//	Copyright(C) 2025  Blake Christensen
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

#include "OrbiterSDK.h"
#include <functional>

namespace bc_orbiter
{
    using FuncEventHandler = std::function<bool(int id, int event)>;

    struct VCEventTarget
    {
        VECTOR3     Location()  { return _V(0.0, 0.0, 0.0); }
        double      Radius()    { return 0.0; }
    };

    using FuncRedrawEvent = std::function<void(int id, int event, SURFHANDLE surf)>;
    using FuncRedrawVCEvent = std::function<void(int id, int event, SURFHANDLE surf, DEVMESHHANDLE mesh)>;

    using FuncOnChanged = std::function<void()>;

    /**
    Contains information needed to draw a font onto a surface.
    */
    struct FontInfo
    {
        SURFHANDLE surfSource;  // SURFHANDLE to the texture that contains the font image.
        int charWidth;          // Width of each character (fixed width).
        int charHeight;         // Hight of each character.
        int sourceX;            // Horizontal start of font image.
        int sourceY;            // Vertical start of font image.
        int blankX;             // Horizontal start of blank.
        int blankY;             // Vertical start of blank.
    };
}
