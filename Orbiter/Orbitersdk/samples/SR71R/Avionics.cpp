//	Avionics - SR-71r Orbiter Addon
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

#include "bc_orbiter\BaseVessel.h"

#include "Avionics.h"
#include "SR71r_mesh.h"

#include <assert.h>

namespace msh = bt_mesh::SR71rVC;

Avionics::Avionics(bco::BaseVessel* vessel, double amps) :
	bco::PoweredComponent(vessel, amps, AV_MIN_VOLT)
{
	dialHeadingSet_.SetDialLeftFunc([this] { DialHeading(-0.1); });
	dialHeadingSet_.SetDialRightFunc([this] { DialHeading(0.1); });
	vessel->RegisterVCEventTarget(&dialHeadingSet_);

	dialCourseSet_.SetDialLeftFunc([this] { DialCourse(-0.1); });
	dialCourseSet_.SetDialRightFunc([this] { DialCourse(0.1); });
	vessel->RegisterVCEventTarget(&dialCourseSet_);

	swSelectRadio_.AddStopFunc(0.0, [this] {navRadio_ = NavRadio::Nav2; });
	swSelectRadio_.AddStopFunc(1.0, [this] {navRadio_ = NavRadio::Nav1; });
	swSelectRadio_.SetStep(0);

	swAvionMode_.AddStopFunc(0.0, [this] { avionMode_ = AvionMode::AvionAtmo; });
	swAvionMode_.AddStopFunc(1.0, [this] { avionMode_ = AvionMode::AvionExo; });
	swAvionMode_.SetStep(0);
}

double Avionics::CurrentDraw()
{
	return (HasPower() && swPower_.IsOn()) ? PoweredComponent::CurrentDraw() : 0.0;
}

void Avionics::Step(double simt, double simdt, double mjd)
{
}

void Avionics::OnSetClassCaps()
{
    auto vessel = GetBaseVessel();

    swPower_.Setup(vessel);
    swSelectRadio_.Setup(vessel);
    swAvionMode_.Setup(vessel);
    
    gaBearingArrow_.Setup(vessel);
    gaRoseCompass_.Setup(vessel);
    gaHeadingBug_.Setup(vessel);
    gaCourse_.Setup(vessel);
	gaCoureError_.Setup(vessel);

    gaAlt1Needle_.Setup(vessel);
    gaAlt10Needle_.Setup(vessel);
    gaAlt100Needle_.Setup(vessel);

    gaVSINeedle_.Setup(vessel);


    gaCrsOnes_.Setup(vessel);
    gaCrsTens_.Setup(vessel);
    gaCrsHund_.Setup(vessel);

    gaMilesOnes_.Setup(vessel);
    gaMilesTens_.Setup(vessel);
    gaMilesHund_.Setup(vessel);

    gaKeasOnes_.Setup(vessel);
    gaKeasTens_.Setup(vessel);
    gaKeasHund_.Setup(vessel);

    gaTDIAltOnes_.Setup(vessel);
    gaTDIAltTens_.Setup(vessel);
    gaTDIAltHund_.Setup(vessel);
    gaTDIAltThous_.Setup(vessel);
    gaTDIAltTenThou_.Setup(vessel);

    gaTDIMachOnes_.Setup(vessel);
    gaTDIMachTens_.Setup(vessel);
    gaTDIMachHund_.Setup(vessel);

    gaAccel_.Setup(vessel);
    gaTrim_.Setup(vessel);

    gaAOA_.Setup(vessel);

    gaMachMax_.Setup(vessel);
    gaSpeed_.Setup(vessel);
    gaKies_.Setup(vessel);

    redrawId_ = GetBaseVessel()->RegisterVCRedrawEvent(this);
}

bool Avionics::OnLoadVC(int id)
{
	// We don't need the redraw event id because Redraw just does everything.
	oapiVCRegisterArea(redrawId_, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	return true;
}

bool Avionics::OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine)
{
	if (_strnicmp(key, ConfigKey, 8) != 0)
	{
		return false;
	}

	int power;
	int heading;
	int course;
    int navSelect;
    int navMode;

	sscanf_s(configLine + 8, "%i%i%i%i%i", &power, &heading, &course, &navSelect, &navMode);

	swPower_.SetState((power == 0) ? 0.0 : 1.0);
    
	
	swSelectRadio_.SetStep((navSelect == 0) ? 0 : 1);

    swAvionMode_.SetStep((navMode == 0) ? 0 : 1);

	setHeading_ = RAD * ((double)min(360, max(0, heading)));
	setCourse_ = RAD * ((double)min(360, max(0, course)));

	return true;
}

