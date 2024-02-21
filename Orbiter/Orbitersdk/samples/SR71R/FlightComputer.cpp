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

#include "stdafx.h"
#include "FlightComputer.h"
#include "SR71r_mesh.h"

#include <assert.h>

namespace mvc = bm::vc;

FC::FlightComputer::FlightComputer(
	  bco::Vessel& Vessel
	, bco::PowerProvider& pwr)
	:
	  power_(pwr)
	, vessel_(Vessel)
	, headingSlot_([&](double v) { prgHoldHeading_.set_target(v); })
{
	Vessel.AddControl(&gcKey0_);		
	Vessel.AddControl(&gcKey1_);		
	Vessel.AddControl(&gcKey2_);		
	Vessel.AddControl(&gcKey3_);		
	Vessel.AddControl(&gcKey4_);		
	Vessel.AddControl(&gcKey5_);		
	Vessel.AddControl(&gcKey6_);		
	Vessel.AddControl(&gcKey7_);		
	Vessel.AddControl(&gcKey8_);		
	Vessel.AddControl(&gcKey9_);		
	Vessel.AddControl(&gcKeyClear_);	
	Vessel.AddControl(&gcKeyDecimal_);
	Vessel.AddControl(&gcKeyEnter_);
	Vessel.AddControl(&gcKeyHUD_);
	Vessel.AddControl(&gcKeyNext_);
	Vessel.AddControl(&gcKeyPlusMinus_);
	Vessel.AddControl(&gcKeyPrev_);
	Vessel.AddControl(&gcKeyF1_);
	Vessel.AddControl(&gcKeyF2_);
	Vessel.AddControl(&gcKeyF3_);
	Vessel.AddControl(&gcKeyF4_);
	Vessel.AddControl(&gcKeyF5_);
	Vessel.AddControl(&gcKeyF6_);
	Vessel.AddControl(&gcKeyF7_);
	Vessel.AddControl(&gcKeyF8_);
	Vessel.AddControl(&gcKeyF9_);
	Vessel.AddControl(&gcKeyF10_);

	Vessel.AddControl(&apBtnMain_);
	Vessel.AddControl(&apBtnHeading_);
	Vessel.AddControl(&apBtnAltitude_);
	Vessel.AddControl(&apBtnKEAS_);
	Vessel.AddControl(&apBtnMACH_);
	
	Vessel.AddControl(&apDspMain_);
	Vessel.AddControl(&apDspHeading_);
	Vessel.AddControl(&apDspAltitude_);
	Vessel.AddControl(&apDspKEAS_);
	Vessel.AddControl(&apDspMACH_);

	Vessel.AddControl(&pnlHUDTile_);
	Vessel.AddControl(&pnlHUDText1_);
	Vessel.AddControl(&pnlHUDText2_);
	Vessel.AddControl(&pnlHUDText3_);

	gcKey0_			.Attach([&]() { keyBuffer_.push_back(FC::GCKey::D0); });
	gcKey1_			.Attach([&]() { keyBuffer_.push_back(FC::GCKey::D1); });
	gcKey2_			.Attach([&]() { keyBuffer_.push_back(FC::GCKey::D2); });
	gcKey3_			.Attach([&]() { keyBuffer_.push_back(FC::GCKey::D3); });
	gcKey4_			.Attach([&]() { keyBuffer_.push_back(FC::GCKey::D4); });
	gcKey5_			.Attach([&]() { keyBuffer_.push_back(FC::GCKey::D5); });
	gcKey6_			.Attach([&]() { keyBuffer_.push_back(FC::GCKey::D6); });
	gcKey7_			.Attach([&]() { keyBuffer_.push_back(FC::GCKey::D7); });
	gcKey8_			.Attach([&]() { keyBuffer_.push_back(FC::GCKey::D8); });
	gcKey9_			.Attach([&]() { keyBuffer_.push_back(FC::GCKey::D9); });
	gcKeyClear_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::Clear); });
	gcKeyDecimal_	.Attach([&]() { keyBuffer_.push_back(FC::GCKey::Decimal); });
	gcKeyEnter_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::Enter); });
	gcKeyHUD_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::HUD); });
	gcKeyNext_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::Next); });
	gcKeyPlusMinus_	.Attach([&]() { keyBuffer_.push_back(FC::GCKey::PlusMinus); });
	gcKeyPrev_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::Previous); });
	gcKeyF1_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::F1); });
	gcKeyF2_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::F2); });
	gcKeyF3_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::F3); });
	gcKeyF4_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::F4); });
	gcKeyF5_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::F5); });
	gcKeyF6_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::F6); });
	gcKeyF7_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::F7); });
	gcKeyF8_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::F8); });
	gcKeyF9_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::F9); });
	gcKeyF10_		.Attach([&]() { keyBuffer_.push_back(FC::GCKey::F10); });

	apBtnMain_		.Attach([&]() { ToggleAtmoProgram(FCProgFlags::AtmoActive); });
	apBtnHeading_	.Attach([&]() { ToggleAtmoProgram(FCProgFlags::HoldHeading); });
	apBtnAltitude_	.Attach([&]() { ToggleAtmoProgram(FCProgFlags::HoldAltitude); });
	apBtnKEAS_		.Attach([&]() { ToggleAtmoProgram(FCProgFlags::HoldKEAS); });
	apBtnMACH_		.Attach([&]() { ToggleAtmoProgram(FCProgFlags::HoldMACH); });
	
	allId_ = Vessel.GetControlId();
	Vessel.RegisterVCComponent(allId_, this);
	Vessel.RegisterPanelComponent(allId_, this);
}

