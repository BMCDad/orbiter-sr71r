//	FlightComputer - SR-71r Orbiter Addon
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

/*
    [ 7 ][ 8 ][ 9 ][+/-][PRV][NXT]
    [ 4 ][ 5 ][ 6 ][ . ][   ][HUD]
    [ 1 ][ 2 ][ 3 ][ 0 ][ENT][CLR]
				1111111111    
	  01234567890123456789
	 +--------------------+
	 |MAIN          PG 1:1|			0
 [F1]|<ASCENT             |[F6]		1
     |                    |			1
 [F2]|<ORBIT OPS          |[F7]		3
     |                    |			4
 [F3]|<RE ENTRY           |[F8]		5
     |                    |			6
 [F4]|<ATMOSPHERE         |[F9]		7
     |                    |			8
 [F5]|                    |[F10]	9
	 |[          ]        |			10
	 +--------------------+
		        1111111111
	  01234567890123456789
	 +--------------------+
	 |ATMO          PG 1:1|			0
 [F1]|<HDG      X         |[F6]		1
	 |                    |			1
 [F2]|<ALT      X         |[F7]		3
	 |                    |			4
 [F3]|<KEAS               |[F8]		5
	 |                    |			6
 [F4]|<MACH     X         |[F9]		7
	 |                    |			8
 [F5]|                RUN>|[F10]	9
	 |[          ]        |			10
	 +--------------------+

	 +--------------------+
    |ASCENT        PG 1:1|
 [ ]|<ALT     [ +232.00k]|[ ]
    |               0.00k|
 [ ]|<INC      [  +32.10]|[ ]
    |               9.32 |
 [ ]|                CLR>|[ ]
    |                    |
 [ ]|<RUN                |[ ]
    |                    |
    |[+ 232.30]       (1)|
    +--------------------+
    +--------------------+
    |ASCENT        PG 2:2|
 [ ]|     INC     ALT(k) |[ ]
    |ACT  120.00  340.20 |
 [ ]|TGT  143.10  345.30 |[ ]
    |DLT   23.20   15.10 |
 [ ]|                    |[ ]
    |                    |
 [ ]|               MAIN>|[ ]
    |                    |
    |                 (1)|
    +--------------------+

*/
#pragma once

#include "Orbitersdk.h"

#include "bc_orbiter\PoweredComponent.h"
#include "bc_orbiter\Tools.h"

#include "SR71r_mesh.h"
#include "IAvionics.h"
#include "IComputer.h"
#include "PropulsionController.h"
#include "SurfaceController.h"
#include "FCProgram.h"
#include "VesselControl.h"

#include <map>
#include <functional>
#include <sstream>
#include <iomanip>
#include <algorithm>

// Interfaces used:

namespace bco = bc_orbiter;
namespace mvc = bt_mesh::SR71rVC;


namespace FC
{


    /**	FlightComputer

    Configuration:
    COMPUTER

	Configuration for AUTOPILOT
	AUTOPILOT a b c d e
	a = 0/1 main power.
	b = 0/1 hold heading.
	c = 0/1 hold altitude.
	d = 0/1 hold speed.
	e = 0/1 0 = MACH, 1 = KEAS
	
    */
    class FlightComputer :
        public bco::PoweredComponent,
		public IVesselControl
    {
		using KeyFunc = std::function<void()>;

    public:
        const static int DISPLAY_COLS = 20;
        const static int DISPLAY_ROWS = 11;

        FlightComputer(bco::BaseVessel* vessel, double amps);

		void Step(double simt, double simdt, double mjd);

		bool DrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);

