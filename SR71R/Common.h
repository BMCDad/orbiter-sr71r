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
}