void FC::FlightComputer::HandleDrawHUD(bco::Vessel& Vessel, int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
	if (oapiCockpitMode() != COCKPIT_VIRTUAL) return;

	if (IsProgramRunning(FCProgFlags::AtmoActive))
	{
		int xLeft = hps->W - 1;
		int yTop = hps->H - 30;

		skp->Rectangle(xLeft - 30,
			yTop,
			xLeft,
			yTop + 25);

		skp->Text(xLeft - 27, yTop + 1, "AP", 2);

		skp->Line(xLeft - 77, yTop, xLeft - 77, yTop + 25);
		skp->Line(xLeft - 127, yTop, xLeft - 127, yTop + 25);

		if (IsProgramRunning(FCProgFlags::HoldHeading))
		{
			skp->Text(xLeft - 72, yTop + 1, "HDG", 3);
		}

		if (IsProgramRunning(FCProgFlags::HoldAltitude))
		{
			skp->Text(xLeft - 122, yTop + 1, "ALT", 3);
		}

		if (IsProgramRunning(FCProgFlags::HoldKEAS))
		{
			skp->Text(xLeft - 162, yTop + 1, "KS", 2);
		}
		
		if (IsProgramRunning(FCProgFlags::HoldMACH))
		{
			skp->Text(xLeft - 162, yTop + 1, "MA", 2);
		}
	}
}

void FC::FlightComputer::Boot()
{
	if (!IsPowered()) return;

	MapKey(GCKey::Clear,		[this] { HandleScratchPadKey(GCKey::Clear); });
	MapKey(GCKey::D0,			[this] { HandleScratchPadKey(GCKey::D0); });
	MapKey(GCKey::D1,			[this] { HandleScratchPadKey(GCKey::D1); });
	MapKey(GCKey::D2,			[this] { HandleScratchPadKey(GCKey::D2); });
	MapKey(GCKey::D3,			[this] { HandleScratchPadKey(GCKey::D3); });
	MapKey(GCKey::D4,			[this] { HandleScratchPadKey(GCKey::D4); });
	MapKey(GCKey::D5,			[this] { HandleScratchPadKey(GCKey::D5); });
	MapKey(GCKey::D6,			[this] { HandleScratchPadKey(GCKey::D6); });
	MapKey(GCKey::D7,			[this] { HandleScratchPadKey(GCKey::D7); });
	MapKey(GCKey::D8,			[this] { HandleScratchPadKey(GCKey::D8); });
	MapKey(GCKey::D9,			[this] { HandleScratchPadKey(GCKey::D9); });
	MapKey(GCKey::Decimal,		[this] { HandleScratchPadKey(GCKey::Decimal); });
	MapKey(GCKey::PlusMinus,	[this] { HandleScratchPadKey(GCKey::PlusMinus); });

	MapKey(GCKey::Menu,			[this] { PageMain(); });
	MapKey(GCKey::Previous,		[this] { });
	MapKey(GCKey::Next,			[this] {});
	MapKey(GCKey::Enter,		[this] {});
	MapKey(GCKey::HUD,			[this] {});
	MapKey(GCKey::F1,			[this] {});
	MapKey(GCKey::F2,			[this] {});
	MapKey(GCKey::F3,			[this] {});
	MapKey(GCKey::F4,			[this] {});
	MapKey(GCKey::F5,			[this] {});
	MapKey(GCKey::F6,			[this] {});
	MapKey(GCKey::F7,			[this] {});
	MapKey(GCKey::F8,			[this] {});
	MapKey(GCKey::F9,			[this] {});
	MapKey(GCKey::F10,			[this] {});
	
	PageMain();
	isRunning_ = true;
}

