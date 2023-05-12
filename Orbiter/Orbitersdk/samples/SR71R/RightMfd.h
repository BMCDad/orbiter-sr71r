//	RightMFD - SR-71r Orbiter Addon
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

#include "Orbitersdk.h"

#include "bc_orbiter\MFDBase.h"
#include "bc_orbiter\Tools.h"

namespace bco = bc_orbiter;

class RightMFD : public bco::MFDBase
{
public:
	RightMFD(bco::BaseVessel* vessel, double amps);

	virtual void OnSetClassCaps() override;

	virtual bool OnVCMouseEvent(int id, int event) override;
	virtual bool OnLoadVC(int id) override;
	virtual bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf) override;

private:
	bco::FontInfo	vcFont_;
};