//	Component Common - SR-71r Orbiter Addon
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
#include "..\bc_orbiter\OnOffInput.h"


namespace sr71_common
{
    namespace status
    {
        constexpr double on = 0.0;
        constexpr double off = 1.0;
        constexpr double error = 2.0;
        constexpr double warn = 3.0;
    }

    namespace panel
    {
        constexpr int main = 0;
        constexpr int right = 1;
    }

    namespace vc
    {
        constexpr int main = 0;
    }

    /* Control data for on/off up/down physical toggle switches */
    const bc_orbiter::OnOffInputMeta toggleOnOff{
           1.5708,              // Rotation angle (RAD)
          10.0,                 // Anim speed
           0.0,                 // anim start
           1.0,                 // anim end
           0.01,                // VC hit radius
           0.0148,              // Panel offset
          PANEL_REDRAW_NEVER,   // VCRedrawFlags
          PANEL_MOUSE_LBDOWN,   // vcMouseFlag
          PANEL_REDRAW_MOUSE,   // panel redraw flag
          PANEL_MOUSE_LBDOWN    // panel mouse flag
    };

}