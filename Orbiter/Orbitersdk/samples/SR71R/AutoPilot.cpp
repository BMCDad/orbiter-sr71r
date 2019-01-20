//	AutoPilot - SR-71r Orbiter Addon
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

#include "StdAfx.h"

#include "Orbitersdk.h"
#include "AutoPilot.h"
#include "Tools.h"
#include "SR71r_mesh.h"
#include "BaseVessel.h"
#include <assert.h>


AutoPilot::AutoPilot(bco::BaseVessel* vessel, double amps) :
	bco::PoweredComponent(vessel, amps, AP_MIN_VOLT),
	configKey_("AUTOPILOT"),
	avionics_(nullptr),
	propulsionControl_(nullptr),
	surfaceController_(nullptr),
	isAPActive_(false),
	visAPMainOn_(bt_mesh::SR71rVC::SwAPMain_verts, bt_mesh::SR71rVC::SwAPMain_id)
{
	swAPMain_.SetPressedFunc(           [this] { ToggleTurningOn(); });
	vessel->RegisterVCEventTarget(&swAPMain_);

    swAPAltitude_.OnFunction(       [this] { OnChangedAltitudeSwitch(); });
    swAPAltitude_.OffFunction(      [this] { OnChangedAltitudeSwitch(); });

    swAPSpeed_.OnFunction(          [this] { OnChangedSpeedSwitch(); });
    swAPSpeed_.OffFunction(         [this] { OnChangedSpeedSwitch(); });

    swAPKeasMach_.OnFunction(       [this] { OnChangedSpeedSwitch(); });
    swAPKeasMach_.OffFunction(      [this] { OnChangedSpeedSwitch(); });

	swAPHeading_.OnFunction(        [this] { OnChangedHeadingSwitch(); });
    swAPHeading_.OffFunction(       [this] { OnChangedHeadingSwitch(); });

    swAPAttitude_.OnFunction(       [this] {OnChangedAttitudeSwitch(); });
    swAPAttitude_.OffFunction(      [this] {OnChangedAttitudeSwitch(); });

	swModeSelect_.AddStopFunc(0.0,  [this] {adjustMode_ = AdjustMode::Altitude; });
    swModeSelect_.AddStopFunc(0.5,  [this] {adjustMode_ = AdjustMode::Speed; });
    swModeSelect_.AddStopFunc(1.0,  [this] {adjustMode_ = AdjustMode::Attitude; });

    eventPitchUp_.SetLeftMouseDownFunc(     [this] {HandleAdjust(AdjustSize::Small, AdjustDirection::Up); });
    eventPitchUp_.SetRightMouseDownFunc(    [this] {HandleAdjust(AdjustSize::Big,   AdjustDirection::Up); });
    vessel->RegisterVCEventTarget(&eventPitchUp_);

    eventPitchDown_.SetLeftMouseDownFunc(   [this] {HandleAdjust(AdjustSize::Small, AdjustDirection::Down); });
    eventPitchDown_.SetRightMouseDownFunc(  [this] {HandleAdjust(AdjustSize::Big,   AdjustDirection::Down); });
    vessel->RegisterVCEventTarget(&eventPitchDown_);

	// Set default switch position, LoadConfig should set this as well.
    swModeSelect_.SetStep(0);
}

void AutoPilot::Step(double simt, double simdt, double mjd)
{
//	animSwitchModeSelector_.Step(		modeSelectorSwitch_.GetState(), simdt);
}

double AutoPilot::CurrentDraw()
{
	return (HasPower() && isAPActive_) ? PoweredComponent::CurrentDraw() : 0.0;
}

void AutoPilot::ChangePowerLevel(double newLevel)
{
    PoweredComponent::ChangePowerLevel(newLevel);
    HasPower() ? TurningOn() : TurningOff();
}

void AutoPilot::SetClassCaps()
{
    swAPHeading_.Setup(GetBaseVessel());
    swAPAltitude_.Setup(GetBaseVessel());
    swAPSpeed_.Setup(GetBaseVessel());
    swAPKeasMach_.Setup(GetBaseVessel());
    swAPAttitude_.Setup(GetBaseVessel());

    swModeSelect_.Setup(GetBaseVessel());
}

