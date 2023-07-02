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

#include "SR71r_mesh.h"

#include <vector>
#include <map>

namespace bco = bc_orbiter;

/**
Models the nav mode selector function.
*/
class NavModes : public bco::PoweredComponent
{
public:
	NavModes(bco::BaseVessel* vessel, double amps);

	// These overrides come directly from SR71Vessel callbacks.
	void OnNavMode(int mode, bool active);
	bool DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

	bco::slot<bool>&	IsKillRotSlot()			{ return slotIsKillRot_; }		// Receives button event, no state
	bco::slot<bool>&	IsHorzLevelSlot()		{ return slotIsHorzLvl_; }
	bco::slot<bool>&	IsProgradeSlot()		{ return slotIsPrograd_; }
	bco::slot<bool>&	IsRetroGradeSlot()		{ return slotIsRetroGr_; }
	bco::slot<bool>&	IsNormalSlot()			{ return slotIsNorm_; }		
	bco::slot<bool>&	IsAntiNormalSlot()		{ return slotIsAntNorm_; }

	bco::signal<bool>&	IsKillRotSignal()		{ return sigIsKillRot_; }
	bco::signal<bool>&	IsHorzLevelSignal()		{ return sigIsHorzLvl_; }
	bco::signal<bool>&	IsProgradeSignal()		{ return sigIsPrograd_; }
	bco::signal<bool>&	IsRetroGradeSignal()	{ return sigIsRetroGr_; }
	bco::signal<bool>&	IsNormalSignal()		{ return sigIsNorm_; }
	bco::signal<bool>&	IsAntiNormalSignal()	{ return sigIsAntNorm_; }

protected:
	void Update();

private:
	void ToggleMode(int mode);

	bco::slot<bool>		slotIsKillRot_;
	bco::slot<bool>		slotIsHorzLvl_;
	bco::slot<bool>		slotIsPrograd_;
	bco::slot<bool>		slotIsRetroGr_;
	bco::slot<bool>		slotIsNorm_;
	bco::slot<bool>		slotIsAntNorm_;

	bco::signal<bool>	sigIsKillRot_;
	bco::signal<bool>	sigIsHorzLvl_;
	bco::signal<bool>	sigIsPrograd_;
	bco::signal<bool>	sigIsRetroGr_;
	bco::signal<bool>	sigIsNorm_;
	bco::signal<bool>	sigIsAntNorm_;

	int		vcUIArea_{ 0 };
	int		panelUIArea_{ 0 };

	int		navMode1_{ 0 };	// HUD nav left area
	int		navMode2_{ 0 };	// HUD nav right area
};