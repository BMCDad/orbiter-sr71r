//	AirBrake - SR-71r Orbiter Addon
//	Copyright(C) 2016  Blake Christensen
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

#include "StdAfx.h"

#include "FCProgram.h"

void FC::Program::Init()
{
}

void FC::Program::Step(double simt, double simdt, double mjd)
{
    if (fabs(simt - prevUpdate_) > 0.2)
    {
        prevUpdate_ = simt;
    }
}

bool FC::Program::HandleKey(FC::GCKey key)
{
    return false;
}