bool AutoPilot::DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
    if (oapiCockpitMode() != COCKPIT_VIRTUAL) return false;

    if (!isAPActive_) return false;

    int xLeft = hps->W -1;
    int yTop = hps->H - 30;

    skp->Rectangle( xLeft - 30,
                    yTop,
                    xLeft,
                    yTop + 25);

    skp->Text(xLeft - 27, yTop + 1, "AP", 2);

    skp->Line(xLeft - 77, yTop, xLeft - 77, yTop + 25);
    skp->Line(xLeft - 127, yTop, xLeft - 127, yTop + 25);

    if (swAPHeading_.IsOn())
    {
        skp->Text(xLeft - 72, yTop + 1, "HDG", 3);
    }

    if (swAPAltitude_.IsOn())
    {
        skp->Text(xLeft - 122, yTop + 1, "ALT", 3);
    }

    if (swAPSpeed_.IsOn())
    {
        if (swAPKeasMach_.IsOn())
        {
            skp->Text(xLeft - 162, yTop + 1, "KS", 2);
        }
        else
        {
            skp->Text(xLeft - 162, yTop + 1, "MA", 2);
        }
    }
    return true;
}

// Configurable:
bool AutoPilot::LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
	if (_strnicmp(key, configKey_.c_str(), configKey_.length()) != 0)
	{
		return false;
	}

	int isOn;
	int isHoldHeading;
	int isHoldAltitude;
	int isHoldSpeed;
	int isHoldMach;
	int adjustMode;

	sscanf_s(configLine + configKey_.length(), "%d%d%d%d%d%d", &isOn, &isHoldHeading, &isHoldAltitude, &isHoldSpeed, &isHoldMach, &adjustMode);

    swAPHeading_.SetState(	        (isHoldHeading == 0) ? 0.0 : 1.0);
    swAPAltitude_.SetState(	        (isHoldAltitude == 0) ? 0.0 : 1.0);
    swAPSpeed_.SetState(		    (isHoldSpeed == 0) ? 0.0 : 1.0);
    swAPKeasMach_.SetState(	        (isHoldMach == 0) ? 0.0 : 1.0);
    swModeSelect_.SetStep(adjustMode);

    (isOn == 1) ? TurningOn() : TurningOff();

	return true;
}

void AutoPilot::SaveConfiguration(FILEHANDLE scn) const
{
	char cbuf[256];

	auto pwr = (isAPActive_) ? 1 : 0;
	auto hd = (swAPHeading_.GetState() == 0.0) ? 0 : 1;
	auto al = (swAPAltitude_.GetState() == 0.0) ? 0 : 1;
	auto sp = (swAPSpeed_.GetState() == 0.0) ? 0 : 1;
	auto km = (swAPKeasMach_.GetState() == 0.0) ? 0 : 1;
	auto am = swModeSelect_.GetStep();

	sprintf_s(cbuf, "%d %d %d %d %d %d", pwr, hd, al, sp, km, am);
	
	oapiWriteScenario_string(scn, (char*)configKey_.c_str(), cbuf);
}

void AutoPilot::ToggleTurningOn()
{
	if (isAPActive_)
		TurningOff();
	else
		TurningOn();
}

void AutoPilot::TurningOn()
{
    // All these must be true or we are not really 'on'.
    if (!HasPower())
    {
        isAPActive_ = false;
        return;
    }

    isAPActive_ = true;

    // Call the 'ON' funcs for switchs that are in the on position.
    if (swAPHeading_.IsOn())  swAPHeading_.SetOn();
    if (swAPAltitude_.IsOn()) swAPAltitude_.SetOn();
    if (swAPSpeed_.IsOn())    swAPSpeed_.SetOn();
    if (swAPAttitude_.IsOn()) swAPAttitude_.SetOn();
}

void AutoPilot::TurningOff()
{
    isAPActive_ = false;
//    ClearAllPrograms();
}