void Avionics::OnSaveConfiguration(FILEHANDLE scn) const
{
	char cbuf[256];
	auto val = (swPower_.GetState() == 0.0) ? 0 : 1;
    auto nav = (swSelectRadio_.GetStep() == 0) ? 0 : 1;
    auto mode = (swAvionMode_.GetStep() == 0) ? 0 : 1;

	sprintf_s(cbuf, "%i %i %i %i %i", val, (int)(DEG * setHeading_), (int)(DEG * setCourse_), nav, mode);
	oapiWriteScenario_string(scn, (char*)ConfigKey, cbuf);
}

bool Avionics::OnVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	auto devMesh = GetBaseVessel()->GetVirtualCockpitMesh0();
	assert(nullptr != devMesh);

    UpdateGauges(devMesh);

	DrawVerticalSpeed(devMesh);
	DrawHSI(devMesh);
	return true;
}

// IAvionics:
double Avionics::GetPitch() const			{ return IsActive() ? GetBaseVessel()->GetPitch() : 0.0; }
double Avionics::GetBank() const			{ return IsActive() ? GetBaseVessel()->GetBank() : 0.0; }
double Avionics::GetAltitudeFeet(AltitudeMode mode) const
{
    if (!IsActive()) return 0.0;

    int res = 0;
    auto alt = GetBaseVessel()->GetAltitude(mode, &res);
    return alt * 3.28084; 
}
double Avionics::GetRawAltitude() const		{ return IsActive() ? GetBaseVessel()->GetAltitude() : 0.0; }
double Avionics::GetVertSpeedFPM() const	{ return IsActive() ? bco::GetVerticalSpeedFPM(GetBaseVessel()) : 0.0; }
double Avionics::GetVertSpeedRaw() const	{ return IsActive() ? bco::GetVerticalSpeedRaw(GetBaseVessel()) : 0.0; }
double Avionics::GetAirSpeedKias() const	{ return IsActive() ? bco::GetVesselKias(GetBaseVessel()) : 0.0; }
double Avionics::GetAtmPressure() const		{ return IsActive() ? GetBaseVessel()->GetAtmPressure() : 0.0; }
double Avionics::GetDynamicPressure() const { return IsActive() ? GetBaseVessel()->GetDynPressure() : 0.0; }
double Avionics::GetAngleOfAttack() const	{ return IsActive() ? GetBaseVessel()->GetAOA() : 0.0; }
double Avionics::GetAirSpeedKeas() const	{ return IsActive() ? bco::GetVesselKeas(GetBaseVessel()) : 0.0; }

/*  GetHeading
    The vessel 'yaw', the way the ship is pointed.
*/
double Avionics::GetHeading() const			{ return IsActive() ? GetBaseVessel()->GetYaw() : 0.0; }
double Avionics::GetGForces() const			{ return IsActive() ? bco::GetVesselGs(GetBaseVessel()) : 0.0; }

/*  GetSetHeading
    The current 'set' heading in the HSI.
    */
double Avionics::GetSetHeading() const		{ return setHeading_; }
double Avionics::GetTrimLevel() const		{ return IsActive() ? GetBaseVessel()->GetControlSurfaceLevel(AIRCTRL_ELEVATORTRIM) : 0.0; }

/**
	Returns the current MACH and MAX allowed MACH for conditions for the vessel.
*/
void Avionics::GetMachNumbers(double& mach, double& maxMach)
{
    mach = 0.0;
    maxMach = 0.0;
    isOverspeed_ = false;
    
    if (IsActive())
	{
		auto vessel = GetBaseVessel();

		mach = vessel->GetMachNumber();
        auto atmDens = vessel->GetAtmDensity();

        if (atmDens > 0.0)
        {
            // 30.0 * 1000 * 2 = 60000 --> a guess at the dynamic values of SR71r.
            const double MaxPress = 60000.0;
            maxMach = sqrt(MaxPress / atmDens) / 331.34;
            isOverspeed_ = (mach > maxMach);
        }
	}
}

void Avionics::GetAngularVel(VECTOR3& v)
{
    GetBaseVessel()->GetAngularVel(v);
}

