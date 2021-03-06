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

#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\OnOffSwitch.h"
#include "bc_orbiter\Circuit.h"
#include "bc_orbiter\TextureVisual.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\IAnimationState.h"
#include "bc_orbiter\VCGauge.h"
#include "bc_orbiter\VCToggleSwitch.h"

#include "FuelCell.h"
#include "SR71r_mesh.h"

#include <map>

namespace bco = bc_orbiter;

class FuelCell;

/**	Power System
	Models the main power circuit for the ship.  Contains a 'Circuit' class which all of 
	the powered components of the ship are attached to.  At each step the circuit is queried
	to determine what the current amp draw is.  Changes to the power availability are also
	communicated to the components via the circuit.

	Three energy sources are available:
	- External: when parked on the ground.
	- Fuel cell:
	- Backup battery: <not imlemented>
		Battery - 50 amp hour should last 40 minutes with nothing on.

	Enable power:
	To enable power you need to attach an available power source to the main circuit.  For
	external and fuel cell, there is an 'AVAIL' light that will come on when that power
	source is available.  The backup battery will engage with the main power switch is on 
	but no power source is attached to the main circuit.  The [BATT] light will illuminate
	in the message panel when power is being drawn off of the backup battery.

	Configuration:
	POWER a b c d e
	a - Main power switch			0/1 - 0ff/on.
	b - External power attached		0/1 - off/attached.
	c - Fuel cell attached			0/1 - off/attached.
	d - Buss voltage				double 27.0
	e - Battery level				double (0 - 1)
*/
class PowerSystem :	public bco::PoweredComponent,
	public bco::IAnimationState
{
public:
	PowerSystem(bco::BaseVessel* vessel);

	virtual void SetClassCaps() override;
	virtual bool LoadVC(int id) override;
	virtual bool VCRedrawEvent(int id, int event, SURFHANDLE surf) override;
	virtual bool LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	virtual void SaveConfiguration(FILEHANDLE scn) const override;

	void Step(double simt, double simdt, double mjd);

    void PostCreation();

	void AddMainCircuitDevice(bco::PoweredComponent* device);

	bool IsExternalSourceAvailable()				{ return fabs(FULL_POWER - powerExternal_) < 5.0; }
	bool IsExternalSourceConnected()				{ return IsExternalSourceAvailable() && swConnectExternal_.IsOn(); }

	bool IsFuelCellAvailable();
	bool IsFuelCellConnected();

	bool IsBatteryPower()							{ return isBatteryDraw_; }

	double GetVoltsLevel()							{ return mainCircuit_.GetVoltLevel(); }
	double GetAmpDraw()								{ return mainCircuit_.GetTotalAmps(); }

	double AmpNeedlePosition()	const   			{ return mainCircuit_.GetTotalAmps() / 90; }
	double VoltNeedlePosition() const				{ return mainCircuit_.GetVoltLevel() / 30; }

	void SetFuelCell(FuelCell* fc)                  { fuelCell_ = fc; }

	// Animation state
	virtual double GetState() const override { return AmpNeedlePosition(); }


private:
	const double			FULL_POWER		=  28.0;
	const double			USEABLE_POWER	=  24.0;
	const double			AMP_OVERLOAD	= 100.0;

	void Update();

	int						areaId_;

	bco::Circuit			mainCircuit_;

	// Power ports:
	double					powerExternal_;
	FuelCell*				fuelCell_;
	const char*				ConfigKey = "POWER";

	double					voltMeterPosition_;
	bool					isBatteryDraw_;
	double					batteryLevel_;
	double					prevTime_;
	double					prevAvailPower_;

	bco::TextureVisual	    externAvailLight_;
	bco::TextureVisual	    externConnectedLight_;
	bco::TextureVisual	    fuelCellAvailLight_;
	bco::TextureVisual	    fuelCellConnectedLight_;


    bco::VCToggleSwitch     swPower_                    {   bt_mesh::SR71rVC::swMainPower_id, 
                                                            bt_mesh::SR71rVC::swMainPower_location, 
                                                            bt_mesh::SR71rVC::PowerTopRightAxis_location 
                                                        };

    bco::VCToggleSwitch     swConnectExternal_          {   bt_mesh::SR71rVC::swConnectExternalPower_id,     
                                                            bt_mesh::SR71rVC::swConnectExternalPower_location, 
                                                            bt_mesh::SR71rVC::PowerBottomRightAxis_location 
                                                        };

    bco::VCToggleSwitch     swConnectFuelCell_          {   bt_mesh::SR71rVC::swConnectFuelCell_id, 
                                                            bt_mesh::SR71rVC::swConnectFuelCell_location, 
                                                            bt_mesh::SR71rVC::PowerBottomRightAxis_location 
                                                        };

    bco::VCGauge            gaugePowerVolt_				{  {bt_mesh::SR71rVC::gaugeVoltMeter_id },
                                                            bt_mesh::SR71rVC::VoltMeterFrontAxis_location,   
															bt_mesh::SR71rVC::gaugeVoltMeter_location,
                                                            (120 * RAD), 
                                                            0.2
                                                        };

	bco::VCGauge            gaugePowerAmp_				{ { bt_mesh::SR71rVC::gaugeAmpMeter_id },
															bt_mesh::SR71rVC::gaugeAmpMeter_location,
															bt_mesh::SR71rVC::VoltMeterFrontAxis_location,
															(120 * RAD),
															0.2
														};

};