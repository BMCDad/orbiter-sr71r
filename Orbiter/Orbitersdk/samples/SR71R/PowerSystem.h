//	PowerSystem - SR-71r Orbiter Addon
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

#include "bc_orbiter/signals.h"
#include "bc_orbiter/on_off_input.h"
#include "bc_orbiter/on_off_display.h"
#include "bc_orbiter/rotary_display.h"
#include "bc_orbiter/status_display.h"

#include "FuelCell.h"
#include "SR71r_mesh.h"
#include "SR71r_common.h"

#include <map>

namespace bco = bc_orbiter;

class FuelCell;

/**	Power System
	Manages the connections and distribution of the three main power sources:
	- External connection (ship is stopped or docked)
	- Fuelcell
	- Battery
	Allows the pilot to select the power source to use, and monitors voltage availability and consumption.

	What a powered component needs to do:
	Hook a reciever slot up to the VoltLevelSignal() and an amp signal up to the AmpDrawSlot().
	On a change to the receiver slot, check that the new voltage level is adequate.
	On each step, report through the amp signal the current amp usage for that component.
*/
class PowerSystem :	
	  public bco::vessel_component
	, public bco::power_provider
	, public bco::post_step
	, public bco::manage_state
{
public:
	PowerSystem(bco::BaseVessel& vessel);

	// post_step
	void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override
	{
		double draw = 0.0;

		for each (auto & c in consumers_) {
			draw += c->amp_draw();
		}

		ampDraw_ = draw;
		gaugePowerAmps_.set_state(ampDraw_);
		Update(vessel);
	}

	// manage_state
	bool handle_load_state(const std::string& line) override;
	std::string handle_save_state() override;

	// Fuelcell:
	bco::slot<double>&		FuelCellAvailablePowerSlot()	{ return slotFuelCellAvailablePower_; }	// Volt quantity available from fuelcell.

	void attach_consumer(bco::power_consumer* consumer) override {
		consumers_.push_back(consumer);
	}

	double volts_available() const override { return prevVolts_; }
	double amp_load() const override { return ampDraw_; }

private:
	void Update(bco::BaseVessel& vessel);

	const double			FULL_POWER		=  28.0;
	const double			USEABLE_POWER	=  24.0;
	const double			AMP_OVERLOAD	= 100.0;

	std::vector<bco::power_consumer*>  consumers_;

	bco::signal<bool>		signalIsDrawingBattery_;
	bco::slot<double>		slotFuelCellAvailablePower_;

	double					ampDraw_{ 0.0 };			// Collects the total amps drawn during a step.
	double					batteryLevel_;
	bool					isDrawingBattery_{ false };
	double					prevStep_{ 0.0 };	
	double					prevVolts_{ -1.0 };

	bco::on_off_input		switchEnabled	{ { bm::vc::swMainPower_id },
												bm::vc::swMainPower_location, bm::vc::PowerTopRightAxis_location,
												toggleOnOff,
												bm::pnl::pnlPwrMain_id,
												bm::pnl::pnlPwrMain_verts,
												bm::pnl::pnlPwrMain_RC
											};

	bco::on_off_input		switchConnectExternal_ {
												{ bm::vc::swConnectExternalPower_id },
												bm::vc::swConnectExternalPower_location, bm::vc::PowerBottomRightAxis_location,
												toggleOnOff,
												bm::pnl::pnlPwrExtBus_id,
												bm::pnl::pnlPwrExtBus_verts,
												bm::pnl::pnlPwrExtBus_RC
											};

	bco::on_off_input		switchConnectFuelCell_ {
												{ bm::vc::swConnectFuelCell_id },
												bm::vc::swConnectFuelCell_location, bm::vc::PowerBottomRightAxis_location,
												toggleOnOff,
												bm::pnl::pnlPwrFCBus_id,
												bm::pnl::pnlPwrFCBus_verts,
												bm::pnl::pnlPwrFCBus_RC
											};

	bco::on_off_display		lightFuelCellConnected_ {
												bm::vc::FuelCellConnectedLight_id,
												bm::vc::FuelCellConnectedLight_verts,
												bm::pnl::pnlLgtFCPwrOn_id,
												bm::pnl::pnlLgtFCPwrOn_verts,
												0.0244
											};

	bco::on_off_display		lightExternalAvail_ {
												bm::vc::ExtAvailableLight_id,
												bm::vc::ExtAvailableLight_verts,
												bm::pnl::pnlLgtExtPwrAvail_id,
												bm::pnl::pnlLgtExtPwrAvail_verts,
												0.0244
											};

	bco::on_off_display		lightExternalConnected_ {
												bm::vc::ExtConnectedLight_id,
												bm::vc::ExtConnectedLight_verts,
												bm::pnl::pnlLgtExtPwrOn_id,
												bm::pnl::pnlLgtExtPwrOn_verts,
												0.0244
											};

	bco::rotary_display<bco::Animation>	gaugePowerVolts_{
												{ bm::vc::gaugeVoltMeter_id },
												bm::vc::gaugeVoltMeter_location, bm::vc::VoltMeterFrontAxis_location,
												bm::pnl::pnlVoltMeter_id,
												bm::pnl::pnlVoltMeter_verts,
												-(120 * RAD),
												0.2,
												[](double d) {return (d / 30); }	// Transform from volts to anim-range.
											};

	bco::rotary_display<bco::Animation>	gaugePowerAmps_{
												{ bm::vc::gaugeAmpMeter_id },
												bm::vc::gaugeAmpMeter_location, bm::vc::VoltMeterFrontAxis_location,
												bm::pnl::pnlAmpMeter_id,
												bm::pnl::pnlAmpMeter_verts,
												(120 * RAD),	// Clockwise
												0.2,
												[](double d) {return (d / 90); }	// Transform to amps.
											};

	bco::status_display     statusBattery_     {
		bm::vc::MsgLightBattery_id,
		bm::vc::MsgLightBattery_verts,
		bm::pnl::pnlMsgLightBattery_id,
		bm::pnl::pnlMsgLightBattery_verts,
		0.0361
	};
};