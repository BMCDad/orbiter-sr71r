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

#include "bc_orbiter/handler_interfaces.h"
#include "bc_orbiter/control.h"
#include "bc_orbiter/simple_event.h"
#include "bc_orbiter/panel_display.h"

#include "VesselControl.h"
#include "SR71r_mesh.h"

#include <map>
#include <functional>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace bco = bc_orbiter;
namespace mvc = bm::vc;

namespace FC
{
	enum GCKey
	{
		D0, D1, D2, D3, D4, D5, D6, D7, D8, D9,
		Decimal,
		PlusMinus,
		Clear,
		Enter,
		Previous, Next,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
		HUD,
		Menu
	};

	enum FCProg
	{
		Main,
		Ascent,
		Orbit,
		ReEntry,
		Atmosphere
	};

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
          public bco::VesselComponent
		, public bco::PowerConsumer
		, public bco::HandlesPostStep
		, public bco::HandlesState
		, public bco::HandlesDrawHud
		, public bco::HandlesVCLoading
		, public bco::HandlesPanelLoading
    {
		using KeyFunc = std::function<void()>;

    public:
        const static int DISPLAY_COLS = 20;
        const static int DISPLAY_ROWS = 11;

        FlightComputer(
			  bco::vessel& vessel
			, bco::PowerProvider& pwr);

		// power_consumer
		double AmpDraw() const override { return IsPowered() ? 4.0 : 0.0; }

		// post_step
		void HandlePostStep(bco::vessel& vessel, double simt, double simdt, double mjd) override;
		
		// draw_hud
		void HandleDrawHUD(bco::vessel& vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp) override;

		// manage_state
		bool HandleLoadState(bco::vessel& vessel, const std::string& line) override;
		std::string HandleSaveState(bco::vessel& vessel) override;

		// load_vc
		virtual bool HandleLoacVC(bco::vessel& vessel, int vcid) override;
		virtual bool HandleRedrawVC(bco::vessel& vessel, int id, int event, SURFHANDLE surf) override;

		// load_panel
		virtual bool HandleLoadPanel(bco::vessel& vessel, int id, PANELHANDLE hPanel) override;
		virtual bool HandleRedrawPanel(bco::vessel& vessel, int id, int event, SURFHANDLE surf) override;

        // Computer interface
		void ClearScreen();
		void DisplayLine(int row, char* mask, ...);
		void DisplayText(int row, int col, const char* text);
        int DisplayCols() { return DISPLAY_COLS; }
        int DisplayRows() { return DISPLAY_ROWS; }
		double GetScratchPad();
		void SetScratchPad(double value);

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

		bco::slot<double>&		HeadingSlot()	{ return headingSlot_; }
		bco::signal<double>&	HeadingSignal()	{ return headingSignal_; }

		void ToggleProgram(FCProgFlags prog) { ToggleAtmoProgram(prog); }
    private:
		void Update();
		void Boot();

		bco::PowerProvider&		power_;
		bco::vessel&				vessel_;

		bool IsPowered() const {
			return power_.VoltsAvailable() > 24.0;
		}

		bco::slot<double>			headingSlot_;
		bco::signal<double>			headingSignal_;

		std::string					configKey_{ "COMPUTER" };
		std::string					apConfigKey_{ "AUTOPILOT" };

		bco::FontInfo				vcFont_;
		int							allId_;
		double						prevTime_{ 0.0 };

		bool						isRunning_{ false };
		bool						isDisplayDirty_;

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

		std::vector<FC::GCKey>			keyBuffer_;
		std::map<FC::GCKey, KeyFunc>	mapKeyFunc_;

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

		void SetProgramState(FCProgFlags pid, bool state)
		{
			runningPrograms_ = (state) ?
				runningPrograms_ | pid :
				runningPrograms_ & ~pid;
		}

		void UpdateProg(bco::vessel& vessel, FCProgFlags current, FCProgFlags pid)
		{
			if ((current & pid) != (prevRunningProgs & pid))
			{
				auto prog = mapPrograms_[pid];
				(current & pid) != FCProgFlags::None ? prog->start(vessel) : prog->stop(vessel);
			}
		}

		void UpdateProgs(bco::vessel& vessel, FCProgFlags current)
		{
			UpdateProg(vessel, current, FCProgFlags::HoldAltitude);
			UpdateProg(vessel, current, FCProgFlags::HoldHeading);
			UpdateProg(vessel, current, FCProgFlags::HoldKEAS);
			UpdateProg(vessel, current, FCProgFlags::HoldMACH);
		}

		constexpr bool IsProgramRunning(FCProgFlags pid) const { return (runningPrograms_ & pid) == pid; }

		void ToggleAtmoProgram(FCProgFlags pid)
		{
			SetProgramState(pid, !IsProgramRunning(pid));

			if ((pid == FCProgFlags::HoldKEAS) && (IsProgramRunning(pid)))	SetProgramState(FCProgFlags::HoldMACH, false);
			if ((pid == FCProgFlags::HoldMACH) && (IsProgramRunning(pid)))	SetProgramState(FCProgFlags::HoldKEAS, false);
			isDisplayDirty_ = true;
		}

		double setHeading_{ 0.0 };

		FCProgFlags					runningPrograms_{ FCProgFlags::None };
		FCProgFlags					prevRunningProgs{ FCProgFlags::None };

		HoldAltitudeProgram			prgHoldAltitude_;
		HoldHeadingProgram			prgHoldHeading_;
		HoldKeasProgram				prgHoldKeas_;
		HoldMachProgram				prgHoldMach_;
		
		std::map<FCProgFlags, control_program*>     mapPrograms_
		{
			{FCProgFlags::HoldAltitude,    &prgHoldAltitude_},
			{FCProgFlags::HoldHeading,     &prgHoldHeading_},
			{FCProgFlags::HoldKEAS,        &prgHoldKeas_ },
			{FCProgFlags::HoldMACH,        &prgHoldMach_ }
		};

		bco::simple_event<> gcKey0_			{ bm::vc::GCKey0_loc,			0.01, bm::pnl::pnlGCKey0_RC };
		bco::simple_event<> gcKey1_			{ bm::vc::GCKey1_loc,			0.01, bm::pnl::pnlGCKey1_RC };
		bco::simple_event<> gcKey2_			{ bm::vc::GCKey2_loc,			0.01, bm::pnl::pnlGCKey2_RC };
		bco::simple_event<> gcKey3_			{ bm::vc::GCKey3_loc,			0.01, bm::pnl::pnlGCKey3_RC };
		bco::simple_event<> gcKey4_			{ bm::vc::GCKey4_loc,			0.01, bm::pnl::pnlGCKey4_RC };
		bco::simple_event<> gcKey5_			{ bm::vc::GCKey5_loc,			0.01, bm::pnl::pnlGCKey5_RC };
		bco::simple_event<> gcKey6_			{ bm::vc::GCKey6_loc,			0.01, bm::pnl::pnlGCKey6_RC };
		bco::simple_event<> gcKey7_			{ bm::vc::GCKey7_loc,			0.01, bm::pnl::pnlGCKey7_RC };
		bco::simple_event<> gcKey8_			{ bm::vc::GCKey8_loc,			0.01, bm::pnl::pnlGCKey8_RC };
		bco::simple_event<> gcKey9_			{ bm::vc::GCKey9_loc,			0.01, bm::pnl::pnlGCKey9_RC };
		
		bco::simple_event<> gcKeyClear_		{ bm::vc::GCKeyClear_loc,		0.01, bm::pnl::pnlGCKeyClear_RC };
		bco::simple_event<> gcKeyDecimal_	{ bm::vc::GCKeyDecimal_loc,	0.01, bm::pnl::pnlGCKeyDecimal_RC };
		bco::simple_event<> gcKeyEnter_		{ bm::vc::GCKeyEnter_loc,		0.01, bm::pnl::pnlGCKeyEnter_RC };
		bco::simple_event<> gcKeyHUD_		{ bm::vc::GCKeyHUD_loc,		0.01, bm::pnl::pnlGCKeyHud_RC };
		bco::simple_event<> gcKeyNext_		{ bm::vc::GCKeyNext_loc,		0.01, bm::pnl::pnlGCKeyNext_RC };
		bco::simple_event<> gcKeyPlusMinus_	{ bm::vc::GCKeyPlusMinus_loc,	0.01, bm::pnl::pnlGCKeyPlusMinus_RC };
		bco::simple_event<> gcKeyPrev_		{ bm::vc::GCKeyPrev_loc,		0.01, bm::pnl::pnlGCKeyPrev_RC };
		bco::simple_event<> gcKeyMenu_		{ bm::vc::GCKeyHome_loc,		0.01, bm::pnl::pnlGCKeyMenu_RC };
		
		bco::simple_event<> gcKeyF1_		{ bm::vc::GCKeyFunc1_loc,		0.01, bm::pnl::pnlGCKeyFunc1_RC };
		bco::simple_event<> gcKeyF2_		{ bm::vc::GCKeyFunc2_loc,		0.01, bm::pnl::pnlGCKeyFunc2_RC };
		bco::simple_event<> gcKeyF3_		{ bm::vc::GCKeyFunc3_loc,		0.01, bm::pnl::pnlGCKeyFunc3_RC };
		bco::simple_event<> gcKeyF4_		{ bm::vc::GCKeyFunc4_loc,		0.01, bm::pnl::pnlGCKeyFunc4_RC };
		bco::simple_event<> gcKeyF5_		{ bm::vc::GCKeyFunc5_loc,		0.01, bm::pnl::pnlGCKeyFunc5_RC };
		bco::simple_event<> gcKeyF6_		{ bm::vc::GCKeyFunc6_loc,		0.01, bm::pnl::pnlGCKeyFunc6_RC };
		bco::simple_event<> gcKeyF7_		{ bm::vc::GCKeyFunc7_loc,		0.01, bm::pnl::pnlGCKeyFunc7_RC };
		bco::simple_event<> gcKeyF8_		{ bm::vc::GCKeyFunc8_loc,		0.01, bm::pnl::pnlGCKeyFunc8_RC };
		bco::simple_event<> gcKeyF9_		{ bm::vc::GCKeyFunc9_loc,		0.01, bm::pnl::pnlGCKeyFunc9_RC };
		bco::simple_event<> gcKeyF10_		{ bm::vc::GCKeyFunc10_loc,		0.01, bm::pnl::pnlGCKeyFunc10_RC };

		bco::simple_event<>	apBtnMain_		{ bm::vc::SwAPMain_loc,		0.01, bm::pnl::pnlAPMain_RC };
		bco::simple_event<>	apBtnHeading_	{ bm::vc::SwAPHeading_loc,		0.01, bm::pnl::pnlAPHeading_RC };
		bco::simple_event<>	apBtnAltitude_	{ bm::vc::SwAPAltitude_loc,	0.01, bm::pnl::pnlAPAltitude_RC };
		bco::simple_event<>	apBtnKEAS_		{ bm::vc::SwAPKEAS_loc,		0.01, bm::pnl::pnlAPKEAS_RC };
		bco::simple_event<>	apBtnMACH_		{ bm::vc::SwAPMACH_loc,		0.01, bm::pnl::pnlAPMACH_RC };

		bco::on_off_display	apDspMain_		{ bm::vc::SwAPMain_id,		bm::vc::SwAPMain_vrt,		bm::pnl::pnlAPMain_id,		bm::pnl::pnlAPMain_vrt,	  0.0352	};
		bco::on_off_display	apDspHeading_	{ bm::vc::SwAPHeading_id,	bm::vc::SwAPHeading_vrt,	bm::pnl::pnlAPHeading_id,	bm::pnl::pnlAPHeading_vrt,  0.0352	};
		bco::on_off_display	apDspAltitude_	{ bm::vc::SwAPAltitude_id,	bm::vc::SwAPAltitude_vrt,	bm::pnl::pnlAPAltitude_id,	bm::pnl::pnlAPAltitude_vrt, 0.0352	};
		bco::on_off_display	apDspKEAS_		{ bm::vc::SwAPKEAS_id,		bm::vc::SwAPKEAS_vrt,		bm::pnl::pnlAPKEAS_id,		bm::pnl::pnlAPKEAS_vrt,	  0.0352	};
		bco::on_off_display	apDspMACH_		{ bm::vc::SwAPMACH_id,		bm::vc::SwAPMACH_vrt,		bm::pnl::pnlAPMACH_id,		bm::pnl::pnlAPMACH_vrt,	  0.0352	};
	
		bco::panel_display	pnlHUDTile_		{ bm::pnl::pnlHUDFCTile_id,		bm::pnl::pnlHUDFCTile_vrt,  0.0914 };
		bco::panel_display	pnlHUDText1_	{ bm::pnl::pnlHUDFCText1_id,	bm::pnl::pnlHUDFCText1_vrt, 0.0305 };
		bco::panel_display	pnlHUDText2_	{ bm::pnl::pnlHUDFCText2_id,	bm::pnl::pnlHUDFCText2_vrt, 0.0305 };
		bco::panel_display	pnlHUDText3_	{ bm::pnl::pnlHUDFCText3_id,	bm::pnl::pnlHUDFCText3_vrt, 0.0305 };
};
}