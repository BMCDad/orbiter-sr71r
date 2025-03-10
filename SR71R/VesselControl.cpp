//	AutoPilot - SR-71r Orbiter Addon
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

//#include "VesselControl.h"
//
//VesselControl::VesselControl(
//      bco::vessel& vessel
//    , bco::power_provider& pwr
//    , bco::avionics_provider& ap
//    , bco::propulsion_control& pc
//    , bco::surface_control& sc
//    ) :
//      power_(pwr)
//    , avionics_(ap)
//    , propulsion_(pc)
//    , surface_(sc)
//    , prgHoldAltitude_(ap, pc, sc)
//    , prgHoldHeading_(ap, pc, sc)
//    , prgHoldKeas_(ap, pc, sc)
//    , prgHoldMach_(ap, pc, sc)
//{
//}
//
//void VesselControl::handle_post_step(bco::vessel& vessel, double simt, double simdt, double mjd)
//{
//    auto current = RunningPrograms();
//    if (prevRunningProgs != current) UpdateProgs(current);
//
//	auto atmoOn = IsRunning(FCProgFlags::AtmoActive);
//
//    if (atmoOn && IsRunning(FCProgFlags::HoldAltitude))	prgHoldAltitude_.Step(simt, simdt, mjd);
//    if (atmoOn && IsRunning(FCProgFlags::HoldHeading))	prgHoldHeading_.Step(simt, simdt, mjd);
//    if (atmoOn && IsRunning(FCProgFlags::HoldKEAS))		prgHoldKeas_.Step(simt, simdt, mjd);
//    if (atmoOn && IsRunning(FCProgFlags::HoldMACH))		prgHoldMach_.Step(simt, simdt, mjd);
//
//    prevRunningProgs = current;
//}
//
//void VesselControl::ToggleAtmoProgram(FCProgFlags pid)
//{
//	SetProgramState(pid, !IsRunning(pid));
//
//	if ((pid == FCProgFlags::HoldKEAS) && (IsRunning(pid)))	SetProgramState(FCProgFlags::HoldMACH, false);
//	if ((pid == FCProgFlags::HoldMACH) && (IsRunning(pid)))	SetProgramState(FCProgFlags::HoldKEAS, false);
//}
