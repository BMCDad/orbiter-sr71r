//	Avionics - SR-71r Orbiter Addon
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

// REFACTOR : Move into bco.

class IAvionics
{
public:
    virtual bool	IsAeroActive() const = 0;
    virtual bool	IsAeroAtmoMode() const = 0;
    virtual double GetSetHeading() const = 0;
    virtual double GetSetCourse() const = 0;
    virtual DWORD GetNavSource() const = 0;
};