double Avionics::SetHeading(double hdg)
{
	if (hdg < 0.0) return setHeading_;
	if (hdg > PI2) return setHeading_;

	setHeading_ = hdg;
	return setHeading_;
}

bool Avionics::IsOverSpeed() const { return isOverspeed_; }

bco::DialSwitch& Avionics::HeadingSetDial() { return dialHeadingSet_; }

bco::OnOffSwitch& Avionics::PowerSwitch() { return swPower_; }

bool Avionics::IsActive() const
{
	return
		HasPower() &&
		swPower_.IsOn();
}

void Avionics::DialHeading(double i)
{
	auto inc = setHeading_ + (i * RAD);
	if (inc > PI2) inc -= PI2;
	if (inc < 0) inc += PI2;
	setHeading_ = inc;
}

void Avionics::DialCourse(double i)
{
	auto inc = setCourse_ + (i * RAD);
	if (inc > PI2) inc -= PI2;
	if (inc < 0) inc += PI2;
	setCourse_ = inc;
}

void Avionics::UpdateGauges(DEVMESHHANDLE devMesh)
{
    // Collect data
    auto altitudeMode   = (avionMode_== AvionMode::AvionExo) ? ALTMODE_GROUND : ALTMODE_MEANRAD;
    auto altitudeFeet   = GetAltitudeFeet(altitudeMode);
    auto keasSpeed      = GetAirSpeedKeas();
    auto gforce         = GetGForces();
    auto trimLevel      = GetTrimLevel();
    auto dynPressure    = GetDynamicPressure();
    auto angleOfAttack  = GetAngleOfAttack();
    auto pitch          = GetPitch();
    auto bank           = GetBank();

    double mach, maxMach;
    GetMachNumbers(mach, maxMach);


    bco::TensParts parts;

    // ** ALTIMETER **
    bco::BreakTens((altitudeFeet>100000) ? 0 : altitudeFeet, parts);  // Limit to 100000

    gaAlt1Needle_.SetState(bco::AngleToState(PI2 * (parts.Hundreds / 10)));
    gaAlt10Needle_.SetState(bco::AngleToState(PI2 * (parts.Thousands / 10)));
    gaAlt100Needle_.SetState(bco::AngleToState(PI2 * (parts.TenThousands / 10)));

    auto altFlgOffset = 0.0244;
    auto isOffFlagOn = !IsActive() || (altitudeFeet > 100000);
    txAltimeterOff_.SetTranslate(_V(isOffFlagOn ? 0.0 :altFlgOffset, 0.0, 0.0));
    txAltimeterOff_.Draw(devMesh);
    
    auto isAltGndOn = !isOffFlagOn && (avionMode_==AvionMode::AvionExo);
    txAltimeterGnd_.SetTranslate(_V(isAltGndOn ? 0.0 : altFlgOffset, 0.0, 0.0));
    txAltimeterGnd_.Draw(devMesh);

    // ** TDI **
    // KEAS:
    bco::GetDigits(keasSpeed, parts);
    gaKeasOnes_.SetState(parts.Tens / 10);
    gaKeasTens_.SetState(parts.Hundreds / 10);
    gaKeasHund_.SetState(parts.Thousands / 10);

    // ALT
    bco::GetDigits(altitudeFeet / 1000, parts); // Adjust for 1000 feet
    gaTDIAltOnes_.SetState(parts.Tens / 10);
    gaTDIAltTens_.SetState(parts.Hundreds / 10);
    gaTDIAltHund_.SetState(parts.Thousands / 10);
    gaTDIAltThous_.SetState(parts.TenThousands / 10);
    gaTDIAltTenThou_.SetState(parts.HundredThousands / 10);

    // MACH
    bco::GetDigits(mach, parts);
    gaTDIMachOnes_.SetState(parts.Tenths / 10);
    gaTDIMachTens_.SetState(parts.Tens / 10);
    gaTDIMachHund_.SetState(parts.Hundreds / 10);

    // ** ACCEL **
    gaAccel_.SetState((gforce + 2) / 6);

    // ** TRIM **
    gaTrim_.SetState((trimLevel + 1) / 2);

    // ** AOA **
	// AOA gauge works from -5 to 20 degrees AOA (-.0873 to .3491)
	// AOA guage has a throw of 75 degrees (1.3090).
	// Guage ratio is 3 AOA -> guage position.  The gauge sits at -5 deg
	// which must be accounted for.
	auto aoaR = 0.0;

	// Only worry about AOA if in the atmosphere.
	if (dynPressure > 200)
	{
		aoaR = angleOfAttack;
		if (aoaR < -0.0873) aoaR = -0.0873;
		if (aoaR > 0.3491) aoaR = 0.3491;

		aoaR = aoaR * 3; // Translate to guage angle.
	}

    // gauge bottom = 0.0872 (rad), top = 0.5232
    gaAOA_.SetState((aoaR + 0.2619) / 1.136);  

    // ** SPEED **
    const double MIN_PIN = 0.0;
    const double MAX_PIN = 300.0;
    const double MAX_MACH = 22.0;

    auto spGauge = (mach > MAX_MACH) ? MAX_MACH : mach;

    if (avionMode_==AvionMode::AvionExo)  // if exo mode, use velocity for spGauge
    {
        spGauge = GetBaseVessel()->GetAirspeed() / 100;
        maxMach = 22.0;
    }


	/*
		0.5		 40.6
		1.5		 91.8
		2.5		125.5
		3.5		150.6
		4.5		170.7
		

		0		  0.0
		1		 69.4
		2		110.0
		3		139.0
		4		161.2
		5		179.4
		6		194.7	[-165.1]
		7		208.2	[-151.8]
		8		220.0	[-140.0]
		9		230.6	[-129.4]
		10		240.1	[-119.9]
		12		256.7	[-103.1]
		14		271.2	[ -88.8]
		16		283.7	[ -76.3]
		18		294.9	[ -65.1]
		20		304.9	[ -55.0]
	
	*/

    // Set WARNING MAX MACH gauge : MACH scale is log base 22 (max mach):
	// LOG range 1 to 23 (22 positions) to avoid <1 values.
    auto l22 = log(23);
    auto r = MAX_PIN * RAD;					// Max amount in RAD the gauge can move.

	if (maxMach > 22.0) maxMach = 22.0;		// Pin MAX to 22 and 1
	if (maxMach < 0.0) maxMach = 0.0;

    auto rm = (maxMach==0.0) ? 0.0 : log(maxMach + 1) / l22;	// Determine LOG based on speed.
    auto mmRot = rm * r;					// Get rotation RAD
	gaMachMax_.SetState(mmRot / r);			// Set based on ratio


    // Kies
    auto kias = GetAirSpeedKias();
	auto rr = log(spGauge + 1.0) / l22;
	auto spRot = rr * r;

    auto spRatio = spRot / r;
    auto kRatio = kias / 600;

    gaSpeed_.SetState(spRatio);

    auto rKies = (avionMode_==AvionMode::AvionExo) ? spRatio : (kRatio - spRatio);
    gaKies_.SetState(rKies);

    auto isSpeedOff = !IsActive() || (mach >= MAX_MACH);
    txSpeedOff_.SetTranslate(_V(isSpeedOff ? 0.0 : altFlgOffset, 0.0, 0.0));
    txSpeedOff_.Draw(devMesh);

    auto isVelOn = IsActive() && (avionMode_ == AvionMode::AvionExo);
    txSpeedVel_.SetTranslate(_V(isVelOn ? 0.0 : altFlgOffset, 0.0, 0.0));
    txSpeedVel_.Draw(devMesh);

    /** ATTITUDE **/

    auto isAttOff = !IsActive();
    txAttitudeOff_.SetTranslate(_V(isAttOff ? 0.0 : altFlgOffset, 0.0, 0.0));
    txAttitudeOff_.Draw(devMesh);


    VECTOR3 v;
    if (GetBaseVessel()->GetAirspeedVector(FRAME_LOCAL, v))
    {
        auto normForward = _V(0.0, 0.0, 1.0);			// Ship's nose direction.
        auto nlon = _V(v.x, 0.0, v.z);					// For longitude motion, left right, collapse to the x/z plane, lose y.
        normalise(nlon);								// Normalize, magnitude is speed, which we don't care about.

                                                        // Longitude prograde
        auto a = dotp(normForward, nlon);
        auto vert = acos(a);							// Get the angle from ship nose to prograde.  This will always be positive
        vert = vert * ((v.x > 0.0) ? 1.0 : -1.0);		// so this line will adjust based on if the nose is left or right of prograde.

                                                        // Course is prograde if exo mode
        if (avionMode_ == AvionMode::AvionExo)
        {
            auto nb = 0.0;
            if (vert >= 0.0)
            {
                nb = 0.5 * (vert / PI);
            }
            else
            {
                nb = 1.0 + (0.5 * (vert / PI));
            }

            if (isnan(vert)) vert = 0.0;
            gaCourse_.SetState(bco::AngleToState(vert));
			gaCoureError_.SetState(bco::AngleToState(vert));

            courseErrorTex_.SetTranslate(_V(0.0, 0.0, 0.0));
            courseErrorTex_.Draw(devMesh);
        }
    }

    if ((abs(pitch - prevPitch_) > 0.002) || (abs(bank - prevBank_) > 0.002))
    {
        prevPitch_ = pitch;
        prevBank_ = bank;

        // 90deg = 1.57079 rad.
        // So, pixel distance between 0 and 90 is 500 - 178 = 322
        // Texture size of 2048, the ratio for 322 = .15722.
        // Divided by 1.57079 = .100093

        auto pTran = 0.100093 * pitch;

        attitudeIndicator_.SetAngle(bank);
        attitudeIndicator_.SetTranslate(_V(0.0, -pTran, 0.0));
        attitudeIndicator_.Draw(devMesh);
    }
}

