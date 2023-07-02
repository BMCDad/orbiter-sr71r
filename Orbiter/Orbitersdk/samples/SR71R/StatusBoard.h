//	StatusBoard - SR-71r Orbiter Addon
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
#include "bc_orbiter\TextureVisual.h"

#include "LandingGear.h"
#include "CargoBayController.h"
#include "Canopy.h"
#include "PowerSystem.h"
#include "PropulsionController.h"
#include "APU.h"
#include "AirBrake.h"
#include "HoverEngines.h"
#include "RetroEngines.h"

class StatusBoard : public bco::PoweredComponent
{
public:
	StatusBoard(bco::BaseVessel* vessel, double amps);

	virtual void OnSetClassCaps() override;

	void Step(double simt, double simdt, double mjd);
	
	virtual bool OnVCMouseEvent(int id, int event) override { return false; }
	virtual bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf) override;
	virtual bool OnLoadVC(int id) override;

	void SetCargoBay(		CargoBayController* cb)	{ cargoBay_ = cb; }
    void SetCanopy(         Canopy* c)              { canopy_ = c; }
//	void SetAvionics(		Avionics* av)			{ avionics_ = av; }
	void SetPower(			PowerSystem* ps)		{ power_ = ps; }
	void SetPropulsion(		PropulsionController* pc)	{ propulsion_ = pc; }
	void SetAPU(			APU* ap)				{ apu_ = ap; }
	void SetAirBrake(		AirBrake* ab)			{ airBrake_ = ab; }
    void SetHover(          HoverEngines* h)        { hover_ = h; }
    void SetRetro(          RetroEngines* re)       { retro_ = re; }

private:
	void DrawDoorsMsg(DEVMESHHANDLE devMesh);
	void DrawKEASMsg(DEVMESHHANDLE devMesh);
	void DrawThrustLimit(DEVMESHHANDLE devMesh);
	void DrawBattery(DEVMESHHANDLE devMesh);
	void DrawAPU(DEVMESHHANDLE devMesh);
	void DrawDock(DEVMESHHANDLE devMesh);
	void DrawFuel(DEVMESHHANDLE devMesh);
	void DrawAirBrakeMsg(DEVMESHHANDLE devMesh);


	const double OnOffset = 0.0;
	const double OffOffset = 0.0361;
	const double ErrOffset = 0.0722;
	const double WrnOffset = 0.1083;

	double GetWarnMsgOffset(double state);

	double					prevTime;
	int						vcRedrawArea_;

	CargoBayController*		cargoBay_;
	bco::TextureVisual		msgCargoBay_;

    Canopy*                 canopy_;
    bco::TextureVisual		msgCanopy_;

	//Avionics*				avionics_;
	//bco::TextureVisual		msgKeasWarning_;

	PowerSystem*			power_;
	bco::TextureVisual		msgBattery_;

	PropulsionController*	propulsion_;
	bco::TextureVisual		msgThrustLimit_;

    RetroEngines*           retro_;
    bco::TextureVisual      msgRetroDoors_;

    HoverEngines*           hover_;
    bco::TextureVisual      msgHoverDoors_;

	APU*					apu_;
	bco::TextureVisual		msgApu_;

	bco::TextureVisual		msgDock_;
	bco::TextureVisual		msgFuel_;

	AirBrake*				airBrake_;
	bco::TextureVisual		msgAirBrake_;
};