//	Lights - SR-71r Orbiter Addon
//	Copyright(C) 2016  Blake Christensen
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

#include "StdAfx.h"

#include "Orbitersdk.h"
#include "Lights.h"
#include "SR71r_mesh.h"

#include <assert.h>

Lights::Lights(bco::BaseVessel* vessel, double amps) :
	bco::PoweredComponent(vessel, amps, 25.0)
{
    ampsPerLight_ = amps;

	swNav_.OnFunction([this] {Update(); });
    swNav_.OffFunction([this] {Update(); });

	swBeacon_.OnFunction([this] {Update(); });
    swBeacon_.OffFunction([this] {Update(); });

	swStrobe_.OnFunction([this] {Update(); });
    swStrobe_.OffFunction([this] {Update(); });

	swDock_.OnFunction([this] {Update(); });
    swDock_.OffFunction([this] {Update(); });

	Update();
}

void Lights::SetClassCaps()
{
    auto vessel = GetBaseVessel();

    swNav_.Setup(vessel);
    swBeacon_.Setup(vessel);
    swStrobe_.Setup(vessel);
    swDock_.Setup(vessel);


	// Set light specs:
	static VECTOR3 colRed       = { 1.0, 0.5, 0.5 };
	static VECTOR3 colGreen     = { 0.5, 1.0, 0.5 };
	static VECTOR3 colWhite     = { 1.0, 1.0, 1.0 };

	// Beacons
	specBeaconTop_.col          = &colRed;
	specBeaconTop_.duration     = 0.1;
	specBeaconTop_.falloff      = 0.6;
	specBeaconTop_.pos          = const_cast<VECTOR3*>(&bt_mesh::SR71r::BeaconTop_location);
	specBeaconTop_.shape        = BEACONSHAPE_STAR;
	specBeaconTop_.size         = 0.55;
	specBeaconTop_.tofs         = 0.0;
	specBeaconTop_.period       = 2.0;

	specBeaconBottom_.col       = &colRed;
	specBeaconBottom_.duration  = 0.1;
	specBeaconBottom_.falloff   = 0.6;
	specBeaconBottom_.pos       = const_cast<VECTOR3*>(&bt_mesh::SR71r::BeaconBottom_location);
	specBeaconBottom_.shape     = BEACONSHAPE_STAR;
	specBeaconBottom_.size      = 0.55;
	specBeaconBottom_.tofs      = 0.2;
	specBeaconBottom_.period    = 2.0;

	// Nav lights
	specNavLeft_.col            = &colRed;
	specNavLeft_.duration       = 0.1;
	specNavLeft_.falloff        = 0.4;
	specNavLeft_.pos            = const_cast<VECTOR3*>(&bt_mesh::SR71r::NavLightP_location);
	specNavLeft_.shape          = BEACONSHAPE_DIFFUSE;
	specNavLeft_.size           = 0.3;
	specNavLeft_.tofs           = 0.2;
	specNavLeft_.period         = 0.0;

	specNavRight_.col           = &colGreen;
	specNavRight_.duration      = 0.1;
	specNavRight_.falloff       = 0.4;
	specNavRight_.pos           = const_cast<VECTOR3*>(&bt_mesh::SR71r::NavLightS_location);
	specNavRight_.shape         = BEACONSHAPE_DIFFUSE;
	specNavRight_.size          = 0.3;
	specNavRight_.tofs          = 0.2;
	specNavRight_.period        = 0.0;

	specNavRear_.col            = &colWhite;
	specNavRear_.duration       = 0.1;
	specNavRear_.falloff        = 0.4;
	specNavRear_.pos            = const_cast<VECTOR3*>(&bt_mesh::SR71r::NavLightTail_location);
	specNavRear_.shape          = BEACONSHAPE_DIFFUSE;
	specNavRear_.size           = 0.3;
	specNavRear_.tofs           = 0.2;
	specNavRear_.period         = 0.0;

	// Strobe
	specStrobeLeft_.col         = &colWhite;
	specStrobeLeft_.duration    = 0.05;
	specStrobeLeft_.falloff     = 0.6;
	specStrobeLeft_.pos         = const_cast<VECTOR3*>(&bt_mesh::SR71r::StrobeLightP_location);
	specStrobeLeft_.shape       = BEACONSHAPE_DIFFUSE;
	specStrobeLeft_.size        = 0.3;
	specStrobeLeft_.tofs        = 0.5;
	specStrobeLeft_.period      = 1.13;

	specStrobeRight_.col        = &colWhite;
	specStrobeRight_.duration   = 0.05;
	specStrobeRight_.falloff    = 0.6;
	specStrobeRight_.pos        = const_cast<VECTOR3*>(&bt_mesh::SR71r::StrobeLightS_location);
	specStrobeRight_.shape      = BEACONSHAPE_DIFFUSE;
	specStrobeRight_.size       = 0.3;
	specStrobeRight_.tofs       = 0.6;
	specStrobeRight_.period     = 1.13;

	vessel->AddBeacon(&specBeaconTop_);
	vessel->AddBeacon(&specBeaconBottom_);
	
	vessel->AddBeacon(&specNavLeft_);
	vessel->AddBeacon(&specNavRight_);
	vessel->AddBeacon(&specNavRear_);
	
	vessel->AddBeacon(&specStrobeLeft_);
	vessel->AddBeacon(&specStrobeRight_);
}

double Lights::CurrentDraw()
{
	auto result = 0.0;

	if (!HasPower())
	{
		return 0.0;
	}

	result += (swNav_.IsOn())      ? ampsPerLight_ : 0.0;
	result += (swBeacon_.IsOn())   ? ampsPerLight_ : 0.0;
	result += (swStrobe_.IsOn())   ? ampsPerLight_ : 0.0;
	result += (swDock_.IsOn())     ? ampsPerLight_ : 0.0;

	return result;
}

void Lights::Step(double simt, double simdt, double mjd)
{
	Update();
}

bool Lights::LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
	if (_strnicmp(key, ConfigKey, 5) != 0)
	{
		return false;
	}

	int nav = 0;
	int beacon = 0;
	int dock = 0;
	int strobe = 0;

	sscanf_s(configLine + 6, "%i%i%i%i", &nav, &beacon, &strobe, &dock);

	swNav_.SetState((nav == 0) ? 0.0 : 1.0);
	swBeacon_.SetState((beacon == 0) ? 0.0 : 1.0);
	swStrobe_.SetState((strobe == 0) ? 0.0 : 1.0);
	swDock_.SetState((dock == 0) ? 0.0 : 1.0);

	return true;
}

void Lights::SaveConfiguration(FILEHANDLE scn) const
{
	char cbuf[256];
	auto a = (swNav_.GetState() == 0.0) ? 0 : 1;
	auto b = (swBeacon_.GetState() == 0.0) ? 0 : 1;
	auto c = (swStrobe_.GetState() == 0.0) ? 0 : 1;
	auto d = (swDock_.GetState() == 0.0) ? 0 : 1;

	sprintf_s(cbuf, "%i %i %i %i", a, b, c, d);
	oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}

void Lights::Update()
{
	specBeaconBottom_.active = HasPower() && swBeacon_.IsOn();
	specBeaconTop_.active = specBeaconBottom_.active;

	specNavLeft_.active = specNavRight_.active = specNavRear_.active = HasPower() && swNav_.IsOn();

	specStrobeLeft_.active = specStrobeRight_.active = HasPower() && swStrobe_.IsOn();
}