void Avionics::DrawVerticalSpeed(DEVMESHHANDLE devMesh)
{
    // VSI displays speeds between -6000 and 6000 fpm.  On the
    // VSI gauge animation this is 0 (-6000) to 1 (6000).
    // Furthermore we scale the speed around 0 so that we get
    // a greater resolution around 0.  This means 0 fpm is
    // 0.5 on the animation.

	auto vertSpeed = GetVertSpeedFPM();

	double isPos = (vertSpeed >= 0) ? 1 : -1;
	auto absSpd = abs(vertSpeed);
    if (absSpd > 6000) absSpd = 6000;


    // This scales our speed so we end up with 0 to 1.
    // 6000 -> 1.0      0 ->    0.0
    // 5000 -> 0.69     1000 -> 0.17
    // 4000 -> 0.44     2000 -> 0.11
    // 3000 -> 0.25     3000 -> 0.25
    // 2000 -> 0.11     4000 -> 0.44
    // 1000 -> 0.03     5000 -> 0.69
    double spRot = (1 - pow((6000 - absSpd) / 6000, 2)) / 2;

    gaVSINeedle_.SetState(0.5 + (isPos * spRot));

    auto altFlgOffset = 0.0244;
    auto isOffFlagOn = !IsActive();
    txVSIOff_.SetTranslate(_V(isOffFlagOn ? 0.0 : altFlgOffset, 0.0, 0.0));
    txVSIOff_.Draw(devMesh);
}

