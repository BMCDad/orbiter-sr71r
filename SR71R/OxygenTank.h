//	OxygenTank - SR-71r Orbiter Addon
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

#pragma once

#include "../bc_orbiter/control.h"
#include "../bc_orbiter/OnOffInput.h"
#include "../bc_orbiter/RotaryDisplay.h"
#include "../bc_orbiter/SimpleEvent.h"
#include "../bc_orbiter/Vessel.h"
#include "../bc_orbiter/VesselTextureElement.h"

#include "SR71r_mesh.h"
#include "SR71r_common.h"
#include "Common.h"
#include "ShipMets.h"

namespace bco = bc_orbiter;
namespace cmn = sr71_common;

class OxygenTank :
    public bco::GenericTank
{
public:
    OxygenTank(bco::PowerProvider& pwr, bco::Vessel& vessel) :
        bco::GenericTank(pwr, O2_SUPPLY, OXYGEN_FILL_RATE),
        vessel_(vessel)
    {
        vessel.AddControl(&gaugeLevel_);
        vessel.AddControl(&lightAvailable_);
        vessel.AddControl(&btnFill_);
        vessel.AddControl(&btnLightFill_);

        btnFill_.attach([&]() { ToggleFilling(); });

        //LevelSignal().attach(		gaugeLevel_.Slot());
        //IsFillingSignal().attach(	btnLightFill_.Slot());
        //IsAvailableSignal().attach(	lightAvailable_.Slot());
    }

    void UpdateLevel(double l) override { gaugeLevel_.set_state(l); }

    void UpdateIsFilling(bool b) override {
        btnLightFill_.set_state(vessel_, b);
    }

    void UpdateIsAvailable(bool b) override {
        lightAvailable_.set_state(vessel_, b);
    }

    double AmpDraw() const override {
        return GenericTank::AmpDraw() + (IsPowered() ? 5.0 : 0.0);	// Cryo cooling.
    }

private:
    bco::Vessel& vessel_;

    // ***  HYDROGEN SUPPLY  *** //
    bco::RotaryDisplay<bco::AnimationTarget> gaugeLevel_{
        { bm::vc::gaugeOxygenLevel_id },
        bm::vc::gaugeOxygenLevel_loc, bm::vc::axisOxygenLevel_loc,
        bm::pnlright::pnlLOXPress_id,
        bm::pnlright::pnlLOXPress_vrt,
        (300 * RAD),	// Clockwise
        0.2,
        1
    };

    bco::VesselTextureElement       lightAvailable_ {
        bm::vc::LOXSupplyOnLight_id,
        bm::vc::LOXSupplyOnLight_vrt,
        cmn::vc::main,
        bm::pnlright::pnlO2Avail_id,
        bm::pnlright::pnlO2Avail_vrt,
        cmn::panel::right
    };

    bco::SimpleEvent<> btnFill_{
        bm::vc::LOXValveOpenSwitch_loc,
        0.01,
        0,
        bm::pnlright::pnlO2Switch_RC,
        1
    };

    bco::VesselTextureElement       btnLightFill_ {
        bm::vc::LOXValveOpenSwitch_id,
        bm::vc::LOXValveOpenSwitch_vrt,
        cmn::vc::main,
        bm::pnlright::pnlO2Switch_id,
        bm::pnlright::pnlO2Switch_vrt,
        cmn::panel::right
    };
};

