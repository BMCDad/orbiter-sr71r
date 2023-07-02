//	HUD - SR-71r Orbiter Addon
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

#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\BaseVessel.h"
#include "bc_orbiter\signals.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

/**
	Manage the heads up display (HUD).

	The HUD requires power from the main circuit to operate (see Power System to enable
	main power).  The HUD draw a constant amp level when powered on (one of the HUD modes
	is selected and main power is available).

	Configuration:
	The HUD mode is managed by Orbiter.
*/
class HUD : public bco::PoweredComponent
{
public:
	HUD(bco::BaseVessel* vessel, double amps);

	bool OnLoadVC(int id) override;

	/**
	Override to manage power based on vessel HUD state.
	*/
	virtual double CurrentDraw() override;

	void ChangePowerLevel(double newLevel) override;

	bool DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

	void OnHudMode(int mode);

	bco::slot<bool>&	DockModeSlot()		{ return slotDockMode_; }
	bco::slot<bool>&	OrbitModeSlot()		{ return slotOrbitMode_; }
	bco::slot<bool>&	SurfaceModeSlot()	{ return slotSurfaceMode_; }

	bco::signal<bool>&	DockModeSignal()	{ return sigDockMode_; }
	bco::signal<bool>&	OrbitModeSignal()	{ return sigOrbitMode_; }
	bco::signal<bool>&	SurfaceModeSignal()	{ return sigSurfaceMode_; }

private:
	void OnChanged(int mode);

	bco::slot<bool> slotDockMode_;
	bco::slot<bool> slotOrbitMode_;
	bco::slot<bool> slotSurfaceMode_;

	bco::signal<bool> sigDockMode_;
	bco::signal<bool> sigOrbitMode_;
	bco::signal<bool> sigSurfaceMode_;
};
