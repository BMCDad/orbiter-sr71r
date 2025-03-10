//	MfdMets - SR-71r Orbiter Addon
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


// VC MFD Mesh coordinates:

const double MFDY = 15.2765;

// Mesh button row center points (z)
const double MFDBRAD = 0.006;	// Radius to use for hit test.

// Texture button column left/right
// left
const int MFDLLTEXL = 16;
const int MFDLLTEXR = 76;
const int MFDLRTEXL = 556;
const int MFDLRTEXR = 616;

// right
const int MFDRLTEXL = 650;
const int MFDRLTEXR = 710;
const int MFDRRTEXL = 1190;
const int MFDRRTEXR = 1250;

// Texture button col top and bottom
const int MFD_B1TEX_TOP = 1547;
const int MFD_B1TEX_BOT = 1587;
const int MFD_B2TEX_TOP = 1603;
const int MFD_B2TEX_BOT = 1643;
const int MFD_B3TEX_TOP = 1660;
const int MFD_B3TEX_BOT = 1700;
const int MFD_B4TEX_TOP = 1715;
const int MFD_B4TEX_BOT = 1755;
const int MFD_B5TEX_TOP = 1772;
const int MFD_B5TEX_BOT = 1812;
const int MFD_B6TEX_TOP = 1828;
const int MFD_B6TEX_BOT = 1868;
const int MFD_BPTEX_TOP = 1778;
const int MFD_BPTEX_BOT = 1838;

const int MFD_BUTTON_WIDTH = MFDLLTEXR - MFDLLTEXL;
const int MFD_BUTTON_HEIGHT = 60;