void Avionics::DrawHSI(DEVMESHHANDLE devMesh)
{
    const double comStatusOffset = 0.0244;
    auto isComStatus = false;

    auto yaw    = IsActive() ? GetHeading() : 0.0;

    auto rotHdg = -yaw + GetSetHeading();
	auto rotCrs = -yaw + setCourse_;

    gaRoseCompass_.SetState(bco::AngleToState(-yaw));
    gaHeadingBug_.SetState(bco::AngleToState(rotHdg));

    auto deg = setCourse_ * 57.2958;
    bco::TensParts parts;
    bco::GetDigits(deg, parts);

    gaCrsOnes_.SetState(parts.Tens / 10);
    gaCrsTens_.SetState(parts.Hundreds / 10);
    gaCrsHund_.SetState(parts.Thousands / 10);


	double errTrans = 0.0;
	double gsTrans = 0.0;

	// Navigation:
    auto radio      = (navRadio_ == NavRadio::Nav1) ? 0 : 1;     // Radio number
	auto vessel     = GetBaseVessel();
    auto navHandle  = IsActive() ? vessel->GetNavSource(radio) : nullptr;

    double milesBeacon = 0.0;

	if (NULL != navHandle)
	{
		auto navType = oapiGetNavType(navHandle);
		if ((navType == TRANSMITTER_ILS) || (navType == TRANSMITTER_VOR) ||(navType == TRANSMITTER_VTOL))
		{
            // We have a radio of a type we are interested in, so enable COM status light.
            isComStatus = true;

			NAVDATA data;
			oapiGetNavData(navHandle, &data);

            // Get the location of the NAV beacon.
            // We get it in global coords, then convert
            // to local lon/lat/rad coords.
			VECTOR3 navPosition;
			auto navRef = vessel->GetSurfaceRef();
			oapiGetNavPos(navHandle, &navPosition);
			double navlng, navlat, rad;
			oapiGlobalToEqu(navRef, navPosition, &navlng, &navlat, &rad);

            // Now get the location of our vessel.
			double vlng, vlat, vrad, slope;
            double navDistance;
            double navBearing;
			OBJHANDLE hRef = vessel->GetEquPos(vlng, vlat, vrad);

            // Now calc the distance and bearing between the two.
			bco::Orthodome(vlng, vlat, navlng, navlat, navDistance, navBearing);
			navDistance *= oapiGetSize(hRef);


            if (avionMode_ == AvionMode::AvionExo)
            {
                VECTOR3 nLoc;
                vessel->Global2Local(navPosition, nLoc);
                //sprintf(oapiDebugString(), "DLat: %+4.4f DLon: %+4.4f ", (navlat - vlat), (navlng - vlng));
                //sprintf(oapiDebugString(), "X: %+4.4f Y: %+4.4f Z: %+4.4f", nLoc.x, nLoc.y, nLoc.z);
            }


			double courseDeviation = navBearing - setCourse_;
			if (courseDeviation < -PI) courseDeviation += PI2;
			else if (courseDeviation >= PI) courseDeviation -= PI2;
			if (courseDeviation < -PI05) courseDeviation = -PI - courseDeviation;
			else if (courseDeviation >= PI05) courseDeviation = PI - courseDeviation;

			const double Range = 0.14;
			const double Slide = 0.025;

			auto devB = min(Range, max(-Range, courseDeviation));
			errTrans = devB / Range * Slide;

			// Glide slope
			slope = atan2(vessel->GetAltitude(), navDistance);

			const double tgtslope = 3.0 * RAD;
			const double tgtvar = 0.6 * RAD;

			double dslope = slope - tgtslope;
			if (dslope > tgtvar) dslope = tgtvar;
			if (dslope < -tgtvar) dslope = -tgtvar;

			gsTrans = 0.01218 * (dslope / tgtvar);

//// TEST
            milesBeacon = navDistance / 1609.34; // convert to miles.
            if (milesBeacon > 999) milesBeacon = 999;

            //targetBearing_ = (-yaw + navBearing) / PI2;
            //if (targetBearing_ < 0.0) targetBearing_ = targetBearing_ + 1.0;
//            gaBearingArrow_.SetAngle(-yaw + navBearing);
            gaBearingArrow_.SetState(bco::AngleToState(-yaw + navBearing));
//            gaBearingArrow_.SetState(targetBearing_);
//            sprintf(oapiDebugString(), "Y: %+4.2f NB: %+4.2f TB: %+4.2f", yaw, navBearing, targetBearing_);
////
		}
	}

    auto comOff = (IsActive() && isComStatus) ? comStatusOffset : 0.0;
    comStatus_.SetTranslate(_V(comOff, 0.0, 0.0));
    comStatus_.Draw(devMesh);

    // Miles barrels
    bco::GetDigits(milesBeacon, parts);
    
    gaMilesOnes_.SetState(parts.Tens / 10);
    gaMilesTens_.SetState(parts.Hundreds / 10);
    gaMilesHund_.SetState(parts.Thousands / 10);

    if ((avionMode_ == AvionMode::AvionAtmo) && IsActive())
    {
        gaCourse_.SetState(bco::AngleToState(rotCrs));
		gaCoureError_.SetState(bco::AngleToState(rotCrs));

        courseErrorTex_.SetTranslate(_V(-errTrans, 0.0, 0.0));
        courseErrorTex_.Draw(devMesh);
    }

	gsNeedle_.SetTranslate(_V(0.0, gsTrans, 0.0));
	gsNeedle_.Draw(devMesh);

    auto altFlgOffset = 0.0244;
    auto isOffFlagOn = !IsActive();
    txHSIOff_.SetTranslate(_V(isOffFlagOn ? 0.0 : altFlgOffset, 0.0, 0.0));
    txHSIOff_.Draw(devMesh);

    auto isExo = IsActive() && (avionMode_ == AvionMode::AvionExo);
    txHSIExo_.SetTranslate(_V(isExo ? 0.0 : altFlgOffset, 0.0, 0.0));
    txHSIExo_.Draw(devMesh);
}
