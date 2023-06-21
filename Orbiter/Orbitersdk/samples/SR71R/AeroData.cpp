//	AeroData - SR-71r Orbiter Addon
//	Copyright(C) 2023  Blake Christensen
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

#include "bc_orbiter\BaseVessel.h"

#include "AeroData.h"

AeroData::AeroData() :
	vessel_component(),
	post_step(),
	enabledSlot_([&](bool v) {}),
	avionicsModeSlot_([&](bool v) { avionicsModeSignal_.fire(v ? AvionMode::AvionAtmo : AvionMode::AvionExo); })
{}


// post_step
void AeroData::handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) {
}