#define STRICT
#include "windows.h"
#include "orbitersdk.h"
#include "SR71rMFD.h"

// ==============================================================
// Global variables

int g_MFDmode; // identifier for new MFD mode

// ==============================================================
// API interface

DLLCLBK void InitModule(HINSTANCE hDLL)
{
	static char *name = "SR-71 R";   // MFD mode name
	MFDMODESPECEX spec;
	spec.name = name;
	spec.key = OAPI_KEY_T;                // MFD mode selection key
	spec.context = NULL;
	spec.msgproc = SR71rMFD::MsgProc;  // MFD mode callback function

	// Register the new MFD mode with Orbiter
	g_MFDmode = oapiRegisterMFDMode(spec);
}

DLLCLBK void ExitModule(HINSTANCE hDLL)
{
	// Unregister the custom MFD mode when the module is unloaded
	oapiUnregisterMFDMode(g_MFDmode);
}

// ==============================================================
// MFD class implementation

// Constructor
SR71rMFD::SR71rMFD(DWORD w, DWORD h, VESSEL *vessel)
	: MFD2(w, h, vessel)
{
	font = oapiCreateFont(w / 20, true, "Arial", FONT_NORMAL);
	// Add MFD initialisation here

	theVessel_ = (SR71Vessel*)vessel;
}

// Destructor
SR71rMFD::~SR71rMFD()
{
	oapiReleaseFont(font);
	// Add MFD cleanup code here
}

// Return button labels
char *SR71rMFD::ButtonLabel(int bt)
{
	// The labels for the two buttons used by our MFD mode
	static char *label[2] = { "UP", "DN" };
	return (bt < 2 ? label[bt] : 0);
}

// Return button menus
int SR71rMFD::ButtonMenu(const MFDBUTTONMENU **menu) const
{
	// The menu descriptions for the two buttons
	static const MFDBUTTONMENU mnu[2] = {
		{ "Move up", 0, '[' },
		{ "Move down", 0, ']' }
	};
	if (menu) *menu = mnu;
	return 2; // return the number of buttons used
}


// Repaint the MFD
bool SR71rMFD::Update(oapi::Sketchpad *skp)
{
	Title(skp, "MFD Template");
	// Draws the MFD title

	IPropulsionControl* prop = theVessel_->GetPropulsionControl();

	auto left = W * 0.1;
	auto right = W - left;
	auto top = H * 0.1;
	auto bottom = H - top;

	skp->Line(left, top, left, bottom);

	// Add MFD display routines here.
	// Use the device context (hDC) for Windows GDI paint functions.

	return true;
}

// MFD message parser
int SR71rMFD::MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		// Our new MFD mode has been selected, so we create the MFD and
		// return a pointer to it.
		return (int)(new SR71rMFD(LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	return 0;
}

