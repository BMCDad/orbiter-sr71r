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

#pragma

#include "bc_orbiter\control.h"
#include "bc_orbiter\signals.h"


namespace bco = bc_orbiter;

class AeroData :
	public bco::vessel_component,
	public bco::post_step {

public:
	enum AvionMode { AvionAtmo, AvionExo };

	AeroData();
	~AeroData() {}

	// post_step
	void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override;

	// Slots:
	bco::slot<bool>&		EnabledSlot()			{ return enabledSlot_; }
	bco::slot<bool>&		AvionicsModeSlot()		{ return avionicsModeSlot_;	}

	// Signals:
	bco::signal<AvionMode>& AvionicsModeSignal()	{ return avionicsModeSignal_; }
private:
	// Slots:
	bco::slot<bool>			enabledSlot_;				// Main avion power switch.
	bco::slot<bool>			avionicsModeSlot_;

	// Signals:
	bco::signal<AvionMode>	avionicsModeSignal_;
};