void AutoPilot::OnChangedHeadingSwitch()
{
//    SetProgramState(PidHoldHeading, (isAPActive_ && swAPHeading_.IsOn()));
}

void AutoPilot::OnChangedAltitudeSwitch()
{
    if (isAPActive_ && swAPAltitude_.IsOn())
    {
//        SetProgramState(PidHoldAltitude, true);
//        atmoAltitudeTarget_ = avionics_->GetAltitudeFeet(ALTMODE_MEANRAD);
    }
    else
    {
//        SetProgramState(PidHoldAltitude, false);
    }
}

void AutoPilot::OnChangedSpeedSwitch()
{
    //if (isAPActive_ && swAPSpeed_.IsOn())
    //{
    //    if (swAPKeasMach_.IsOn())
    //    {
    //        SetProgramState(PidHoldMACH, false);
    //        atmoKEASTarget_ = avionics_->GetAirSpeedKeas();
    //        SetProgramState(PidHoldKEAS, true);
    //    }
    //    else
    //    {
    //        double mach, maxMach;
    //        avionics_->GetMachNumbers(mach, maxMach);
    //        atmoMACHTarget_ = mach;

    //        SetProgramState(PidHoldKEAS, false);
    //        SetProgramState(PidHoldMACH, true);
    //    }
    //}
    //else
    //{
    //    SetProgramState(PidHoldKEAS, false);
    //    SetProgramState(PidHoldMACH, false);
    //}
}

void AutoPilot::OnChangedAttitudeSwitch()
{
    if (isAPActive_ && swAPAttitude_.IsOn())
    {
//        isAtmoAttitudeRunning_ = true;
        //atmoAOATarget_ = avionics_->GetAngleOfAttack();
        //atmoBankTarget_ = avionics_->GetBank();
    }
    else
    {
//        isAtmoAttitudeRunning_ = false;
    }
}

bool AutoPilot::LoadVC(int id)
{
	// Redraw
	areaId_ = GetBaseVessel()->RegisterVCRedrawEvent(this);
	oapiVCRegisterArea(areaId_, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	return true;
}

bool AutoPilot::VCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	auto devMesh = GetBaseVessel()->GetVirtualCockpitMesh0();
	assert(devMesh != nullptr);

	const double availOffset = 0.0244;
	double trans = 0.0;

	trans = isAPActive_ ? availOffset : 0.0;
	visAPMainOn_.SetTranslate(_V(trans, 0.0, 0.0));
	visAPMainOn_.Draw(devMesh);

	return true;
}

void AutoPilot::HandleAdjust(AdjustSize size, AdjustDirection dir)
{
	//// If autopilot is off, nothing to do:
	//if (!isAPActive_)
	//{
	//	return;
	//}

	//switch (adjustMode_)
	//{
	//case AdjustMode::Altitude:
	//	if (isAtmoAltitudeRunning_)
	//	{
	//		auto aa = (size == AdjustSize::Big) ? (100.0 * 0.3048) : (10.0 * 0.3048);
	//		holdAltitude_ += (dir == AdjustDirection::Up) ? aa : 0 - aa;
	//	}
	//	break;

	//case AdjustMode::Speed:
	//	if (isAtmoKEASRunning_)
	//	{
	//		// Keas
	//		auto sk = (size == AdjustSize::Big) ? 10 : 1;
	//		holdKeas_ += (dir == AdjustDirection::Up) ? sk : 0 - sk;
	//	}
	//	else if (isAtmoMachRunning_)
	//	{
	//		auto sm = (size == AdjustSize::Big) ? 1.0 : 0.1;
	//		holdMach_ += (dir == AdjustDirection::Up) ? sm : 0 - sm;
	//	}
	//	break;

	//case AdjustMode::Attitude:
	//	if (isAttitudeHoldRunning_)
	//	{
	//		auto at = (size == AdjustSize::Big) ? 10.0 : 1.0;
	//		holdPitch_ += (dir == AdjustDirection::Up) ? at : 0 - at;
	//	}
	//	break;
	//}
}