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
#include "Ascent.h"


class SR71Vessel;

class APMFD : public MFD2
{
public:
    APMFD(DWORD w, DWORD h, VESSEL* vessel);
    ~APMFD();

    bool Update(oapi::Sketchpad* skp) override;
    char* ButtonLabel(int bt) override;
    int ButtonMenu(const MFDBUTTONMENU** menu) const override;
    bool ConsumeButton(int bt, int event) override;

    static int MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

private:
    SR71Vessel* sr71Vessel_;

    Ascent* ascent_;

    std::map<int, std::string> labels_{
        {0, "ZRO"},
        {1, "ONE"},
        {2, "TWO"},
        {3, "THR"},
        {4, "FOR"},
        {5, "FVE"},
        {6, "SIX"},
        {7, "SVN"},
        {8, "EGT"},
        {9, "NIN"},
        {10, "TEN"},
        {11, "ELV"}
    };

    std::vector<MFDBUTTONMENU> menu_{
        {"Zero Option", nullptr, 'A'},
        {"First Option", nullptr, 'B'},
        {"Second Option", nullptr, 'C'},
        {"Third Option", nullptr, 'D'},
        {"Fourth Option", nullptr, 'E'},
        {"Fifth Option", nullptr, 'F'},
        {"Sixth Option", nullptr, 'G'},
        {"Seventh Option", nullptr, 'H'},
        {"Eighth Option", nullptr, 'I'},
        {"Nineth Option", nullptr, 'J'},
        {"Tenth Option", nullptr, 'K'},
        {"Eleventh Option", nullptr, 'L'}
    };
};