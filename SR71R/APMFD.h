//	APMFD - SR-71r Orbiter Addon
//	Copyright(C) 2024  Blake Christensen
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
#include "SR71Vessel.h"

class SR71Vessel;

class APMFD : public MFD2
{
public:
    APMFD(DWORD w, DWORD h, VESSEL* vessel);
    ~APMFD();

    bool Update(oapi::Sketchpad* skp) override;
    static int MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

private:
    SR71Vessel* sr71Vessel_;

};