void FC::FlightComputer::HandlePostStep(bco::Vessel& Vessel, double simt, double simdt, double mjd)
{
	// Handle low lever computer stuff, boot etc.
	Update();

	if (!isRunning_) return;

    // Handle key input
    if (!keyBuffer_.empty())
    {
        auto key = keyBuffer_.back();
        keyBuffer_.pop_back();
		mapKeyFunc_[key]();
    }

	// Give control programs simulator time.
	//vesselCtrl_.Step(simt, simdt, mjd);
	if (prevRunningProgs != runningPrograms_) UpdateProgs(Vessel, runningPrograms_);

	auto atmoOn = IsProgramRunning(FCProgFlags::AtmoActive);
	auto isHoldAlt = (atmoOn && IsProgramRunning(FCProgFlags::HoldAltitude));
	auto isHoldHdg = (atmoOn && IsProgramRunning(FCProgFlags::HoldHeading));
	auto isHoldKEAS = (atmoOn && IsProgramRunning(FCProgFlags::HoldKEAS));
	auto isHoldMACH = (atmoOn && IsProgramRunning(FCProgFlags::HoldMACH));

	if (isHoldAlt)	prgHoldAltitude_.step(Vessel, simt, simdt, mjd);
	if (isHoldHdg)	prgHoldHeading_.step(Vessel, simt, simdt, mjd);
	if (isHoldKEAS)	prgHoldKeas_.step(Vessel, simt, simdt, mjd);
	if (isHoldMACH)	prgHoldMach_.step(Vessel, simt, simdt, mjd);

	pnlHUDTile_.SetPosition(atmoOn ? 0 : 1);
	pnlHUDText1_.SetPosition(isHoldHdg ? 1 : 0);
	pnlHUDText2_.SetPosition(isHoldAlt ? 2 : 0);
	pnlHUDText3_.SetPosition((isHoldKEAS || isHoldMACH) ? (isHoldKEAS ? 3 : 4) : 0);
	
	prevRunningProgs = runningPrograms_;

	apDspMain_		.set_state(atmoOn);
	apDspHeading_	.set_state(IsProgramRunning(FCProgFlags::HoldHeading));
	apDspAltitude_	.set_state(IsProgramRunning(FCProgFlags::HoldAltitude));
	apDspKEAS_		.set_state(IsProgramRunning(FCProgFlags::HoldKEAS));
	apDspMACH_		.set_state(IsProgramRunning(FCProgFlags::HoldMACH));

	// Now call the update method for the active page.
	if (fabs(simt - prevTime_) > 0.2)
	{
		funcUpdate_();
		prevTime_ = simt;
	}
}

void FC::FlightComputer::ClearScreen()
{
	for (int i = 0; i < DISPLAY_ROWS; i++)
	{
		DisplayLine(i, "                   ");
	}
}

void FC::FlightComputer::DisplayLine(int row, char* mask, ...)
{
	va_list argptr;
	va_start(argptr, mask);
	vsnprintf(display_[row], DISPLAY_COLS, mask, argptr);
	va_end(argptr);

	isDisplayDirty_ = true;
}

