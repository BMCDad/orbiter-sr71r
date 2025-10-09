/*
IPowerProvider - SR-71r Orbiter Addon
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

class IPowerProvider
{
public:
    /**
     * \brief Get the current power level.
     * \return The current power level as a double.
     */
    virtual double GetPowerLevel() const = 0;
    
    /**
     * \brief Draw down the power by a specified amount.
     * \param amps The current power draw in amps.
     */
    virtual void DrawPower(double amps) = 0;

    /**
    * \brief Represents the current amp load on the power system.
    */
    virtual double CurrentAmps() const = 0;

    virtual ~IPowerProvider() {}
};