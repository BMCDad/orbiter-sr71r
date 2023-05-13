//	RCSSystem - SR-71r Orbiter Addon
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
#include "bc_orbiter\VCRotorSwitch.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class VESSEL3;

/** RCSMode
	
	Configure the RCS Mode switch position.  It is possible for the switch
	to be in teh LIN or ROT position, but the vessel is in 'None' mode.  This
	will happen when the vessel has no power.

	Configuration:
	RCSSWITCH a
	a - 0/1/2.  0=LIN, 1=RCS, 2=OFF
*/

class RCSSystem : public bco::PoweredComponent
{
public:
	RCSSystem(bco::BaseVessel* vessel, double amps);

	virtual void OnSetClassCaps() override;

	virtual bool OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
	virtual void OnSaveConfiguration(FILEHANDLE scn) const override;

	virtual void ChangePowerLevel(double newLevel) override;

	virtual bool OnLoadVC(int id) override;

	virtual double CurrentDraw() override;
	
	// Callback:
	void OnRCSMode(int mode);

private:
    void SwitchPositionChanged(int mode);

	const char*				ConfigKey = "RCSSWITCH";
    int						vcUIArea_;

	bool					isInternalTrigger_{ false };

    //bco::TextureVisual		visRCSRot_;
    //bco::TextureVisual		visRCSLin_;

    //bco::PushButtonSwitch   btnRotation_    {bt_mesh::SR71rVC::RCS_ROT_location,     0.01};
    //bco::PushButtonSwitch   btnLinear_      {bt_mesh::SR71rVC::RCS_LIN_location,     0.01};

	bco::VCRotorSwitch		swSelectMode_{	bm::vc::SwRCSMode_id,
											bm::vc::SwRCSMode_location,
											bm::vc::SwRCSSelectAxis_location,
											(90 * RAD)
	};
};