void FC::FlightComputer::DisplayText(int row, int col, const char* text)
{
	if (row < 0 || row > DISPLAY_ROWS - 1) return;
	if (col < 0 || col > DISPLAY_COLS - 1) return;

	int offs = col;
	size_t tIdx = 0;
	auto textLen = strnlen(text, DISPLAY_COLS);

	while (offs < DISPLAY_COLS && tIdx < textLen)
	{
		if (text[tIdx] != '~')		// use ~ to mask parts of a string not to update.
		{
			display_[row][offs] = text[tIdx];
		}

		tIdx++;
		offs++;
	}

	isDisplayDirty_ = true;
}

double FC::FlightComputer::GetScratchPad()
{
	auto temp = scratchValue_;
	ClearScratchPad();
	return temp;
}

void FC::FlightComputer::SetScratchPad(double Value)
{
	scratchValue_ = Value;
	DisplayLine(10, "[%10.2f]       ", scratchValue_);
}

bool FC::FlightComputer::HandleLoacVC(bco::Vessel& Vessel, int vcid)
{
    auto vcMeshHandle = Vessel.GetVCMeshHandle0();
    assert(vcMeshHandle != nullptr);

    SURFHANDLE surfHandle = oapiGetTextureHandle(vcMeshHandle, bm::vc::TXIDX_SR71R_100_VC2_dds);

    // handle_set_class_caps our font:
    vcFont_.surfSource = surfHandle;
    vcFont_.charWidth = 12;
    vcFont_.charHeight = 20;
    vcFont_.sourceX = 4;
    vcFont_.sourceY = 2021;
    vcFont_.blankX = 1600;
    vcFont_.blankY = 2021;

    oapiVCRegisterArea(
        allId_,
        _R(80, 835, 320, 1060), //_R(1710, 95, 1950, 320),
        PANEL_REDRAW_USER,
        PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
        PANEL_MAP_BACKGROUND,
        surfHandle);

    return true;
}

bool FC::FlightComputer::HandleRedrawVC(bco::Vessel& Vessel, int id, int event, SURFHANDLE surf)
{
	for (int i = 0; i < DISPLAY_ROWS; i++) {
		bco::DrawSurfaceText(0, i * 20, display_[i], bco::DrawTextFormat::Left, surf, vcFont_);
	}

	return true;
}

bool FC::FlightComputer::HandleLoadPanel(bco::Vessel& Vessel, int id, PANELHANDLE hPanel)
{
	auto panelMesh = Vessel.GetpanelMeshHandle0();
	SURFHANDLE surfHandle = oapiGetTextureHandle(panelMesh, bm::pnl::TXIDX_SR71R_100_2DPanel_dds);

	// handle_set_class_caps our font:
	vcFont_.surfSource = surfHandle;
	vcFont_.charWidth = 12;
	vcFont_.charHeight = 20;
	vcFont_.sourceX = 4;
	vcFont_.sourceY = 2;
	vcFont_.blankX = 1600;
	vcFont_.blankY = 2;

	Vessel.RegisterPanelArea(
		hPanel,
		allId_,
//		_R(1565, 2152, 1808, 2371), //_R(1710, 95, 1950, 320),
		bm::pnl::pnlFCScreen_RC,
		PANEL_REDRAW_USER,
		PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
		surfHandle);
	
	return true;
}

bool FC::FlightComputer::HandleRedrawPanel(bco::Vessel& Vessel, int id, int event, SURFHANDLE surf)
{
	int left = 1585;
	int top = 2150;
	for (int i = 0; i < DISPLAY_ROWS; i++) {
		bco::DrawSurfaceText(left, top + (i * 20), display_[i], bco::DrawTextFormat::Left, surf, vcFont_);
	}

	return true;
}

