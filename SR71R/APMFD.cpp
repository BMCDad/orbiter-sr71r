//	APMFD - SR-71r Orbiter Addon
//	Copyright(C) 2024  Blake Christensen
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

#include "..\bc_orbiter\Tools.h"
#include "APMFD.h"

APMFD::APMFD(DWORD w, DWORD h, VESSEL* vessel) : MFD2(w, h, vessel) {
    sr71Vessel_ = (SR71Vessel*)vessel;
    ascent_ = sr71Vessel_->get_ascent();
    // Allocate pens, etc.
}

APMFD::~APMFD() {
    // Release pens, etc. here.
}

bool APMFD::Update(oapi::Sketchpad* skp)
{
    Title(skp, "SR71 Auto Pilots");

    double longitude = ascent_->vessel_longitude();
    double latitude = ascent_->vessel_latitude();
    double radius = ascent_->vessel_radius();

    skp->SetTextColor(0x00ff00);
    char cbuf[256];

    auto lbleft = cw;
    auto vleft = cw * 14;
    auto lbright = cw * 25;
    auto vright = cw * 37;
    
    skp->SetTextAlign(oapi::Sketchpad::LEFT);

    skp->Text(lbleft, ch*1, "Lng:", 4);
    skp->Text(lbleft, ch*2, "Lat:", 4);
    skp->Text(lbleft, ch*3, "Gst:", 4);
    skp->Text(lbleft, ch*4, "Lst:", 4);

    skp->SetTextAlign(oapi::Sketchpad::RIGHT);
    sprintf(cbuf, "%0.2fº", longitude * DEG);
    skp->Text(vleft, ch*1, cbuf, strlen(cbuf));
    sprintf(cbuf, "%0.2fº", latitude * DEG);
    skp->Text(vleft, ch*2, cbuf, strlen(cbuf));
    
    auto gst = bc_orbiter::greenwich_siderial(oapiGetSimMJD());
    
    sprintf(cbuf, "%0.3f", gst);
    skp->Text(vleft, ch*3, cbuf, strlen(cbuf));

    auto local_lon = (longitude < 0) ? PI2 + longitude : longitude;
    auto lst = fmod(gst + ((local_lon * DEG) / 15), 24);

    sprintf(cbuf, "%0.3f", lst);
    skp->Text(vleft, ch*4, cbuf, strlen(cbuf));

    skp->SetTextColor(0x00ffff);
    skp->SetTextAlign(oapi::Sketchpad::LEFT);

    skp->Text(lbright, ch * 1, "Inc:", 4);
    skp->Text(lbright, ch * 2, "LAN:", 4);
    skp->Text(lbright, ch * 3, "Lst:", 4);
    skp->Text(lbright, ch * 4, "TAz:", 4);

    skp->SetTextAlign(oapi::Sketchpad::RIGHT);
    sprintf(cbuf, "%0.2fº", ascent_->target_inclination() * DEG);
    skp->Text(vright, ch * 1, cbuf, strlen(cbuf));
    sprintf(cbuf, "%0.2fº", ascent_->target_lan() * DEG);
    skp->Text(vright, ch * 2, cbuf, strlen(cbuf));
    sprintf(cbuf, "%0.3f", ascent_->target_lst());
    skp->Text(vright, ch * 3, cbuf, strlen(cbuf));
    if (ascent_->launch_heading() != 0.0)
        sprintf(cbuf, "%0.2fº", ascent_->launch_heading() * DEG);
    else
        sprintf(cbuf, "n/s");
    skp->Text(vright, ch * 4, cbuf, strlen(cbuf));




    //tm date;
    //bc_orbiter::mjddate(oapiGetSimMJD(), date);


    return true;
}

char* APMFD::ButtonLabel(int bt)
{
    auto lb = labels_.find(bt);
    if (lb == labels_.end()) return nullptr;
    return (char*)lb->second.c_str();
}

int APMFD::ButtonMenu(const MFDBUTTONMENU** menu) const
{
    if (menu == 0) { return menu_.size(); }

    if (menu) *menu = (MFDBUTTONMENU*)menu_.data();
    return menu_.size();
}

bool APMFD::ConsumeButton(int bt, int event)
{
    return false;
}

int APMFD::MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
    case OAPI_MSG_MFD_OPENED:
        return (int)(new APMFD(LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
    }
    
    return 0;
}
