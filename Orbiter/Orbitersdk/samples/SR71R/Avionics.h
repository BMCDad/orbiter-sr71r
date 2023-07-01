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
    virtual void OnSetClassCaps() override;
    virtual bool OnLoadVC(int id) override;
    virtual bool OnVCRedrawEvent(int id, int event, SURFHANDLE surf) override;
    virtual bool OnLoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
    virtual void OnSaveConfiguration(FILEHANDLE scn) const override;

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

//    bco::DialSwitch& HeadingSetDial();
//    bco::OnOffSwitch& PowerSwitch();

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

    //bco::DialSwitch		    dialHeadingSet_     { bm::vc::HeadingKnob_location,        0.007 };
    //bco::DialSwitch		    dialCourseSet_      { bm::vc::CourseKnob_location,         0.007 };


    /* Attitude Indicator */
//    bco::TextureVisual		attitudeIndicator_  { bm::vc::AttitudeIndicator_verts,     bm::vc::AttitudeIndicator_id };

//    bco::TextureVisual		txAttitudeOff_      { bm::vc::AttitudeFlagOff_verts,       bm::vc::AttitudeFlagOff_id };


    /* COM Status */
//    bco::TextureVisual      comStatus_          { bm::vc::COMStatusPanel_verts,        bm::vc::COMStatusPanel_id };


    /* HSI */
    //bco::VCGaugeWrap        gaBearingArrow_{	{bm::vc::HSIBearingArrow_id},
    //                                            bm::vc::HSIBearingArrow_location,
    //                                            bm::vc::HSIAxis_location,
    //                                            (360 * RAD),
    //                                            1.0
    //};

    //bco::VCGaugeWrap        gaRoseCompass_{ {bm::vc::RoseCompass_id},
    //                                                    bm::vc::RoseCompass_location,
    //                                                    bm::vc::HSIAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //};

    //bco::VCGaugeWrap        gaHeadingBug_{ {bm::vc::HSICompassHeading_id},
    //                                                    bm::vc::HSICompassHeading_location,
    //                                                    bm::vc::HSIAxis_location,
    //                                                    (360 * RAD),
    //                                                    0.3
    //};

    //bco::VCGaugeWrap        gaCourse_{ {   bm::vc::HSICourse_id},
    //                                                    bm::vc::HSICourse_location,
    //                                                    bm::vc::HSIAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //};


    // The same mesh will have both texture and mesh transform.
    //   bco::MeshVisual			courseErrorMesh_        {   bt_mesh::SR71rVC::HSICourseNeedle_verts,		bt_mesh::SR71rVC::HSICourseNeedleId };
	//bco::VCGaugeWrap		gaCoureError_{ {	bm::vc::HSICourseNeedle_id},
	//											bm::vc::HSICourseNeedle_location,
	//											bm::vc::HSIAxis_location,
	//											(360 * RAD),
	//											1.0
	//};

 //   bco::TextureVisual		courseErrorTex_{ bm::vc::HSICourseNeedle_verts,		bm::vc::HSICourseNeedle_id, };


    bco::MeshVisual			gsNeedle_{ bm::vc::GlideSlopeNeedle_verts,	bm::vc::GlideSlopeNeedle_id, };


    /* Altimeter */
    //bco::VCGaugeWrap        gaAlt1Needle_{ {bm::vc::gaAlt1Needle_id},
    //                                                    bm::vc::gaAlt1Needle_location,
    //                                                    bm::vc::AltimeterAxis_location,
    //                                                    (360 * RAD),
    //                                                    2.0
    //};

    //bco::VCGaugeWrap        gaAlt10Needle_{ {bm::vc::gaAlt10Needle_id},
    //                                                    bm::vc::gaAlt10Needle_location,
    //                                                    bm::vc::AltimeterAxis_location,
    //                                                    (360 * RAD),
    //                                                    2.0
    //};


	//			340.0	rotary_display layout based on 340deg circle.
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


    //bco::VCGaugeWrap        gaAlt100Needle_{ {bm::vc::gaAlt100Needle_id},
    //                                                    bm::vc::gaAlt100Needle_location,
    //                                                    bm::vc::AltimeterAxis_location,
    //                                                    (360 * RAD),
    //                                                    2.0
    //};

    //bco::TextureVisual		txAltimeterOff_ { bm::vc::AltimeterOffFlag_verts, bm::vc::AltimeterOffFlag_id };
    //bco::TextureVisual		txAltimeterGnd_{ bm::vc::AltimeterGround_verts, bm::vc::AltimeterGround_id };

    /* VSI */
    //bco::VCGauge            gaVSINeedle_{ {bm::vc::gaVSINeedle_id},
    //                                                    bm::vc::gaVSINeedle_location,
    //                                                    bm::vc::VSIAxis_location,
    //                                                    (340 * RAD),
    //                                                    2.0
    //};