bool FC::FlightComputer::HandleLoadState(bco::Vessel& Vessel, const std::string& line)
{
	std::istringstream in(line);
	bool isOn, isHoldHeading, isHoldAltitude, isHoldSpeed, isHoldMACH;
	if (in >> isOn >> isHoldHeading >> isHoldAltitude >> isHoldSpeed >> isHoldMACH) {
		SetProgramState(FCProgFlags::AtmoActive,	isOn);
		SetProgramState(FCProgFlags::HoldHeading,	isHoldHeading);
		SetProgramState(FCProgFlags::HoldAltitude,	isHoldAltitude);
		SetProgramState(FCProgFlags::HoldKEAS,		(isHoldSpeed && !isHoldMACH));
		SetProgramState(FCProgFlags::HoldMACH,		(isHoldSpeed && isHoldMACH));
	}

	return true;
}

std::string FC::FlightComputer::HandleSaveState(bco::Vessel& Vessel)
{
	std::ostringstream os;
	os << IsProgramRunning(FCProgFlags::AtmoActive)
		<< " " << IsProgramRunning(FCProgFlags::HoldHeading)
		<< " " << IsProgramRunning(FCProgFlags::HoldAltitude)
		<< " " << (IsProgramRunning(FCProgFlags::HoldKEAS) || IsProgramRunning(FCProgFlags::HoldMACH))
		<< " " << IsProgramRunning(FCProgFlags::HoldMACH);
	
	return os.str();
}

void FC::FlightComputer::PageMain()
{
	ClearScreen();
	DisplayLine(0, "MAIN         PG 1:1");
	DisplayLine(1, "<ASCENT");
	DisplayLine(3, "<ORBIT OPS");
	DisplayLine(5, "<REENTRY");
	DisplayLine(7, "<AUTO PILOT");

	ClearFuncKeys();
	MapKey(GCKey::F1, [this] { PageAscent(); });
	MapKey(GCKey::F2, [this] { PageOrbitOps(); });
	MapKey(GCKey::F3, [this] { PageReEntry(); });
	MapKey(GCKey::F4, [this] { PageAtmosphere(); });

	funcUpdate_ = [] {};
}

void FC::FlightComputer::PageAtmosphere()
{
	ClearScreen();
	DisplayLine(0, "AUTO PILOT   PG 1:1");

	ClearFuncKeys();
	MapKey(GCKey::F1,	[this] { ToggleAtmoProgram(FCProgFlags::HoldHeading); });
	MapKey(GCKey::F2,	[this] { ToggleAtmoProgram(FCProgFlags::HoldAltitude); });
	MapKey(GCKey::F3,	[this] { ToggleAtmoProgram(FCProgFlags::HoldKEAS); });
	MapKey(GCKey::F4,	[this] { ToggleAtmoProgram(FCProgFlags::HoldMACH); });
	MapKey(GCKey::F10,	[this] { ToggleAtmoProgram(FCProgFlags::AtmoActive); });

	prevRunning_ = FCProgFlags::HoldInvalid;  // Force re-eval
	funcUpdate_ = [this] { UpdateAtmospherePage(); };
}

void FC::FlightComputer::UpdateAtmospherePage()
{
	if (runningPrograms_ != prevRunning_)
	{
		DisplayLine(1, "<HDG   -%s-", IsProgramRunning(FCProgFlags::HoldHeading) ? "ON " : " . ");
		DisplayLine(3, "<ALT   -%s-", IsProgramRunning(FCProgFlags::HoldAltitude) ? "ON " : " . ");
		DisplayLine(5, "<KEAS  -%s-", IsProgramRunning(FCProgFlags::HoldKEAS) ? "ON " : " . ");
		DisplayLine(7, "<MACH  -%s-", IsProgramRunning(FCProgFlags::HoldMACH) ? "ON " : " . ");
		DisplayLine(9, " -%s-   -%s->", 
			IsProgramRunning(FCProgFlags::AtmoActive) ? "RUNNING" : "   .   ",
			IsProgramRunning(FCProgFlags::AtmoActive) ? "STOP>" : " RUN>");
		
		prevRunning_ = runningPrograms_;
	}
}

