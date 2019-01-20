//	Avionics - SR-71r Orbiter Addon
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

#pragma once

#include "IAvionics.h"

namespace FC // Flight Computer
{
    enum GCKey
    {
        D0, D1, D2, D3, D4, D5, D6, D7, D8, D9,
        Decimal,
        PlusMinus,
        Clear,
        Enter,
        Previous, Next,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
		Home
    };

	enum FCProg
	{
		Main,
		Ascent,
		Orbit,
		ReEntry,
		Atmosphere
	};

    /**
    Define the interface the flight computer exposes to the programming layer.
    */
    class IComputer
    {
    public:
        virtual void        ClearScreen() = 0;
        virtual void        DisplayLine(int row, char* mask, ...) = 0;
		virtual void		DisplayText(int row, int col, const char* text) = 0;
        virtual int         DisplayCols() = 0;
        virtual int         DisplayRows() = 0;
		virtual double		GetScratchPad() = 0;
		virtual void		SetScratchPad(double value) = 0;
        virtual IAvionics*  Avionics() = 0;
		virtual void		ActivateProg(FCProg prog) = 0;
    };

	class IDisplay
	{
	public:
		virtual void		SetFuncLabel(GCKey key, const char* label) = 0;
		virtual void		DisplayText(int row, int col, const char* text) = 0;
	};
}