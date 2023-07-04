//	NavModes - SR-71r Orbiter Addon
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

#pragma

#include "bc_orbiter\bco.h"
#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\BaseVessel.h"

#include "SR71r_mesh.h"

#include <vector>
#include <map>

namespace bco = bc_orbiter;

/**
Models the nav mode selector function.
*/
class NavModes
{
public:
	NavModes(bco::BaseVessel& baseVessel);

	// These overrides come directly from SR71Vessel callbacks.
	void OnNavMode(int mode, bool active);
	bool DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

	bco::slot<bool>&					IsEnabledSlot()			{ return slotIsEnabled_; }
	bco::slot<int>&						NavButtonSlot()			{ return slotNavButton_; }		// Nav button inputs.
	bco::slot<bco::draw_hud_data>&		DrawHudSlot()			{ return slotDrawHud_; }		// Nav mode change notifications from vessel.
	
	bco::signal<bco::navmode_data>&		NavModeSignal()			{ return sigNavMode_; }

protected:
	void Update();

private:
	bco::BaseVessel&					baseVessel_;
	void ToggleMode(int mode);

	bco::slot<bool>						slotIsEnabled_;
	bco::slot<int>						slotNavButton_;
	bco::slot<bco::draw_hud_data>		slotDrawHud_;

	bco::signal<bco::navmode_data>		sigNavMode_;

	int		vcUIArea_{ 0 };
	int		panelUIArea_{ 0 };

	int		navMode1_{ 0 };	// HUD nav left area
	int		navMode2_{ 0 };	// HUD nav right area
};