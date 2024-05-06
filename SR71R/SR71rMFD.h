// ==============================================================
// SR-71r MFD
// ==============================================================

#pragma once

#include "Orbitersdk.h"
#include "SR71Vessel.h"


class SR71rMFD : public MFD2 {
public:
	SR71rMFD(DWORD w, DWORD h, VESSEL *vessel);
	~SR71rMFD();
	char *ButtonLabel(int bt);
	int ButtonMenu(const MFDBUTTONMENU **menu) const;
	bool Update(oapi::Sketchpad *skp);
	static int MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

protected:
	oapi::Font *font;

	SR71Vessel*		theVessel_;
};
