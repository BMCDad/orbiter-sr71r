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

#pragma once

#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\Tools.h"
#include "bc_orbiter\OnOffSwitch.h"
#include "bc_orbiter\RotarySwitch.h"
#include "bc_orbiter\DialSwitch.h"
#include "bc_orbiter\Animation.h"
#include "bc_orbiter\TextureVisual.h"
#include "bc_orbiter\MeshVisual.h"
#include "bc_orbiter\VCToggleSwitch.h"
#include "bc_orbiter\VCRotorSwitch.h"
#include "bc_orbiter\VCGauge.h"

#include "IAvionics.h"
#include "SR71r_mesh.h"

const double AV_MIN_VOLT = 20.0;

class VESSEL3;

namespace bco = bc_orbiter;

/**	Avionics
	Models the atmospheric and exoatmospheric avionics package.  It provides the following 
    inputs for the plane's auto pilots and control panels:
	- Pitch
	- Bank
	- Altitude
	- Vertical speed
	- Airspeed (KIAS, KEAS, MACH)
	- Atmospheric pressure
	- Angle of attack
	- Heading
	- G Forces
	- Set heading (HDG) bug knob.
    - Set course (CRS) bug knob.

	The avioncs package requires power from the main power circuit and draws a consistent
	amp level when the main circuit is powered and the main avionics switch is on (up).

	To enable:
	- Power main circuit (external or fuel cell).
	- Turn 'Main' power switch on (up).
	- Turn 'Avionics' switch (AVION) on (up).

	+ Turn avionics package off during orbital flight periods to save fuel cell life.



	Configuration:
	AVIONICS a b c d e
	a - 0/1 avionics switch off/on.
	b - Heading setting from heading bug (0-360).
	c - Course setting from HIS (0-360).
    d - 0/1 Nav radio select. 1=COM1, 0=COM2 (default COM1)
    e - 0/1 Instrument mode select.  1=EXO, 0=ATMO.
*/
class Avionics :
    public bco::PoweredComponent,
    public IAvionics
{
public:
	enum AvionMode { AvionAtmo, AvionExo };
	enum NavRadio { Nav2, Nav1 };

    Avionics(bco::BaseVessel* vessel, double amps);

    // *** PoweredComponent
    virtual double	CurrentDraw() override;

    // *** Component ***
    virtual void SetClassCaps() override;
    virtual bool LoadVC(int id) override;
    virtual bool VCRedrawEvent(int id, int event, SURFHANDLE surf) override;
    virtual bool LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
    virtual void SaveConfiguration(FILEHANDLE scn) const override;

    void Step(double simt, double simdt, double mjd);

    double GetPitch() const override;
    double GetBank() const override;
    double GetAltitudeFeet(AltitudeMode mode) const override;
    double GetRawAltitude() const override;
    double GetVertSpeedFPM() const override;
    double GetVertSpeedRaw() const override;
    double GetAirSpeedKias() const override;
    double GetAtmPressure() const override;
    double GetDynamicPressure() const override;
    double GetAngleOfAttack() const override;
    double GetAirSpeedKeas() const override;
    double GetHeading() const override;
    double GetGForces() const override;
    double GetSetHeading() const override;
    double GetTrimLevel() const override;
    void GetMachNumbers(double& mach, double& maxMach) override;
    void GetAngularVel(VECTOR3& v) override;
	double SetHeading(double hdg) override;

    bool IsOverSpeed() const;

    bco::DialSwitch& HeadingSetDial();
    bco::OnOffSwitch& PowerSwitch();

private:
    bool IsActive() const;
	void DialHeading(double i);
	void DialCourse(double i);

    void UpdateGauges(DEVMESHHANDLE devMesh);

	void DrawVerticalSpeed(DEVMESHHANDLE devMesh);
	void DrawHSI(DEVMESHHANDLE devMesh);

    bool				    isOverspeed_;
    int                     redrawId_{ 0 };

    const char*			    ConfigKey = "AVIONICS";

    double				    prevPitch_{ 0.0 };
    double				    prevBank_{ 0.0 };
    double				    prevAltFeet_{ 0.0 };

	AvionMode				avionMode_{ AvionMode::AvionAtmo };
	NavRadio				navRadio_{ NavRadio::Nav1 };

    bco::DialSwitch		    dialHeadingSet_     { bt_mesh::SR71rVC::HeadingKnob_location,        0.007 };
    bco::DialSwitch		    dialCourseSet_      { bt_mesh::SR71rVC::CourseKnob_location,         0.007 };


    /* Attitude Indicator */
    bco::TextureVisual		attitudeIndicator_  { bt_mesh::SR71rVC::AttitudeIndicator_verts,     bt_mesh::SR71rVC::AttitudeIndicator_id };

    bco::TextureVisual		txAttitudeOff_      { bt_mesh::SR71rVC::AttitudeFlagOff_verts,       bt_mesh::SR71rVC::AttitudeFlagOff_id };


    /* COM Status */
    bco::TextureVisual      comStatus_          { bt_mesh::SR71rVC::COMStatusPanel_verts,        bt_mesh::SR71rVC::COMStatusPanel_id };


    /* HSI */
    bco::VCGaugeWrap        gaBearingArrow_{	{bt_mesh::SR71rVC::HSIBearingArrow_id},
                                                bt_mesh::SR71rVC::HSIBearingArrow_location,
                                                bt_mesh::SR71rVC::HSIAxis_location,
                                                (360 * RAD),
                                                1.0
    };

    bco::VCGaugeWrap        gaRoseCompass_{ {bt_mesh::SR71rVC::RoseCompass_id},
                                                        bt_mesh::SR71rVC::RoseCompass_location,
                                                        bt_mesh::SR71rVC::HSIAxis_location,
                                                        (360 * RAD),
                                                        1.0
    };

    bco::VCGaugeWrap        gaHeadingBug_{ {bt_mesh::SR71rVC::HSICompassHeading_id},
                                                        bt_mesh::SR71rVC::HSICompassHeading_location,
                                                        bt_mesh::SR71rVC::HSIAxis_location,
                                                        (360 * RAD),
                                                        0.3
    };

    bco::VCGaugeWrap        gaCourse_{ {   bt_mesh::SR71rVC::HSICourse_id},
                                                        bt_mesh::SR71rVC::HSICourse_location,
                                                        bt_mesh::SR71rVC::HSIAxis_location,
                                                        (360 * RAD),
                                                        1.0
    };


    // The same mesh will have both texture and mesh transform.
    //   bco::MeshVisual			courseErrorMesh_        {   bt_mesh::SR71rVC::HSICourseNeedle_verts,		bt_mesh::SR71rVC::HSICourseNeedleId };
	bco::VCGaugeWrap		gaCoureError_{ {	bt_mesh::SR71rVC::HSICourseNeedle_id},
												bt_mesh::SR71rVC::HSICourseNeedle_location,
												bt_mesh::SR71rVC::HSIAxis_location,
												(360 * RAD),
												1.0
	};

    bco::TextureVisual		courseErrorTex_{ bt_mesh::SR71rVC::HSICourseNeedle_verts,		bt_mesh::SR71rVC::HSICourseNeedle_id, };


    bco::MeshVisual			gsNeedle_{ bt_mesh::SR71rVC::GlideSlopeNeedle_verts,	bt_mesh::SR71rVC::GlideSlopeNeedle_id, };


    /* Altimeter */
    bco::VCGaugeWrap        gaAlt1Needle_{ {bt_mesh::SR71rVC::gaAlt1Needle_id},
                                                        bt_mesh::SR71rVC::gaAlt1Needle_location,
                                                        bt_mesh::SR71rVC::AltimeterAxis_location,
                                                        (360 * RAD),
                                                        2.0
    };

    bco::VCGaugeWrap        gaAlt10Needle_{ {bt_mesh::SR71rVC::gaAlt10Needle_id},
                                                        bt_mesh::SR71rVC::gaAlt10Needle_location,
                                                        bt_mesh::SR71rVC::AltimeterAxis_location,
                                                        (360 * RAD),
                                                        2.0
    };


	//			340.0	Gauge layout based on 340deg circle.
	//	6000	170.0
	//	5500	168.8
	//	5000	165.3
	//	4500	159.4
	//	4000	151.1
	//	3500	140.5
	//	3000	127.5
	//	2500	112.2
	//	2000	94.4
	//	1500	74.4
	//	1000	51.9
	//	500		27.2
	//	0		0.0


    bco::VCGaugeWrap        gaAlt100Needle_{ {bt_mesh::SR71rVC::gaAlt100Needle_id},
                                                        bt_mesh::SR71rVC::gaAlt100Needle_location,
                                                        bt_mesh::SR71rVC::AltimeterAxis_location,
                                                        (360 * RAD),
                                                        2.0
    };

    bco::TextureVisual		txAltimeterOff_ { bt_mesh::SR71rVC::AltimeterOffFlag_verts, bt_mesh::SR71rVC::AltimeterOffFlag_id };
    bco::TextureVisual		txAltimeterGnd_{ bt_mesh::SR71rVC::AltimeterGround_verts, bt_mesh::SR71rVC::AltimeterGround_id };

    /* VSI */
    bco::VCGauge            gaVSINeedle_{ {bt_mesh::SR71rVC::gaVSINeedle_id},
                                                        bt_mesh::SR71rVC::gaVSINeedle_location,
                                                        bt_mesh::SR71rVC::VSIAxis_location,
                                                        (340 * RAD),
                                                        2.0
    };

    bco::TextureVisual		txVSIOff_{ bt_mesh::SR71rVC::VSIOffFlag_verts, bt_mesh::SR71rVC::VSIOffFlag_id, };


    /* HSI Course odo */
    bco::VCGaugeWrap        gaCrsOnes_{ {bt_mesh::SR71rVC::CrsOnes_id},
                                                        bt_mesh::SR71rVC::CrsOnes_location,
                                                        bt_mesh::SR71rVC::CrsOdoAxis_location,
                                                        (360 * RAD),
                                                        1.0
    };

    bco::VCGaugeWrap        gaCrsTens_{ { bt_mesh::SR71rVC::CrsTens_id },
                                                        bt_mesh::SR71rVC::CrsTens_location,
                                                        bt_mesh::SR71rVC::CrsOdoAxis_location,
                                                        (360 * RAD),
                                                        1.0
    };

    bco::VCGaugeWrap        gaCrsHund_{ { bt_mesh::SR71rVC::CrsHundreds_id },
                                                        bt_mesh::SR71rVC::CrsHundreds_location,
                                                        bt_mesh::SR71rVC::CrsOdoAxis_location,
                                                        (360 * RAD),
                                                        1.0
    };

    /* HSI Miles */
    bco::VCGaugeWrap        gaMilesOnes_{ {bt_mesh::SR71rVC::MilesOnes_id},
                                                        bt_mesh::SR71rVC::MilesOnes_location,
                                                        bt_mesh::SR71rVC::CrsOdoAxis_location,
                                                        (360 * RAD),
                                                        1.0
    };

    bco::VCGaugeWrap        gaMilesTens_{ { bt_mesh::SR71rVC::MilesTens_id },
                                                        bt_mesh::SR71rVC::MilesTens_location,
                                                        bt_mesh::SR71rVC::CrsOdoAxis_location,
                                                        (360 * RAD),
                                                        1.0
    };

    bco::VCGaugeWrap        gaMilesHund_            { { bt_mesh::SR71rVC::MilesHundred_id },
                                                        bt_mesh::SR71rVC::MilesHundred_location,
                                                        bt_mesh::SR71rVC::CrsOdoAxis_location,
                                                        (360 * RAD),
                                                        1.0
                                                    };

    bco::TextureVisual		txHSIOff_{ bt_mesh::SR71rVC::HSIOffFlag_verts, bt_mesh::SR71rVC::HSIOffFlag_id };
    bco::TextureVisual		txHSIExo_{ bt_mesh::SR71rVC::HSIExoFlag_verts, bt_mesh::SR71rVC::HSIExoFlag_id };

    /* TDI barrels */
    /* KEAS */
    bco::VCGaugeWrap        gaKeasOnes_             { {bt_mesh::SR71rVC::TDIKeasOnes_id}, 
                                                        bt_mesh::SR71rVC::TDIKeasOnes_location, 
                                                        bt_mesh::SR71rVC::KEASAxis_location, 
                                                        (360 * RAD), 
                                                        1.0 
                                                    };

    bco::VCGaugeWrap        gaKeasTens_             { { bt_mesh::SR71rVC::TDIKeasTens_id },
                                                        bt_mesh::SR71rVC::TDIKeasTens_location,
                                                        bt_mesh::SR71rVC::KEASAxis_location,
                                                        (360 * RAD),
                                                        1.0
                                                    };

    bco::VCGaugeWrap        gaKeasHund_             { { bt_mesh::SR71rVC::TDIKeasHund_id },
                                                        bt_mesh::SR71rVC::TDIKeasHund_location,
                                                        bt_mesh::SR71rVC::KEASAxis_location,
                                                        (360 * RAD),
                                                        1.0
                                                    };

    /* Altitude */
    bco::VCGaugeWrap        gaTDIAltOnes_           { {bt_mesh::SR71rVC::TDIAltOnes_id}, 
                                                        bt_mesh::SR71rVC::TDIAltOnes_location, 
                                                        bt_mesh::SR71rVC::TDIAltAxis_location, 
                                                        (360*RAD), 
                                                        1.0
                                                    };

    bco::VCGaugeWrap        gaTDIAltTens_           { { bt_mesh::SR71rVC::TDIAltTens_id },
                                                        bt_mesh::SR71rVC::TDIAltTens_location,
                                                        bt_mesh::SR71rVC::TDIAltAxis_location,
                                                        (360 * RAD),
                                                        1.0
                                                    };

    bco::VCGaugeWrap        gaTDIAltHund_           { { bt_mesh::SR71rVC::TDIAltHund_id },
                                                        bt_mesh::SR71rVC::TDIAltHund_location,
                                                        bt_mesh::SR71rVC::TDIAltAxis_location,
                                                        (360 * RAD),
                                                        1.0
                                                    };

    bco::VCGaugeWrap        gaTDIAltThous_          { { bt_mesh::SR71rVC::TDIAltThous_id },
                                                        bt_mesh::SR71rVC::TDIAltThous_location,
                                                        bt_mesh::SR71rVC::TDIAltAxis_location,
                                                        (360 * RAD),
                                                        1.0
                                                    };

    bco::VCGaugeWrap        gaTDIAltTenThou_        { { bt_mesh::SR71rVC::TDIAltTenThous_id },
                                                        bt_mesh::SR71rVC::TDIAltTenThous_location,
                                                        bt_mesh::SR71rVC::TDIAltAxis_location,
                                                        (360 * RAD),
                                                        1.0
                                                    };

    /* Mach */
    bco::VCGaugeWrap        gaTDIMachOnes_          { {bt_mesh::SR71rVC::TDIMachOne_id}, 
                                                        bt_mesh::SR71rVC::TDIMachOne_location, 
                                                        bt_mesh::SR71rVC::TDIMachAxis_location, 
                                                        (360 * RAD), 
                                                        1.0
                                                    };

    bco::VCGaugeWrap        gaTDIMachTens_          { { bt_mesh::SR71rVC::TDIMachTens_id },
                                                        bt_mesh::SR71rVC::TDIMachTens_location,
                                                        bt_mesh::SR71rVC::TDIMachAxis_location,
                                                        (360 * RAD),
                                                        1.0
                                                    };

    bco::VCGaugeWrap        gaTDIMachHund_          { { bt_mesh::SR71rVC::TDIMachHund_id },
                                                        bt_mesh::SR71rVC::TDIMachHund_location,
                                                        bt_mesh::SR71rVC::TDIMachAxis_location,
                                                        (360 * RAD),
                                                        1.0
                                                    };


    /* Accelerometer */
    bco::VCGauge            gaAccel_                { {bt_mesh::SR71rVC::AccelNeedle_id}, 
                                                        bt_mesh::SR71rVC::AccelNeedle_location, 
                                                        bt_mesh::SR71rVC::AccelAxis_location,
                                                        (295*RAD), 
                                                        2.0
                                                    };

    /* Trim */
    bco::VCGauge            gaTrim_                 { {bt_mesh::SR71rVC::TrimNeedle_id}, 
                                                        bt_mesh::SR71rVC::TrimNeedle_location, 
                                                        bt_mesh::SR71rVC::TrimAxis_location, 
                                                        (180*RAD), 
                                                        2.0 
                                                    };

    /* AOA */
    bco::VCGauge            gaAOA_                  { {bt_mesh::SR71rVC::AOANeedle_id},
                                                        bt_mesh::SR71rVC::AOANeedle_location,
                                                        bt_mesh::SR71rVC::AOAAxis_location,
                                                        (78*RAD),
                                                        2.0
                                                    };

    /* Speed */
    bco::VCGauge            gaMachMax_              { {bt_mesh::SR71rVC::SpeedNeedleMax_id},
                                                        bt_mesh::SR71rVC::SpeedNeedleMax_location,
                                                        bt_mesh::SR71rVC::SpeedAxis_location,
                                                        (300*RAD),
                                                        2.0
                                                    };

    bco::VCGauge            gaKies_                 { {bt_mesh::SR71rVC::SpeedIndicatorKies_id},
                                                        bt_mesh::SR71rVC::SpeedIndicatorKies_location,
                                                        bt_mesh::SR71rVC::SpeedAxisBack_location,
                                                        (300*RAD), 
                                                        2.0
                                                    };

    bco::VCGauge        gaSpeed_                    { {bt_mesh::SR71rVC::SpeedNeedle_id},
                                                        bt_mesh::SR71rVC::SpeedNeedle_location,
                                                        bt_mesh::SR71rVC::SpeedAxis_location,
                                                        (300*RAD),
                                                        2.0
                                                    };

    bco::TextureVisual		txSpeedOff_ {   bt_mesh::SR71rVC::SpeedFlagOff_verts, bt_mesh::SR71rVC::SpeedFlagOff_id };

    bco::TextureVisual      txSpeedVel_ {   bt_mesh::SR71rVC::SpeedVelocityFlag_verts, bt_mesh::SR71rVC::SpeedVelocityFlag_id};

    bco::VCToggleSwitch     swPower_                {   bt_mesh::SR71rVC::SwAvionics_id,
                                                        bt_mesh::SR71rVC::SwAvionics_location,
                                                        bt_mesh::SR71rVC::PowerTopRightAxis_location
                                                    };

    bco::VCRotorSwitch		swSelectRadio_          {   bt_mesh::SR71rVC::SwNavSelect_id, 
                                                        bt_mesh::SR71rVC::SwNavSelect_location, 
                                                        bt_mesh::SR71rVC::SwNavSelectAxis_location,
														(117*RAD)
                                                    };

    bco::VCRotorSwitch     swAvionMode_            {   bt_mesh::SR71rVC::SwExoSelect_id,
                                                        bt_mesh::SR71rVC::SwExoSelect_location, 
                                                        bt_mesh::SR71rVC::SwExoSelectAxis_location,
														(117*RAD)
                                                    };

	double				    setHeading_;
	double				    setCourse_;
};