		// Component overrides:
        void SetClassCaps() override;
		bool MouseEvent(int id, int event) override;
		bool LoadVC(int id) override;
		bool VCRedrawEvent(int id, int event, SURFHANDLE surf) override;
		bool LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) override;
		void SaveConfiguration(FILEHANDLE scn) const override;


		// PoweredComponent overrides:
		void ChangePowerLevel(double newLevel) override;
        double CurrentDraw() override;

		bco::PushButtonSwitch   APMainButton()		const { return swAPMain_; }
		bco::PushButtonSwitch   APHeadingButton()	const { return swAPHeading_; }
		bco::PushButtonSwitch   APAltitudeButton()	const { return swAPHAltitude_; }
		bco::PushButtonSwitch   APKEASButton()		const { return swAPKEAS_; }
		bco::PushButtonSwitch   APMACHButton()		const { return swAPMACH_; }



        // IComputer

		void ClearScreen();

		void DisplayLine(int row, char* mask, ...);

		void DisplayText(int row, int col, const char* text);

        int DisplayCols() { return DISPLAY_COLS; }
        int DisplayRows() { return DISPLAY_ROWS; }

		double GetScratchPad();
		void SetScratchPad(double value);


        // Input:
		void SetAvionics(IAvionics* av)						{ avionics_ = av; }
        void SetPropulsionControl(PropulsionController* p)	{ propulsionControl_ = p; }
        void SetSurfaceControl(SurfaceController* s)		{ surfaceController_ = s; }

		// IVesselControl
		IAvionics*              GetAvionics()               const override { return avionics_; }
		PropulsionController*   GetPropulsionController()   const override { return propulsionControl_; }
		SurfaceController*      GetSurfaceController()      const override { return surfaceController_; }


		// Page Functions
		void PageMain();
		void NullFunc() {};
		void PageOrbitOps() {};
		void PageReEntry() {};
		
		// Atmo-AP
		void PageAtmosphere();
		void UpdateAtmospherePage();
		FCProgFlags prevRunning_{ FCProgFlags::None };

		// Ascent
		void PageAscent();
		void UpdateAscentPage();
		void SetTargetIncDeg(double tgt);

		KeyFunc   funcUpdate_ = []{};
		double ascentTargetAlt_{ 0.0 };
		double ascentTargetInc_{ 0.0 };
		double ascentHeading_{ 0.0 };
		double ascentHeadingAlt_{ 0.0 };
		double launchLatitude_{ 0.0 };
		bool isAscentPageDirty_{ true };

    private:
		void Update();
		void Boot();

		bool LoadAPConfiguration(FILEHANDLE scn, const char* configLine);
		void SaveAPConfiguration(FILEHANDLE scn) const;


		std::string					configKey_{ "COMPUTER" };
		std::string					apConfigKey_{ "AUTOPILOT" };

		bco::FontInfo				vcFont_;
		int							allId_;
		double						prevTime_{ 0.0 };


		bool						isRunning_{ false };
		bool						isDisplayDirty_;
        //std::vector<std::string>	display_;

		char display_[DISPLAY_ROWS][DISPLAY_COLS] = 
		{
			"                   ", 
			"                   ",
			"                   ",
			"                   ",
			"                   ",
			"                   ",
			"                   ",
			"                   ",
			"                   ",
			"                   ",
			"                   "
		};

		std::map<int, FC::GCKey>	mapKey_;            // Map the mouse event id to the key.
		std::vector<FC::GCKey>      keyBuffer_;

        // Map the key to the location.  This is used during LoadVC
        // to register mouse event id with a key.
        std::map<FC::GCKey, VECTOR3>  mapKeyLocation_
        {
            { FC::GCKey::D0,        mvc::GCKey0_location},
            { FC::GCKey::D1,        mvc::GCKey1_location },
            { FC::GCKey::D2,        mvc::GCKey2_location },
            { FC::GCKey::D3,        mvc::GCKey3_location },
            { FC::GCKey::D4,        mvc::GCKey4_location },
            { FC::GCKey::D5,        mvc::GCKey5_location },
            { FC::GCKey::D6,        mvc::GCKey6_location },
            { FC::GCKey::D7,        mvc::GCKey7_location },
            { FC::GCKey::D8,        mvc::GCKey8_location },
            { FC::GCKey::D9,        mvc::GCKey9_location },
            { FC::GCKey::Clear,     mvc::GCKeyClear_location },
            { FC::GCKey::Decimal,   mvc::GCKeyDecimal_location },
            { FC::GCKey::Enter,     mvc::GCKeyEnter_location },
            { FC::GCKey::Next,      mvc::GCKeyNext_location },
            { FC::GCKey::Previous,  mvc::GCKeyPrev_location },
            { FC::GCKey::PlusMinus, mvc::GCKeyPlusMinus_location },
            { FC::GCKey::F1,        mvc::GCKeyFunc1_location },
            { FC::GCKey::F2,        mvc::GCKeyFunc2_location },
            { FC::GCKey::F3,        mvc::GCKeyFunc3_location },
            { FC::GCKey::F4,        mvc::GCKeyFunc4_location },
            { FC::GCKey::F5,        mvc::GCKeyFunc5_location },
            { FC::GCKey::F6,        mvc::GCKeyFunc6_location },
            { FC::GCKey::F7,        mvc::GCKeyFunc7_location },
            { FC::GCKey::F8,        mvc::GCKeyFunc8_location },
            { FC::GCKey::F9,        mvc::GCKeyFunc9_location },
            { FC::GCKey::F10,       mvc::GCKeyFunc10_location },
			{ FC::GCKey::Home,		mvc::GCKeyHome_location }
        };

		std::map<FC::GCKey, KeyFunc> mapKeyFunc_;

		void MapKey(FC::GCKey key, KeyFunc fn) { mapKeyFunc_[key] = fn; }
		void MapKey(FC::GCKey key) { mapKeyFunc_[key] = []{}; }
		void ClearFuncKeys();


		// Scratchpad impl
		bool HandleScratchPadKey(FC::GCKey key);
		void ClearScratchPad();
		void SetScratchError(const char* msg);
		bool						isScratchError_{ false };
		bool						scratchIsPos_{ true };
		std::string					scratchKeys_;
		double						scratchValue_ = 0.0;

		/// VesselControl moved stuff VVVV
		//VesselControl vesselCtrl_;		// << to be removed

		void SetProgramState(FCProgFlags pid, bool state)
		{
			runningPrograms_ = (state) ?
				runningPrograms_ | pid :
				runningPrograms_ & ~pid;
		}

		void UpdateProg(FCProgFlags current, FCProgFlags pid)
		{
			if ((current & pid) != (prevRunningProgs & pid))
			{
				auto prog = mapPrograms_[pid];
				(current & pid) != FCProgFlags::None ? prog->Start() : prog->Stop();
			}
		}

		void UpdateProgs(FCProgFlags current)
		{
			UpdateProg(current, FCProgFlags::HoldAltitude);
			UpdateProg(current, FCProgFlags::HoldHeading);
			UpdateProg(current, FCProgFlags::HoldKEAS);
			UpdateProg(current, FCProgFlags::HoldMACH);
		}

		constexpr bool IsProgramRunning(FCProgFlags pid) const { return (runningPrograms_ & pid) == pid; }

		void ToggleAtmoProgram(FCProgFlags pid)
		{
			SetProgramState(pid, !IsProgramRunning(pid));

			if ((pid == FCProgFlags::HoldKEAS) && (IsProgramRunning(pid)))	SetProgramState(FCProgFlags::HoldMACH, false);
			if ((pid == FCProgFlags::HoldMACH) && (IsProgramRunning(pid)))	SetProgramState(FCProgFlags::HoldKEAS, false);
		}


		FCProgFlags					runningPrograms_{ FCProgFlags::None };
		FCProgFlags					prevRunningProgs{ FCProgFlags::None };

		HoldAltitudeProgram			prgHoldAltitude_;
		HoldHeadingProgram			prgHoldHeading_;
		HoldKeasProgram				prgHoldKeas_;
		HoldMachProgram				prgHoldMach_;
		
		PropulsionController*		propulsionControl_;
		IAvionics*					avionics_;
		SurfaceController*			surfaceController_;

		std::map<FCProgFlags, ControlProgram*>     mapPrograms_
		{
			{FCProgFlags::HoldAltitude,    &prgHoldAltitude_},
			{FCProgFlags::HoldHeading,     &prgHoldHeading_},
			{FCProgFlags::HoldKEAS,        &prgHoldKeas_ },
			{FCProgFlags::HoldMACH,        &prgHoldMach_ }
		};








		// Auto pilot panel
		bco::TextureVisual		visAPMainOn_;
		bco::PushButtonSwitch   swAPMain_{ bt_mesh::SR71rVC::SwAPMain_location, 0.01, GetBaseVessel() };
		
		bco::TextureVisual		visAPHeadingOn_;
		bco::PushButtonSwitch   swAPHeading_{ bt_mesh::SR71rVC::SwAPHeading_location, 0.01, GetBaseVessel() };

		bco::TextureVisual		visAPAltitudeOn_;
		bco::PushButtonSwitch   swAPHAltitude_{ bt_mesh::SR71rVC::SwAPAltitude_location, 0.01, GetBaseVessel() };

		bco::TextureVisual		visAPKEASOn_;
		bco::PushButtonSwitch   swAPKEAS_{ bt_mesh::SR71rVC::SwAPKEAS_location, 0.01, GetBaseVessel() };

		bco::TextureVisual		visAPMACHOn_;
		bco::PushButtonSwitch   swAPMACH_{ bt_mesh::SR71rVC::SwAPMACH_location, 0.01, GetBaseVessel() };


    };
}