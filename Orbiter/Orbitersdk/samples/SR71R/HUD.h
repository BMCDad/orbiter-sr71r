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

#include "bc_orbiter/Animation.h"
#include "bc_orbiter/vessel.h"
#include "bc_orbiter/control.h"
#include "bc_orbiter/on_off_display.h"
#include "bc_orbiter/simple_event.h"

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
class HUD : 
	  public bco::vessel_component
	, public bco::power_consumer
	, public bco::load_vc
	, public bco::draw_hud
{
public:
	HUD(bco::power_provider& pwr, bco::vessel& vessel);

	bool handle_load_vc(bco::vessel& vessel, int vcid) override;

	void handle_draw_hud(bco::vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;

	double amp_draw() const override { return IsPowered() ? 4.0 : 0.0; }

	void OnHudMode(int mode);

	//bco::slot<bool>&	DockModeSlot()		{ return slotDockMode_; }
	//bco::slot<bool>&	OrbitModeSlot()		{ return slotOrbitMode_; }
	//bco::slot<bool>&	SurfaceModeSlot()	{ return slotSurfaceMode_; }

	//bco::signal<bool>&	DockModeSignal()	{ return sigDockMode_; }
	//bco::signal<bool>&	OrbitModeSignal()	{ return sigOrbitMode_; }
	//bco::signal<bool>&	SurfaceModeSignal()	{ return sigSurfaceMode_; }

private:
	bco::power_provider& power_;

	bool IsPowered() const {	return power_.volts_available() > 24.0; }

	void OnChanged(int mode);

	//bco::slot<bool> slotDockMode_;
	//bco::slot<bool> slotOrbitMode_;
	//bco::slot<bool> slotSurfaceMode_;

	//bco::signal<bool> sigDockMode_;
	//bco::signal<bool> sigOrbitMode_;
	//bco::signal<bool> sigSurfaceMode_;

	// *** HUD *** 
	bco::simple_event<>		btnDocking_	{		bm::vc::vcHUDDock_location,
												0.01,
												bm::pnl::pnlHUDDock_RC
											};

	bco::on_off_display		btnLightDocking_ {	bm::vc::vcHUDDock_id,
												bm::vc::vcHUDDock_verts,
												bm::pnl::pnlHUDDock_id,
												bm::pnl::pnlHUDDock_verts,
												0.0352
											};

	bco::simple_event<>		btnOrbit_ {			bm::vc::vcHUDOrbit_location,
												0.01,
												bm::pnl::pnlHUDOrbit_RC
											};

	bco::on_off_display		btnLightOrbit_ {	bm::vc::vcHUDOrbit_id,
												bm::vc::vcHUDOrbit_verts,
												bm::pnl::pnlHUDOrbit_id,
												bm::pnl::pnlHUDOrbit_verts,
												0.0352
											};

	bco::simple_event<>		btnSurface_ {		bm::vc::vcHUDSURF_location,
												0.01,
												bm::pnl::pnlHUDSurf_RC
										};

	bco::on_off_display		btnLightSurface_ {	bm::vc::vcHUDSURF_id,
												bm::vc::vcHUDSURF_verts,
												bm::pnl::pnlHUDSurf_id,
												bm::pnl::pnlHUDSurf_verts,
												0.0352
											};
};
