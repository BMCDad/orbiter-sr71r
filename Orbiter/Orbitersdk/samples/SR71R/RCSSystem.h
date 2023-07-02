//	RCSSystem - SR-71r Orbiter Addon
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

#include "bc_orbiter\PoweredComponent.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class VESSEL3;

/** RCSMode
*/

class RCSSystem : public bco::PoweredComponent
{
public:
	RCSSystem(bco::BaseVessel* vessel, double amps);

	virtual void ChangePowerLevel(double newLevel) override;
	virtual double CurrentDraw() override;
	
	// Callback:
	void OnRCSMode(int mode);

	bco::slot<bool>&	ToggleLinearSlot()	{ return slotToggleLinear_; }
	bco::slot<bool>&	ToggleRotateSlot()	{ return slotToggleRotate_; }

	bco::signal<bool>&	IsLinearSignal()	{ return sigIsLinear_; }
	bco::signal<bool>&	IsRotateSignal()	{ return sigIsRotate_; }

private:
    void OnChanged(int mode);

	bco::slot<bool>		slotToggleLinear_;
	bco::slot<bool>		slotToggleRotate_;

	bco::signal<bool>	sigIsLinear_;
	bco::signal<bool>	sigIsRotate_;
};