void FC::FlightComputer::PageAscent()
{
	ClearScreen();
	DisplayLine(0, "ASCENT       PG 1:1");

	ClearFuncKeys();
	MapKey(GCKey::F1, [this] { SetTargetIncDeg(GetScratchPad()); });

	MapKey(GCKey::F7, [this] { headingSignal_.Fire(ascentHeading_); });
	MapKey(GCKey::F8, [this] { headingSignal_.Fire(ascentHeadingAlt_); });

	// Target cannot be less than lat.
	double vlng, vlat, vrad;
	OBJHANDLE hRef = vessel_.GetEquPos(vlng, vlat, vrad);

	launchLatitude_ = vlat;
	SetTargetIncDeg(DEG * launchLatitude_);

	funcUpdate_ = [this] { UpdateAscentPage(); };
}

void FC::FlightComputer::UpdateAscentPage()
{
	if (!isAscentPageDirty_) return;

	DisplayLine(1, "<TGT INC  :%6.2f", DEG * ascentTargetInc_);
	DisplayLine(3, "       SET HDG %3.0f>", DEG * ascentHeading_);
	DisplayLine(5, "       SET HDG %3.0f>", DEG * ascentHeadingAlt_);

	isAscentPageDirty_ = false;
}

/**	SetTargetIncDeg
	@param tgt Target Inclination in degrees.
	Takes the target inclincation and sets the ascentHeading, both
	direct and alternate. (-180).  Inclination must be valid for 
	the current launchLatitude.
*/
void FC::FlightComputer::SetTargetIncDeg(double tgt)
{
	if (bco::CalcLaunchHeading(launchLatitude_, RAD * tgt, ascentHeading_, ascentHeadingAlt_)) {
		ascentTargetInc_ = tgt * RAD;
	}
	else {
		SetScratchError("Invalid Input");
	}

	isAscentPageDirty_ = true;
}

/**
	Boots the computer if needed, checks for dirty display.
	Does not update in program states.
*/
void FC::FlightComputer::Update()
{
	if (!isRunning_) {
		if (IsPowered()) {
			Boot();
		}
	}
	else {
		if (!IsPowered()) {
			ClearScreen();
			isRunning_ = false;
		}
	}

    if (isDisplayDirty_) {
        oapiTriggerRedrawArea(0, 0, allId_);
        isDisplayDirty_ = false;
    }
}

void FC::FlightComputer::ClearFuncKeys()
{
	MapKey(GCKey::F1);
	MapKey(GCKey::F2);
	MapKey(GCKey::F3);
	MapKey(GCKey::F4);
	MapKey(GCKey::F5);
	MapKey(GCKey::F6);
	MapKey(GCKey::F7);
	MapKey(GCKey::F8);
	MapKey(GCKey::F9);
	MapKey(GCKey::F10);
}

void FC::FlightComputer::ClearScratchPad()
{
	scratchKeys_.clear();
	scratchIsPos_ = true;
	SetScratchPad(0.0);
}

void FC::FlightComputer::SetScratchError(const char* msg)
{
	isScratchError_ = true;
	DisplayLine(10, "!! %s", msg);
}

bool FC::FlightComputer::HandleScratchPadKey(FC::GCKey key)
{
	if (key == FC::GCKey::Clear) {
		ClearScratchPad();
		isScratchError_ = false;
		return true;
	}

	if (isScratchError_) return false;

	auto evaluate = false;
	double newValue = 0.0;

	switch (key) {
	case FC::GCKey::D0:
	case FC::GCKey::D1:
	case FC::GCKey::D2:
	case FC::GCKey::D3:
	case FC::GCKey::D4:
	case FC::GCKey::D5:
	case FC::GCKey::D6:
	case FC::GCKey::D7:
	case FC::GCKey::D8:
	case FC::GCKey::D9:
		if (scratchKeys_.size() < 8) {
			scratchKeys_.append(std::to_string(key));
			evaluate = true;
		}
		break;

	case FC::GCKey::Decimal:
		if (scratchKeys_.find('.') == std::string::npos) {
			scratchKeys_.append(".");
			evaluate = true;
		}
		break;

	case FC::GCKey::PlusMinus:
		scratchIsPos_ = !scratchIsPos_;
		evaluate = true;
		break;
	}

	if (evaluate) {
		newValue = atof(scratchKeys_.c_str());
		if (!scratchIsPos_) newValue *= -1.0;

		SetScratchPad(newValue);
	}

	return true;
}