/*
MFDs - SR-71r Orbiter Addon
Copyright(C) 2025  Blake Christensen

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <cmath>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"
#include "..\bc_orbiter\Tools.h"
#include "..\bc_orbiter\Types.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"


#include "IAvionics.h"
#include "IPowerProvider.h"

namespace bco = bc_orbiter;

class MFDs
{
    public:
    MFDs() = default;
    ~MFDs() = default;
    void Setup(bco::Vessel& vessel);
    void LoadVC(bco::Vessel& vessel);
    void LoadPanel(bco::Vessel& vessel, PANELHANDLE handle, MESHHANDLE mesh);    // Called when the 2D panel is loaded to setup animations.

    void ResetMFD(bco::Vessel& vessel, int mfd);

    void UpdateVCUI(bco::Vessel& vessel, MESHHANDLE mesh);   // Called to update the VC UI when the VC is active.
    void UpdateMainPanelUI(MESHHANDLE mesh);    // Called to update the 2D panel UI when the panel is active.
private:
    bco::FontInfo vcFont_;

    // METS:
// VC MFD Mesh coordinates:

    const double MFDY = 15.2765;

    // Mesh button row center points (z)
    const double MFDBRAD = 0.006;	// Radius to use for hit test.

    // Texture button column left/right
    // left
    const int MFDLLTEXL = 16;
    const int MFDLLTEXR = 76;
    const int MFDLRTEXL = 556;
    const int MFDLRTEXR = 616;

    // right
    const int MFDRLTEXL = 650;
    const int MFDRLTEXR = 710;
    const int MFDRRTEXL = 1190;
    const int MFDRRTEXR = 1250;

    // Texture button col top and bottom
    const int MFD_B1TEX_TOP = 1547;
    const int MFD_B1TEX_BOT = 1587;
    const int MFD_B2TEX_TOP = 1603;
    const int MFD_B2TEX_BOT = 1643;
    const int MFD_B3TEX_TOP = 1660;
    const int MFD_B3TEX_BOT = 1700;
    const int MFD_B4TEX_TOP = 1715;
    const int MFD_B4TEX_BOT = 1755;
    const int MFD_B5TEX_TOP = 1772;
    const int MFD_B5TEX_BOT = 1812;
    const int MFD_B6TEX_TOP = 1828;
    const int MFD_B6TEX_BOT = 1868;
    const int MFD_BPTEX_TOP = 1778;
    const int MFD_BPTEX_BOT = 1838;

    const int MFD_BUTTON_WIDTH = MFDLLTEXR - MFDLLTEXL;
    const int MFD_BUTTON_HEIGHT = 60;

    struct MFDData
    {
        int id;
        int key;
        int row;
        int col;
        const VECTOR3& vcLoc;
        const RECT& pnlRC;
    };

    std::vector<MFDData> left_data_
    {
        { -1, 0,  0, 0, bm::vc::MFCLeftL1_loc, bm::pnl::pnlLeftMFD1_RC },
        { -1, 1,  1, 0, bm::vc::MFCLeftL2_loc, bm::pnl::pnlLeftMFD2_RC },
        { -1, 2,  2, 0, bm::vc::MFCLeftL3_loc, bm::pnl::pnlLeftMFD3_RC },
        { -1, 3,  3, 0, bm::vc::MFCLeftL4_loc, bm::pnl::pnlLeftMFD4_RC },
        { -1, 4,  4, 0, bm::vc::MFCLeftL5_loc, bm::pnl::pnlLeftMFD5_RC },
        { -1, 5,  5, 0, bm::vc::MFCLeftL6_loc, bm::pnl::pnlLeftMFD6_RC },
        { -1, 6,  0, 1, bm::vc::MFCLeftR1_loc, bm::pnl::pnlLeftMFD7_RC },
        { -1, 7,  1, 1, bm::vc::MFCLeftR2_loc, bm::pnl::pnlLeftMFD8_RC },
        { -1, 8,  2, 1, bm::vc::MFCLeftR3_loc, bm::pnl::pnlLeftMFD9_RC },
        { -1, 9,  3, 1, bm::vc::MFCLeftR4_loc, bm::pnl::pnlLeftMFD10_RC },
        { -1, 10, 4, 1, bm::vc::MFCLeftR5_loc, bm::pnl::pnlLeftMFD11_RC },
        { -1, 11, 5, 1, bm::vc::MFCLeftR6_loc, bm::pnl::pnlLeftMFD12_RC }
    };
    int eventId_LeftPwr_{ -1 };
    int eventId_LeftSel_{ -1 };
    int eventId_LeftMenu_{ -1 };
    
    std::vector<MFDData> right_data_
    {
        { -1, 0,  0, 0, bm::vc::MFCRightL1_loc, bm::pnl::pnlRightMFD1_RC },
        { -1, 1,  1, 0, bm::vc::MFCRightL2_loc, bm::pnl::pnlRightMFD2_RC },
        { -1, 2,  2, 0, bm::vc::MFCRightL3_loc, bm::pnl::pnlRightMFD3_RC },
        { -1, 3,  3, 0, bm::vc::MFCRightL4_loc, bm::pnl::pnlRightMFD4_RC },
        { -1, 4,  4, 0, bm::vc::MFCRightL5_loc, bm::pnl::pnlRightMFD5_RC },
        { -1, 5,  5, 0, bm::vc::MFCRightL6_loc, bm::pnl::pnlRightMFD6_RC },
        { -1, 6,  0, 1, bm::vc::MFCRightR1_loc, bm::pnl::pnlRightMFD7_RC },
        { -1, 7,  1, 1, bm::vc::MFCRightR2_loc, bm::pnl::pnlRightMFD8_RC },
        { -1, 8,  2, 1, bm::vc::MFCRightR3_loc, bm::pnl::pnlRightMFD9_RC },
        { -1, 9,  3, 1, bm::vc::MFCRightR4_loc, bm::pnl::pnlRightMFD10_RC },
        { -1, 10, 4, 1, bm::vc::MFCRightR5_loc, bm::pnl::pnlRightMFD11_RC },
        { -1, 11, 5, 1, bm::vc::MFCRightR6_loc, bm::pnl::pnlRightMFD12_RC }
    };
    int eventId_RightPwr_{ -1 };
    int eventId_RightSel_{ -1 };
    int eventId_RightMenu_{ -1 };

    void DrawVCButton(int mfdId, int btnId, SURFHANDLE surf)
    {
        bco::DrawBlankText(10, 13, surf, vcFont_);

        const char* label = NULL;
        label = oapiMFDButtonLabel(mfdId, btnId);

        if (NULL != label) {
            bco::DrawSurfaceText(30, 13, label, bco::DrawTextFormat::Center, surf, vcFont_);
        }
    }

    void DrawPanelButton(int mfdId, int btnId, int row, int col, SURFHANDLE surf)
    {
        const char* label = NULL;
        label = oapiMFDButtonLabel(mfdId, btnId);

        auto ColLeftX = (mfdId == MFD_LEFT) ? 235 : 1786;
        auto xPos = ColLeftX + (col * 540);  // Cols diff (540)
        auto yPos = 1941 + (row * 56);   // Pnl rows top (1941), Pnl rows eif (56)
        bco::DrawBlankText(xPos - 20, yPos, surf, vcFont_);

        if (NULL != label) {
            bco::DrawSurfaceText(xPos, yPos, label, bco::DrawTextFormat::Center, surf, vcFont_);
        }
    }
};

inline void MFDs::Setup(bco::Vessel& vessel)
{
      // Register events:
      for (auto& a : left_data_)
      {
         a.id = vessel.RegisterEventHandler([a](int id, int event) { return oapiProcessMFDButton(MFD_LEFT, a.key, event); });
         vessel.RegisterVCRedrawEvent(a.id, [this, a](int id, int event, SURFHANDLE surf, DEVMESHHANDLE) { DrawVCButton(MFD_LEFT, a.key, surf); });
         vessel.AddPanelRedrawEvent(a.id, [this, a](int id, int event, SURFHANDLE surf) { DrawPanelButton(MFD_LEFT, a.key, a.row, a.col, surf); });
      }

      eventId_LeftPwr_ = vessel.RegisterEventHandler([this](int id, int event) { oapiToggleMFD_on(MFD_LEFT); return true; });
      eventId_LeftSel_ = vessel.RegisterEventHandler([this](int id, int event) { oapiSendMFDKey(MFD_LEFT, OAPI_KEY_F1); return true; });
      eventId_LeftMenu_ = vessel.RegisterEventHandler([this](int id, int event) { oapiSendMFDKey(MFD_LEFT, OAPI_KEY_GRAVE); return true; });

      for (auto& a : right_data_)
      {
         a.id = vessel.RegisterEventHandler([a](int id, int event) { return oapiProcessMFDButton(MFD_RIGHT, a.key, event); });
         vessel.RegisterVCRedrawEvent(a.id, [this, a](int id, int event, SURFHANDLE surf, DEVMESHHANDLE) { DrawVCButton(MFD_RIGHT, a.key, surf); });
         vessel.AddPanelRedrawEvent(a.id, [this, a](int id, int event, SURFHANDLE surf) { DrawPanelButton(MFD_RIGHT, a.key, a.row, a.col, surf); });
      }

      eventId_RightPwr_ = vessel.RegisterEventHandler([this](int id, int event) { oapiToggleMFD_on(MFD_RIGHT); return true; });
      eventId_RightSel_ = vessel.RegisterEventHandler([this](int id, int event) { oapiSendMFDKey(MFD_RIGHT, OAPI_KEY_F1); return true; });
      eventId_RightMenu_ = vessel.RegisterEventHandler([this](int id, int event) { oapiSendMFDKey(MFD_RIGHT, OAPI_KEY_GRAVE); return true; });
}

inline void MFDs::LoadVC(bco::Vessel& vessel)
{
    auto meshHandle = oapiLoadMeshGlobal(bm::vc::MESH_NAME);
    SURFHANDLE surfHandle = oapiGetTextureHandle(meshHandle, bm::vc::TXIDX_SR71R_100_VC2_dds);

    // handle_set_class_caps our font:
    vcFont_.surfSource = surfHandle;
    vcFont_.charWidth = 12;
    vcFont_.charHeight = 20;
    vcFont_.sourceX = 4;
    vcFont_.sourceY = 2021;
    vcFont_.blankX = 1600;
    vcFont_.blankY = 2021;

    static VCMFDSPEC mfds_left = { 1, bm::vc::MfdLeft_id };
    oapiVCRegisterMFD(MFD_LEFT, &mfds_left);   // left MFD

    // These are texture metrics, not mesh, so they don't live in the mesh file.

    int rc_             = MFDLLTEXL;                        // Left side of left buttons;
    int rc_col2_offset  = MFDLRTEXL - MFDLLTEXL;            // Diff between right col and left
    int rc_top          = MFD_B1TEX_TOP;                    // Top of row 1 buttons
    int rc_row_offset   = MFD_B2TEX_TOP - MFD_B1TEX_TOP;    // Difference in button y axis
    int btn_height      = MFD_B1TEX_BOT - MFD_B1TEX_TOP;    // Button height
    int btn_width       = MFDLLTEXR - MFDLLTEXL;            // Button width;

    // This can be confusing:  oapiVCRegisterArea takes a RECT that is in TEXTURE space, not MESH space, this
    // tells Orbiter where to redraw.  The area ID is then also associated with a mouse event.  HOWEVER the 
    // mouse event area is defined in the oapiVCSetAreaClickmode_Spherical call below.  That takes a mesh location.
    for (auto& a : left_data_)
    {
        int rc_l = (a.col * rc_col2_offset) + rc_;
        int rc_t = (a.row * rc_row_offset) + rc_top;
        int rc_r = rc_l + btn_width;
        int rc_b = rc_t + btn_height;
        bco::LoadVCRedrawEvent(a.id, { rc_l, rc_t, rc_r, rc_b }, a.vcLoc, MFDBRAD, surfHandle);
    }

    bco::LoadVCSimpleEvent(eventId_LeftPwr_, bm::vc::MFCLeftPWR_loc, MFDBRAD);
    bco::LoadVCSimpleEvent(eventId_LeftSel_, bm::vc::MFCLeftSEL_loc, MFDBRAD);
    bco::LoadVCSimpleEvent(eventId_LeftMenu_, bm::vc::MFCLeftMNU_loc, MFDBRAD);

    static VCMFDSPEC mfds = { 1, bm::vc::MfdRight_id };
    oapiVCRegisterMFD(MFD_RIGHT, &mfds);   // left MFD

    rc_ = MFDRLTEXL;        // Left side of left buttons;

    for (auto& a : right_data_)
    {
        int rc_l = (a.col * rc_col2_offset) + rc_;
        int rc_t = (a.row * rc_row_offset) + rc_top;
        int rc_r = rc_l + btn_width;
        int rc_b = rc_t + btn_height;
        bco::LoadVCRedrawEvent(a.id, { rc_l, rc_t, rc_r, rc_b }, a.vcLoc, MFDBRAD, surfHandle);
    }

    bco::LoadVCSimpleEvent(eventId_RightPwr_,   bm::vc::MFCRightPWR_loc, MFDBRAD);
    bco::LoadVCSimpleEvent(eventId_RightSel_,   bm::vc::MFCRightSEL_loc, MFDBRAD);
    bco::LoadVCSimpleEvent(eventId_RightMenu_,  bm::vc::MFCRightMNU_loc, MFDBRAD);
}

inline void MFDs::UpdateVCUI(bco::Vessel& vessel, MESHHANDLE mesh)
{ }

inline void MFDs::UpdateMainPanelUI(MESHHANDLE mesh)
{}

inline void MFDs::LoadPanel(bco::Vessel& vessel, PANELHANDLE handle, MESHHANDLE mesh)
{
    SURFHANDLE surfHandle = oapiGetTextureHandle(mesh, bm::pnl::TXIDX_SR71R_100_2DPanel_dds);

    // handle_set_class_caps our font:
    vcFont_.surfSource = surfHandle;
    vcFont_.charWidth = 12;
    vcFont_.charHeight = 20;
    vcFont_.sourceX = 4;
    vcFont_.sourceY = 2;
    vcFont_.blankX = 1600;
    vcFont_.blankY = 2;

    // !!! Left MFD: !!!
    // Orbiter bug with panel MFDs, it expects a specific vertex sequence, so we need to re-arrange.
    static NTVERTEX VTX_MFD[1][4] = {
        {
            bm::pnl::pnlMFDLeft_vrt[2],
            bm::pnl::pnlMFDLeft_vrt[3],
            bm::pnl::pnlMFDLeft_vrt[0],
            bm::pnl::pnlMFDLeft_vrt[1]
        }
    };
    static WORD IDX_MFD[6] = {
        0,1,2,
        3,2,1
    };

    MESHGROUP grp_lmfd = { VTX_MFD[0], IDX_MFD, 4, 6, 0, 0, 0, 0, 0 };
    auto lmfd_grp = oapiAddMeshGroup(mesh, &grp_lmfd);   // left MFD

    vessel.RegisterPanelMFDGeometry(handle, MFD_LEFT, 0, lmfd_grp);

    for (auto& a : left_data_)
    {
        bco::LoadPanelRedrawEvent(vessel, a.id, handle, a.pnlRC, surfHandle);
    }

    bco::LoadPanelSimpleEvent(vessel, eventId_LeftPwr_, handle, bm::pnl::pnlLeftMFDPwr_RC);
    bco::LoadPanelSimpleEvent(vessel, eventId_LeftSel_, handle, bm::pnl::pnlLeftMFDSel_RC);
    bco::LoadPanelSimpleEvent(vessel, eventId_LeftMenu_, handle, bm::pnl::pnlLeftMFDMenu_RC);

    // !!! Right MFD: !!!
    // Orbiter bug with panel MFDs, it expects a specific vertex sequence, so we need to re-arrange.
    static NTVERTEX VTX_RMFD[1][4] = {
        {
            bm::pnl::pnlMFDRight_vrt[2],
            bm::pnl::pnlMFDRight_vrt[3],
            bm::pnl::pnlMFDRight_vrt[0],
            bm::pnl::pnlMFDRight_vrt[1]
        }
    };
    static WORD IDX_RMFD[6] = {
        0,1,2,
        3,2,1
    };

    MESHGROUP grp_rmfd = { VTX_RMFD[0], IDX_RMFD, 4, 6, 0, 0, 0, 0, 0 };
    auto rmfd_grp = oapiAddMeshGroup(mesh, &grp_rmfd);   // left MFD

    vessel.RegisterPanelMFDGeometry(handle, MFD_RIGHT, 0, rmfd_grp);

    for (auto& a : right_data_)
    {
        bco::LoadPanelRedrawEvent(vessel, a.id, handle, a.pnlRC, surfHandle);
    }

    bco::LoadPanelSimpleEvent(vessel, eventId_RightPwr_, handle,    bm::pnl::pnlRightMFDPwr_RC);
    bco::LoadPanelSimpleEvent(vessel, eventId_RightSel_, handle,    bm::pnl::pnlRightMFDSel_RC);
    bco::LoadPanelSimpleEvent(vessel, eventId_RightMenu_, handle,   bm::pnl::pnlRightMFDMenu_RC);
}

inline void MFDs::ResetMFD(bco::Vessel& vessel, int mfd)
{
    if (mfd == MFD_LEFT) {
        for (auto& p : left_data_)
        {
            vessel.TriggerRedrawArea(0, 0, p.id);
        }
    }
    else if (mfd == MFD_RIGHT) {
         for (auto& p : right_data_)
         {
               vessel.TriggerRedrawArea(0, 0, p.id);
         }
    }
}