//    bco::TextureVisual		txVSIOff_{ bm::vc::VSIOffFlag_verts, bm::vc::VSIOffFlag_id, };


    /* HSI Course odo */
    //bco::VCGaugeWrap        gaCrsOnes_{ {bm::vc::CrsOnes_id},
    //                                                    bm::vc::CrsOnes_location,
    //                                                    bm::vc::CrsOdoAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //};

    //bco::VCGaugeWrap        gaCrsTens_{ { bm::vc::CrsTens_id },
    //                                                    bm::vc::CrsTens_location,
    //                                                    bm::vc::CrsOdoAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //};

    //bco::VCGaugeWrap        gaCrsHund_{ { bm::vc::CrsHundreds_id },
    //                                                    bm::vc::CrsHundreds_location,
    //                                                    bm::vc::CrsOdoAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //};

    /* HSI Miles */
    //bco::VCGaugeWrap        gaMilesOnes_{ {bm::vc::MilesOnes_id},
    //                                                    bm::vc::MilesOnes_location,
    //                                                    bm::vc::CrsOdoAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //};

    //bco::VCGaugeWrap        gaMilesTens_{ { bm::vc::MilesTens_id },
    //                                                    bm::vc::MilesTens_location,
    //                                                    bm::vc::CrsOdoAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //};

    //bco::VCGaugeWrap        gaMilesHund_            { { bm::vc::MilesHundred_id },
    //                                                    bm::vc::MilesHundred_location,
    //                                                    bm::vc::CrsOdoAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //                                                };

    //bco::TextureVisual		txHSIOff_{ bm::vc::HSIOffFlag_verts, bm::vc::HSIOffFlag_id };
    //bco::TextureVisual		txHSIExo_{ bm::vc::HSIExoFlag_verts, bm::vc::HSIExoFlag_id };

    /* TDI barrels */
    /* KEAS */
    //bco::VCGaugeWrap        gaKeasOnes_             { {bm::vc::TDIKeasOnes_id}, 
    //                                                    bm::vc::TDIKeasOnes_location, 
    //                                                    bm::vc::KEASAxis_location, 
    //                                                    (360 * RAD), 
    //                                                    1.0 
    //                                                };

    //bco::VCGaugeWrap        gaKeasTens_             { { bm::vc::TDIKeasTens_id },
    //                                                    bm::vc::TDIKeasTens_location,
    //                                                    bm::vc::KEASAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //                                                };

    //bco::VCGaugeWrap        gaKeasHund_             { { bm::vc::TDIKeasHund_id },
    //                                                    bm::vc::TDIKeasHund_location,
    //                                                    bm::vc::KEASAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //                                                };

    /* Altitude */
    //bco::VCGaugeWrap        gaTDIAltOnes_           { {bm::vc::TDIAltOnes_id}, 
    //                                                    bm::vc::TDIAltOnes_location, 
    //                                                    bm::vc::TDIAltAxis_location, 
    //                                                    (360*RAD), 
    //                                                    1.0
    //                                                };

    //bco::VCGaugeWrap        gaTDIAltTens_           { { bm::vc::TDIAltTens_id },
    //                                                    bm::vc::TDIAltTens_location,
    //                                                    bm::vc::TDIAltAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //                                                };

    //bco::VCGaugeWrap        gaTDIAltHund_           { { bm::vc::TDIAltHund_id },
    //                                                    bm::vc::TDIAltHund_location,
    //                                                    bm::vc::TDIAltAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //                                                };

    //bco::VCGaugeWrap        gaTDIAltThous_          { { bm::vc::TDIAltThous_id },
    //                                                    bm::vc::TDIAltThous_location,
    //                                                    bm::vc::TDIAltAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //                                                };

    //bco::VCGaugeWrap        gaTDIAltTenThou_        { { bm::vc::TDIAltTenThous_id },
    //                                                    bm::vc::TDIAltTenThous_location,
    //                                                    bm::vc::TDIAltAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //                                                };

    /* Mach */
    //bco::VCGaugeWrap        gaTDIMachOnes_          { {bm::vc::TDIMachOne_id}, 
    //                                                    bm::vc::TDIMachOne_location, 
    //                                                    bm::vc::TDIMachAxis_location, 
    //                                                    (360 * RAD), 
    //                                                    1.0
    //                                                };

    //bco::VCGaugeWrap        gaTDIMachTens_          { { bm::vc::TDIMachTens_id },
    //                                                    bm::vc::TDIMachTens_location,
    //                                                    bm::vc::TDIMachAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //                                                };

    //bco::VCGaugeWrap        gaTDIMachHund_          { { bm::vc::TDIMachHund_id },
    //                                                    bm::vc::TDIMachHund_location,
    //                                                    bm::vc::TDIMachAxis_location,
    //                                                    (360 * RAD),
    //                                                    1.0
    //                                                };


    /* Accelerometer */
    bco::VCGauge            gaAccel_                { {bm::vc::AccelNeedle_id}, 
                                                        bm::vc::AccelNeedle_location, 
                                                        bm::vc::AccelAxis_location,
                                                        (295*RAD), 
                                                        2.0
                                                    };

    /* Trim */
    bco::VCGauge            gaTrim_                 { {bm::vc::TrimNeedle_id}, 
                                                        bm::vc::TrimNeedle_location, 
                                                        bm::vc::TrimAxis_location, 
                                                        (180*RAD), 
                                                        2.0 
                                                    };

    /* AOA */
    bco::VCGauge            gaAOA_                  { {bm::vc::AOANeedle_id},
                                                        bm::vc::AOANeedle_location,
                                                        bm::vc::AOAAxis_location,
                                                        (78*RAD),
                                                        2.0
                                                    };

    /* Speed */
    //bco::VCGauge            gaMachMax_              { {bm::vc::SpeedNeedleMax_id},
    //                                                    bm::vc::SpeedNeedleMax_location,
    //                                                    bm::vc::SpeedAxis_location,
    //                                                    (300*RAD),
    //                                                    2.0
    //                                                };

    //bco::VCGauge            gaKies_                 { {bm::vc::SpeedIndicatorKies_id},
    //                                                    bm::vc::SpeedIndicatorKies_location,
    //                                                    bm::vc::SpeedAxisBack_location,
    //                                                    (300*RAD), 
    //                                                    2.0
    //                                                };

    //bco::VCGauge        gaSpeed_                    { {bm::vc::SpeedNeedle_id},
    //                                                    bm::vc::SpeedNeedle_location,
    //                                                    bm::vc::SpeedAxis_location,
    //                                                    (300*RAD),
    //                                                    2.0
    //                                                };

    //bco::TextureVisual		txSpeedOff_ {   bm::vc::SpeedFlagOff_verts, bm::vc::SpeedFlagOff_id };

    //bco::TextureVisual      txSpeedVel_ {   bm::vc::SpeedVelocityFlag_verts, bm::vc::SpeedVelocityFlag_id};

    //bco::VCToggleSwitch     swPower_                {   bm::vc::SwAvionics_id,
    //                                                    bm::vc::SwAvionics_location,
    //                                                    bm::vc::PowerTopRightAxis_location
    //                                                };

    //bco::VCRotorSwitch		swSelectRadio_          {   bm::vc::SwNavSelect_id, 
    //                                                    bm::vc::SwNavSelect_location, 
    //                                                    bm::vc::SwNavSelectAxis_location,
				//										(117*RAD)
    //                                                };

    //bco::VCRotorSwitch     swAvionMode_            {   bm::vc::SwExoSelect_id,
    //                                                    bm::vc::SwExoSelect_location, 
    //                                                    bm::vc::SwExoSelectAxis_location,
				//										(117*RAD)
    //                                                };

	double				    setHeading_;
	double				    setCourse_;
};