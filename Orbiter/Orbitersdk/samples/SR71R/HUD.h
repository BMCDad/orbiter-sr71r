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
#include "bc_orbiter\TextureVisual.h"
#include "bc_orbiter\PushButtonSwitch.h"
#include "bc_orbiter\VCRotorSwitch.h"

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

	virtual void SetClassCaps() override;

	/**
	Override to manage power based on vessel HUD state.
	*/
	virtual double CurrentDraw() override;

	virtual bool LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	virtual void SaveConfiguration(FILEHANDLE scn) const override;

	void ChangePowerLevel(double newLevel) override;

	virtual bool LoadVC(int id) override;

	bool DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

	void OnHudMode(int mode);

private:
	void SwitchPositionChanged(int mode);


    const char*	        ConfigKey = "HUDSWITCH";

    int hudMode_{ HUD_NONE };
    int areaId_;

	bool isInternalTrigger_{ false };

	bco::VCRotorSwitch		swSelectMode_{	bt_mesh::SR71rVC::SwHUDMode_id,
											GetBaseVessel(),
											bt_mesh::SR71rVC::SwHUDMode_location,
											bt_mesh::SR71rVC::SwHUDSelectAxis_location,
											(120 * RAD)
	};
};
