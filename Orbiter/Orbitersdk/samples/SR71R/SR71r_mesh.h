// Auto generated code file.  Blender: 3.5.0  Blender Tools: (2, 1, 0)
// Date: Fri Jun 16 06:24:04 2023


#include "orbitersdk.h"

#ifndef __SR71r_H
#define __SR71r_H

namespace bm 
{

// Scene SR71r

  namespace main 
  {
    const NTVERTEX WindowSFI_verts[3] = {
    {0.3303f, 0.5991f, 15.2599f, -0.6322f, -0.6280f, -0.4539f, 0.8423f, 0.0694f},
    {0.0241f, 0.8939f, 15.2785f, -0.6322f, -0.6280f, -0.4539f, 0.8719f, 0.0692f},
    {0.0297f, 0.6076f, 15.6669f, -0.6322f, -0.6280f, -0.4539f, 0.8505f, 0.1224f}    };
    const NTVERTEX CanopyWindowInsideLeft_verts[6] = {
    {-0.4317f, 0.7433f, 14.4864f, 0.7922f, -0.5886f, -0.1614f, 0.8405f, 0.1251f},
    {-0.2610f, 0.9771f, 14.5001f, 0.7922f, -0.5886f, -0.1614f, 0.8404f, 0.0677f},
    {-0.3712f, 0.6164f, 15.1668f, 0.7074f, -0.6178f, -0.3433f, 0.9667f, 0.1259f},
    {-0.0599f, 0.9239f, 15.2037f, 0.7074f, -0.6178f, -0.3433f, 0.9667f, 0.0675f},
    {-0.1872f, 0.9526f, 14.9490f, 0.7518f, -0.6070f, -0.2577f, 0.9147f, 0.0684f},
    {-0.4043f, 0.6517f, 14.9686f, 0.7500f, -0.6076f, -0.2614f, 0.9192f, 0.1256f}    };
    const NTVERTEX CanopyWindowSI_verts[6] = {
    {0.4317f, 0.7433f, 14.4864f, -0.7921f, -0.5887f, -0.1614f, 0.8405f, 0.1251f},
    {0.2610f, 0.9772f, 14.5001f, -0.7921f, -0.5887f, -0.1614f, 0.8404f, 0.0677f},
    {0.3712f, 0.6164f, 15.1668f, -0.7076f, -0.6179f, -0.3428f, 0.9667f, 0.1259f},
    {0.0597f, 0.9242f, 15.2044f, -0.7076f, -0.6179f, -0.3428f, 0.9667f, 0.0675f},
    {0.1871f, 0.9526f, 14.9490f, -0.7518f, -0.6070f, -0.2574f, 0.9147f, 0.0684f},
    {0.4044f, 0.6517f, 14.9687f, -0.7501f, -0.6076f, -0.2611f, 0.9192f, 0.1256f}    };
    const DWORD TXIDX_SR71_VCCockPit_dds = 1;
    const DWORD TXIDX_SR71rBody_dds = 2;
    constexpr auto MESH_NAME = "SR71r";

    const UINT CockpitPanelHood_id = 0;
    const UINT HoverDoorSA_id = 1;
    const UINT HoverDoorPA_id = 2;
    const UINT HoverDoorSF_id = 3;
    const UINT HoverDoorPF_id = 4;
    const UINT ElevonSIT_id = 5;
    const UINT ElevonPIT_id = 6;
    const UINT CockpitPanelF_id = 7;
    const UINT CanopyFI_id = 8;
    const UINT CargoBay_id = 9;
    const UINT Body_id = 10;
    const UINT WindowF_id = 11;
    const UINT CanopyFO_id = 12;
    const UINT LGDoorSI_id = 13;
    const UINT LGDoorPI_id = 14;
    const UINT LGCrossP_id = 15;
    const UINT LGLArm_id = 16;
    const UINT LGLFork_id = 17;
    const UINT LGLWheel2_id = 18;
    const UINT LGLWheel1_id = 19;
    const UINT LGLWheel3_id = 20;
    const UINT LGRCross_id = 21;
    const UINT LGRArm_id = 22;
    const UINT LGRFork_id = 23;
    const UINT LGRWheel1_id = 24;
    const UINT LGRWheel3_id = 25;
    const UINT LGRWheel2_id = 26;
    const UINT LGDoorSO_id = 27;
    const UINT LGDoorPO_id = 28;
    const UINT LGCrossF_id = 29;
    const UINT LGFrontArm_id = 30;
    const UINT LGFrontLeftWheel_id = 31;
    const UINT LGFrontAxel_id = 32;
    const UINT LGFrontRightWheel_id = 33;
    const UINT LGDoorPF_id = 34;
    const UINT LGDoorSF_id = 35;
    const UINT ElevonSIB_id = 36;
    const UINT RightOuterElevon_id = 37;
    const UINT ElevonPIB_id = 38;
    const UINT LeftOuterElevon_id = 39;
    const UINT OuterWings_id = 40;
    const UINT Engines_id = 41;
    const UINT RudderBase_id = 42;
    const UINT LeftRudder_id = 43;
    const UINT RightRudder_id = 44;
    const UINT EngineCone_id = 45;
    const UINT DockConnector_id = 46;
    const UINT BayDoorPF_id = 47;
    const UINT BayDoorSF_id = 48;
    const UINT BayDoorPA_id = 49;
    const UINT BayDoorSA_id = 50;
    const UINT ForwardCockpit_id = 51;
    const UINT ForwardCanopyWindow_id = 52;
    const UINT CockpitPanelHoodTop_id = 53;
    const UINT WindowSFI_id = 54;
    const UINT CanopyWindowInsideLeft_id = 55;
    const UINT CanopyWindowSI_id = 56;
    constexpr VECTOR3 TPWingP_location =     {-8.8078, -0.0127, -8.8233};
    constexpr VECTOR3 TPWingS_location =     {8.8078, -0.0127, -8.8233};
    constexpr VECTOR3 TPTail_location =     {0.0000, 0.4476, -12.7113};
    constexpr VECTOR3 TPRudder_location =     {0.0000, 3.4638, -9.5882};
    constexpr VECTOR3 TPHood_location =     {0.0000, 1.0369, 15.1190};
    constexpr VECTOR3 TPNose_location =     {0.0000, -0.1994, 20.0974};
    constexpr VECTOR3 ThrustRetroP_location =     {-4.3800, 0.0000, 1.9977};
    constexpr VECTOR3 ThrustRetroS_location =     {4.3800, 0.0000, 1.9977};
    constexpr VECTOR3 ThrustHoverF_location =     {0.0000, -0.2311, 5.0000};
    constexpr VECTOR3 ThrustHoverP_location =     {-3.0000, 0.0050, -5.0000};
    constexpr VECTOR3 ThrustHoverS_location =     {3.0000, 0.0050, -5.0000};
    constexpr VECTOR3 HoverDoorAxisPFA_location =     {-0.5010, -0.4450, 4.5090};
    constexpr VECTOR3 HoverDoorAxisPFF_location =     {-0.5010, -0.4450, 5.5110};
    constexpr VECTOR3 HoverDoorAxisSFF_location =     {0.5010, -0.4450, 5.5110};
    constexpr VECTOR3 HoverDoorAxisSFA_location =     {0.5010, -0.4450, 4.5090};
    constexpr VECTOR3 HoverDoorAxisPA_location =     {-3.5070, -0.0182, -5.5110};
    constexpr VECTOR3 HoverDoorAxisPF_location =     {-3.5070, -0.0354, -4.5090};
    constexpr VECTOR3 HoverDoorAxisSA_location =     {3.5070, -0.0182, -5.5110};
    constexpr VECTOR3 HoverDoorAxisSF_location =     {3.5070, -0.0354, -4.5090};
    constexpr VECTOR3 StrobeLightP_location =     {-3.6518, 3.4787, -7.8293};
    constexpr VECTOR3 StrobeLightS_location =     {3.6518, 3.4787, -7.8293};
    constexpr VECTOR3 NavLightTail_location =     {0.0000, 0.4364, -12.7415};
    constexpr VECTOR3 NavLightS_location =     {8.5460, 0.0000, -7.9544};
    constexpr VECTOR3 NavLightP_location =     {-8.5460, 0.0000, -7.9544};
    constexpr VECTOR3 BeaconBottom_location =     {-0.0042, -0.6031, 2.5301};
    constexpr VECTOR3 BeaconTop_location =     {-0.0042, 1.1307, 2.5301};
    constexpr VECTOR3 BrakeDragPoint_location =     {0.0000, 0.0000, -10.1577};
    constexpr VECTOR3 AirBrakeAxisPBO_location =     {-3.4266, 0.1097, -9.4846};
    constexpr VECTOR3 AirBrakeAxisPTO_location =     {-3.4266, 0.1338, -9.4846};
    constexpr VECTOR3 AirBrakeAxisPBI_location =     {-0.6781, 0.2195, -10.2013};
    constexpr VECTOR3 AirBrakeAxisPTI_location =     {-0.6781, 0.2879, -10.2013};
    constexpr VECTOR3 AirBrakeAxisSBO_location =     {3.4266, 0.1097, -9.4846};
    constexpr VECTOR3 AirBrakeAxisSTO_location =     {3.4266, 0.1338, -9.4846};
    constexpr VECTOR3 AirBrakeAxisSBI_location =     {0.6782, 0.2195, -10.2013};
    constexpr VECTOR3 AirBrakeAxisSTI_location =     {0.6782, 0.2879, -10.2013};
    constexpr VECTOR3 CockpitAxisS_location =     {0.2700, 1.0463, 13.8123};
    constexpr VECTOR3 CockpitAxisP_location =     {-0.2700, 1.0463, 13.8123};
    constexpr VECTOR3 DockingPort_location =     {0.0000, 0.5292, 12.1462};
    constexpr VECTOR3 Bay2AxisSA_location =     {0.8797, 0.5366, 5.6548};
    constexpr VECTOR3 Bay2AxisSF_location =     {0.8600, 0.5366, 11.4888};
    constexpr VECTOR3 Bay2AxisPA_location =     {-0.8797, 0.5366, 5.6548};
    constexpr VECTOR3 Bay2AxisPF_location =     {-0.8600, 0.5366, 11.4888};
    constexpr VECTOR3 Bay1AxisSA_location =     {0.8046, 0.5370, 12.2323};
    constexpr VECTOR3 Bay1AxisSF_location =     {0.6661, 0.5370, 13.5433};
    constexpr VECTOR3 Bay1AxisPA_location =     {-0.8046, 0.5370, 12.2323};
    constexpr VECTOR3 Bay1AxisPF_location =     {-0.6661, 0.5370, 13.5433};
    constexpr VECTOR3 RCS_R_FORWARD_location =     {0.4603, 0.0202, 17.6352};
    constexpr VECTOR3 RCS_L_FORWARD_location =     {-0.4603, 0.0202, 17.6352};
    constexpr VECTOR3 RCS_L_BOTTOM_location =     {-5.5000, -0.0007, 0.0000};
    constexpr VECTOR3 RCS_L_TOP_location =     {-5.5000, 0.1094, 0.0000};
    constexpr VECTOR3 RCS_R_BOTTOM_location =     {5.5000, -0.0007, 0.0000};
    constexpr VECTOR3 RCS_R_TOP_location =     {5.5000, 0.1094, 0.0000};
    constexpr VECTOR3 RCS_FL_LEFT_location =     {-0.4611, 0.0000, 17.5924};
    constexpr VECTOR3 RCS_FR_RIGHT_location =     {0.4599, 0.0000, 17.5924};
    constexpr VECTOR3 RCS_RR_RIGHT_location =     {0.1891, 0.0000, -10.9224};
    constexpr VECTOR3 RCS_RL_LEFT_location =     {-0.1891, 0.0000, -10.9224};
    constexpr VECTOR3 RCS_RR_UP_location =     {0.5044, 0.8613, -8.0000};
    constexpr VECTOR3 RCS_RL_UP_location =     {-0.5044, 0.8613, -8.0000};
    constexpr VECTOR3 RCS_FL_UP_location =     {-0.4761, 0.0149, 17.5113};
    constexpr VECTOR3 RCS_FR_UP_location =     {0.4761, 0.0149, 17.5244};
    constexpr VECTOR3 RCS_RL_DOWN_location =     {-0.5044, -0.3530, -8.0000};
    constexpr VECTOR3 RCS_RR_DOWN_location =     {0.5044, -0.3530, -8.0521};
    constexpr VECTOR3 RCS_FR_DOWN_location =     {0.4769, -0.3031, 17.4977};
    constexpr VECTOR3 RCS_FL_DOWN_location =     {-0.4769, -0.3031, 17.4846};
    constexpr VECTOR3 PilotPOV_location =     {0.0000, 0.8204, 14.7813};
    constexpr VECTOR3 LandingTouchS_location =     {2.6492, -2.0381, -2.2199};
    constexpr VECTOR3 LandingTouchP_location =     {-2.6492, -2.0381, -2.2199};
    constexpr VECTOR3 LandingTouchF_location =     {0.0000, -2.0381, 9.9099};
    constexpr VECTOR3 LGDoorAxisPIA_location =     {-0.1629, -0.5319, -2.6792};
    constexpr VECTOR3 LGDoorAxisSIA_location =     {0.1629, -0.5319, -2.6792};
    constexpr VECTOR3 LGDoorAxisPIF_location =     {-0.1629, -0.5319, -1.8050};
    constexpr VECTOR3 LGDoorAxisSIF_location =     {0.1629, -0.5319, -1.8050};
    constexpr VECTOR3 LGDoorAxisSOA_location =     {2.8329, -0.0794, -2.6401};
    constexpr VECTOR3 LGDoorAxisSOF_location =     {2.8329, -0.0812, -1.8050};
    constexpr VECTOR3 LGCrossAxisSA_location =     {2.6575, 0.0463, -2.6266};
    constexpr VECTOR3 LGCrossAxisSF_location =     {2.6575, 0.0463, -1.7983};
    constexpr VECTOR3 LGDoorAxisPOF_location =     {-2.8315, -0.0770, -1.8050};
    constexpr VECTOR3 LGDoorAxisPOA_location =     {-2.8321, -0.0784, -2.6401};
    constexpr VECTOR3 LGCrossAxisPF_location =     {-2.6575, 0.0463, -1.7983};
    constexpr VECTOR3 LGCrossAxisPA_location =     {-2.6575, 0.0463, -2.6266};
    constexpr VECTOR3 LGFrontRightPivot_location =     {-0.3107, -0.0655, 9.9842};
    constexpr VECTOR3 LGFrontLeftPivot_location =     {0.3064, -0.0655, 9.9842};
    constexpr VECTOR3 AileronAxisSO_location =     {8.5592, -0.0012, -8.3760};
    constexpr VECTOR3 AileronAxisPO_location =     {-8.5592, -0.0012, -8.3760};
    constexpr VECTOR3 AileronAxisSI_location =     {5.5504, 0.0862, -8.6488};
    constexpr VECTOR3 AileronAxisPI_location =     {-5.5504, 0.0862, -8.6488};
    constexpr VECTOR3 LGFLFrontPivot_location =     {-0.3411, -0.4900, 12.0132};
    constexpr VECTOR3 LGFLBackPivot_location =     {-0.3505, -0.4900, 9.7597};
    constexpr VECTOR3 LGFRBackPivot_location =     {0.3505, -0.4900, 9.7597};
    constexpr VECTOR3 LGFRFrontPivot_location =     {0.3411, -0.4900, 12.0132};
    constexpr VECTOR3 ThrusterS_location =     {4.6234, 0.0000, -9.0000};
    constexpr VECTOR3 ThrusterP_location =     {-4.6234, 0.0000, -9.0000};
    constexpr VECTOR3 VertAirfoilRef_location =     {0.0000, 1.9796, -7.8691};
    constexpr VECTOR3 RudderAxisST_location =     {3.6518, 3.7517, -7.8293};
    constexpr VECTOR3 RudderAxisPT_location =     {-3.6518, 3.7517, -7.8293};
    constexpr VECTOR3 RudderAxisSB_location =     {4.1468, 1.9649, -7.8293};
    constexpr VECTOR3 RudderAxisPB_location =     {-4.1468, 1.9649, -7.8293};

  }

// Scene SR71r2D

  namespace pnl 
  {
    const NTVERTEX pnlNavKillrot_verts[4] = {
    {846.0000f, 672.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3476f, 0.3128f},
    {916.0000f, 672.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3825f, 0.3128f},
    {846.0000f, 616.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3476f, 0.2838f},
    {916.0000f, 616.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3825f, 0.2838f}    };
    const NTVERTEX pnlNavHorzLvl_verts[4] = {
    {846.0000f, 729.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4255f, 0.3126f},
    {916.0000f, 729.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4603f, 0.3126f},
    {846.0000f, 673.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4255f, 0.2839f},
    {916.0000f, 673.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4603f, 0.2839f}    };
    const NTVERTEX pnlNavPrograde_verts[4] = {
    {918.0000f, 672.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3474f, 0.2838f},
    {988.0000f, 672.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.2838f},
    {918.0000f, 616.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3474f, 0.2551f},
    {988.0000f, 616.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.2551f}    };
    const NTVERTEX pnlNavRetro_verts[4] = {
    {918.0000f, 729.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4257f, 0.2839f},
    {988.0000f, 729.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4605f, 0.2839f},
    {918.0000f, 673.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4257f, 0.2551f},
    {988.0000f, 673.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4605f, 0.2551f}    };
    const NTVERTEX pnlNavNorm_verts[4] = {
    {990.0000f, 672.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3474f, 0.2551f},
    {1060.0000f, 672.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.2551f},
    {990.0000f, 616.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3474f, 0.2264f},
    {1060.0000f, 616.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.2264f}    };
    const NTVERTEX pnlNavAntiNorm_verts[4] = {
    {990.0000f, 729.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3475f, 0.3126f},
    {1060.0000f, 729.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3824f, 0.3126f},
    {990.0000f, 673.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3475f, 0.2838f},
    {1060.0000f, 673.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3824f, 0.2838f}    };
    const NTVERTEX pnlRCSRot_verts[4] = {
    {123.4692f, 81.1468f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3473f, 0.3412f},
    {193.4692f, 81.1468f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.3412f},
    {123.4692f, 25.1468f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3473f, 0.3132f},
    {193.4692f, 25.1468f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.3132f}    };
    const NTVERTEX pnlRCSLin_verts[4] = {
    {43.0741f, 80.3027f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4255f, 0.3413f},
    {113.0741f, 80.3027f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4604f, 0.3413f},
    {43.0741f, 24.3027f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4255f, 0.3131f},
    {113.0741f, 24.3027f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4604f, 0.3131f}    };
    const NTVERTEX pnlHUDDock_verts[4] = {
    {235.2687f, 84.0573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3473f, 0.3699f},
    {305.2687f, 84.0573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.3699f},
    {235.2687f, 28.0573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3473f, 0.3418f},
    {305.2687f, 28.0573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.3418f}    };
    const NTVERTEX pnlHUDSurf_verts[4] = {
    {316.2920f, 88.0418f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4254f, 0.3699f},
    {386.2920f, 88.0418f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4603f, 0.3699f},
    {316.2920f, 32.0418f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4254f, 0.3418f},
    {386.2920f, 32.0418f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4603f, 0.3418f}    };
    const NTVERTEX pnlHUDOrbit_verts[4] = {
    {409.8770f, 82.6050f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5036f, 0.3412f},
    {479.8770f, 82.6050f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5384f, 0.3412f},
    {409.8770f, 26.6050f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5036f, 0.3132f},
    {479.8770f, 26.6050f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5384f, 0.3132f}    };
    const NTVERTEX pnlMFDLeft_verts[4] = {
    {304.1738f, 557.3519f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0000f, 1.0000f},
    {712.1738f, 557.3519f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 1.0000f, 1.0000f},
    {304.1738f, 149.3518f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0f, 0.0f},
    {712.1738f, 149.3518f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 1.0000f, 0.0000f}    };
    const NTVERTEX pnlMFDRight_verts[4] = {
    {1844.3953f, 557.3519f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0000f, 1.0000f},
    {2252.3953f, 557.3519f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 1.0000f, 1.0000f},
    {1844.3953f, 149.3518f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0f, 0.0f},
    {2252.3953f, 149.3518f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 1.0000f, 0.0000f}    };
    const NTVERTEX pnlAirBrake_verts[4] = {
    {11.6717f, 767.4300f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0098f, 0.0356f},
    {81.6717f, 767.4300f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0407f, 0.0356f},
    {11.6717f, 729.4846f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0098f, 0.0214f},
    {81.6717f, 729.4846f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0407f, 0.0214f}    };
    const NTVERTEX pnlAPUSwitch_verts[4] = {
    {292.2038f, 994.1796f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {324.7502f, 994.1796f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {292.2038f, 906.2440f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {324.7502f, 906.2440f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlScreenSwitch_verts[4] = {
    {376.2162f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {408.7457f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {376.2162f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {408.7457f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlLightNav_verts[4] = {
    {777.8757f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {810.4051f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {777.8757f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {810.4051f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlLightBeacon_verts[4] = {
    {857.9585f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {890.4879f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {857.9585f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {890.4879f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlLightStrobe_verts[4] = {
    {937.9597f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {970.4891f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {937.9597f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {970.4891f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlLightDock_verts[4] = {
    {1018.1187f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {1050.6482f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {1018.1187f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {1050.6482f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlDoorCanopy_verts[4] = {
    {455.9846f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {488.5140f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {455.9846f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {488.5140f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlDoorCargo_verts[4] = {
    {535.8281f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {568.3575f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {535.8281f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {568.3575f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlDoorHover_verts[4] = {
    {615.9783f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {648.5078f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {615.9783f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {648.5078f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlDoorRetro_verts[4] = {
    {696.1431f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {728.6725f, 994.1573f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {696.1431f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {728.6725f, 906.2676f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlPwrMain_verts[4] = {
    {1980.5995f, 774.9550f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {2013.1289f, 774.9550f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {1980.5995f, 687.0652f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {2013.1289f, 687.0652f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlPwrAvion_verts[4] = {
    {2061.2910f, 774.9550f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {2093.8206f, 774.9550f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {2061.2910f, 687.0652f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {2093.8206f, 687.0652f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlPwrCanopy_verts[4] = {
    {2141.3799f, 774.9550f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {2173.9094f, 774.9550f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {2141.3799f, 687.0652f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {2173.9094f, 687.0652f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlPwrCargo_verts[4] = {
    {2221.5039f, 774.9550f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {2254.0334f, 774.9550f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {2221.5039f, 687.0652f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {2254.0334f, 687.0652f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlPwrExtBus_verts[4] = {
    {2062.6484f, 873.3264f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {2095.1780f, 873.3264f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {2062.6484f, 785.4366f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {2095.1780f, 785.4366f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlPwrFCBus_verts[4] = {
    {2222.4790f, 873.3264f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {2255.0085f, 873.3264f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {2222.4790f, 785.4366f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {2255.0085f, 785.4366f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlPwrFC_verts[4] = {
    {2302.5981f, 806.4562f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0623f},
    {2335.1277f, 806.4562f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0623f},
    {2302.5981f, 718.5665f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0486f, 0.0208f},
    {2335.1277f, 718.5665f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0646f, 0.0208f}    };
    const NTVERTEX pnlLgtExtPwrAvail_verts[4] = {
    {1976.1337f, 852.4731f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5518f, 0.0253f},
    {2021.4833f, 852.4731f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5731f, 0.0253f},
    {1976.1337f, 807.1235f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5518f, 0.0040f},
    {2021.4833f, 807.1235f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5731f, 0.0040f}    };
    const NTVERTEX pnlLgtFCPwrAvail_verts[4] = {
    {2296.7805f, 852.4731f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5518f, 0.0253f},
    {2342.1299f, 852.4731f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5731f, 0.0253f},
    {2296.7805f, 807.1235f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5518f, 0.0040f},
    {2342.1299f, 807.1235f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5731f, 0.0040f}    };
    const NTVERTEX pnlLgtFCPwrOn_verts[4] = {
    {2167.0547f, 852.4731f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5030f, 0.0253f},
    {2212.4041f, 852.4731f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5243f, 0.0253f},
    {2167.0547f, 807.1235f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5030f, 0.0040f},
    {2212.4041f, 807.1235f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5243f, 0.0040f}    };
    const NTVERTEX pnlLgtExtPwrOn_verts[4] = {
    {2104.4646f, 852.4731f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5030f, 0.0253f},
    {2149.8140f, 852.4731f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5243f, 0.0253f},
    {2104.4646f, 807.1235f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5030f, 0.0040f},
    {2149.8140f, 807.1235f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5243f, 0.0040f}    };
    const NTVERTEX pnlAPMain_verts[4] = {
    {846.0000f, 793.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4255f, 0.1685f},
    {916.0000f, 793.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4604f, 0.1685f},
    {846.0000f, 737.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4255f, 0.1398f},
    {916.0000f, 737.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4604f, 0.1398f}    };
    const NTVERTEX pnlAPHeading_verts[4] = {
    {990.0000f, 793.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4255f, 0.1974f},
    {1060.0000f, 793.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4603f, 0.1974f},
    {990.0000f, 737.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4255f, 0.1687f},
    {1060.0000f, 737.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4603f, 0.1687f}    };
    const NTVERTEX pnlAPAltitude_verts[4] = {
    {846.0000f, 850.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3473f, 0.1974f},
    {916.0000f, 850.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.1974f},
    {846.0000f, 794.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3473f, 0.1687f},
    {916.0000f, 794.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.1687f}    };
    const NTVERTEX pnlAPKEAS_verts[4] = {
    {918.0000f, 850.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4255f, 0.2261f},
    {988.0000f, 850.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4604f, 0.2261f},
    {918.0000f, 794.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4255f, 0.1974f},
    {988.0000f, 794.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4604f, 0.1974f}    };
    const NTVERTEX pnlAPMACH_verts[4] = {
    {990.0000f, 850.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3474f, 0.2262f},
    {1060.0000f, 850.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.2262f},
    {990.0000f, 794.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3474f, 0.1975f},
    {1060.0000f, 794.2000f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.1975f}    };
    const NTVERTEX pnlLH2Switch_verts[4] = {
    {1822.9939f, 852.9133f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3474f, 0.1686f},
    {1892.9939f, 852.9133f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.1686f},
    {1822.9939f, 796.9133f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3474f, 0.1399f},
    {1892.9939f, 796.9133f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.1399f}    };
    const NTVERTEX pnlLH2Avail_verts[4] = {
    {1837.7535f, 787.3530f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5518f, 0.0253f},
    {1883.1031f, 787.3530f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5731f, 0.0253f},
    {1837.7535f, 742.0035f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5518f, 0.0040f},
    {1883.1031f, 742.0035f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5731f, 0.0040f}    };
    const NTVERTEX pnlO2Switch_verts[4] = {
    {1731.9213f, 853.4367f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3474f, 0.1398f},
    {1801.9213f, 853.4367f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.1398f},
    {1731.9213f, 797.4367f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3474f, 0.1111f},
    {1801.9213f, 797.4367f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3822f, 0.1111f}    };
    const NTVERTEX pnlO2Avail_verts[4] = {
    {1746.1576f, 787.3530f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5518f, 0.0253f},
    {1791.5072f, 787.3530f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5731f, 0.0253f},
    {1746.1576f, 742.0035f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5518f, 0.0040f},
    {1791.5072f, 742.0035f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.5731f, 0.0040f}    };
    const NTVERTEX pnlLandingGear_verts[4] = {
    {153.2238f, 855.0703f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0075f, 0.1129f},
    {223.2238f, 855.0703f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0416f, 0.1129f},
    {153.2238f, 709.9055f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0075f, 0.0562f},
    {223.2238f, 709.9055f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.0416f, 0.0562f}    };
    const NTVERTEX pnlClockTimerMinute_verts[4] = {
    {726.3527f, 664.4812f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3656f, 0.5392f},
    {766.0398f, 664.4812f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3846f, 0.5392f},
    {726.3527f, 803.3861f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3656f, 0.6072f},
    {766.0398f, 803.3861f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3846f, 0.6072f}    };
    const NTVERTEX pnlClockHour_verts[4] = {
    {726.0686f, 663.5839f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4056f, 0.5393f},
    {766.3004f, 663.5839f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4245f, 0.5393f},
    {726.0686f, 804.3950f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4056f, 0.6071f},
    {766.3004f, 804.3950f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4245f, 0.6071f}    };
    const NTVERTEX pnlClockMinute_verts[4] = {
    {727.2206f, 667.5706f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3856f, 0.5393f},
    {765.1708f, 667.5706f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4045f, 0.5393f},
    {727.2206f, 800.3960f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3856f, 0.6072f},
    {765.1708f, 800.3960f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4045f, 0.6072f}    };
    const NTVERTEX pnlClockSecond_verts[4] = {
    {726.5493f, 665.3192f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3455f, 0.5393f},
    {765.8004f, 665.3192f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3645f, 0.5393f},
    {726.5493f, 802.6982f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3455f, 0.6071f},
    {765.8004f, 802.6982f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.3645f, 0.6071f}    };
    const NTVERTEX pnlHydPress_verts[4] = {
    {87.8170f, 274.1197f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4245f, 0.5380f},
    {51.2935f, 253.4328f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4459f, 0.5380f},
    {149.9961f, 164.3399f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4245f, 0.6040f},
    {113.4725f, 143.6531f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4459f, 0.6040f}    };
    const NTVERTEX pnlLOXPress_verts[4] = {
    {87.0950f, 448.8831f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4245f, 0.5380f},
    {51.5838f, 428.7697f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4459f, 0.5380f},
    {149.5234f, 338.6630f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4245f, 0.6040f},
    {114.0122f, 318.5496f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4459f, 0.6040f}    };
    const NTVERTEX pnlLH2Press_verts[4] = {
    {87.1339f, 620.9686f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4245f, 0.5380f},
    {51.7170f, 600.9086f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4459f, 0.5380f},
    {149.5517f, 510.7673f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4245f, 0.6040f},
    {114.1348f, 490.7072f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.4459f, 0.6040f}    };
    const NTVERTEX pnlAmpMeter_verts[4] = {
    {2436.7600f, 802.2948f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.6628f, 0.0240f},
    {2408.5574f, 777.4471f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.6628f, 0.0050f},
    {2523.7273f, 703.5856f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.7306f, 0.0240f},
    {2495.5247f, 678.7379f, 0.0000f, 0.0000f, 0.0000f, -1.0000f, 0.7306f, 0.0050f}    };
    const NTVERTEX pnlVoltMeter_verts[4] = {
    {2524.0522f, 776.9352f, 0.0000f, -0.0000f, -0.0000f, -1.0000f, 0.6628f, 0.0240f},
    {2496.0710f, 802.0320f, 0.0000f, -0.0000f, -0.0000f, -1.0000f, 0.6628f, 0.0050f},
    {2436.2136f, 679.0007f, 0.0000f, -0.0000f, -0.0000f, -1.0000f, 0.7306f, 0.0240f},
    {2408.2324f, 704.0975f, 0.0000f, -0.0000f, -0.0000f, -1.0000f, 0.7306f, 0.0050f}    };
    const DWORD TXIDX_SR71R_100_2DPanel_dds = 1;
    const DWORD TXIDX_SR71R_100_VC1_dds = 2;
    const DWORD TXIDX_SR71R_100_VC2_dds = 3;
    constexpr auto MESH_NAME = "SR71r2D";

    const UINT MainPanel_id = 0;
    const UINT pnlNavKillrot_id = 1;
    const UINT pnlNavHorzLvl_id = 2;
    const UINT pnlNavPrograde_id = 3;
    const UINT pnlNavRetro_id = 4;
    const UINT pnlNavNorm_id = 5;
    const UINT pnlNavAntiNorm_id = 6;
    const UINT pnlRCSRot_id = 7;
    const UINT pnlRCSLin_id = 8;
    const UINT pnlHUDDock_id = 9;
    const UINT pnlHUDSurf_id = 10;
    const UINT pnlHUDOrbit_id = 11;
    const UINT pnlMFDLeft_id = 12;
    const UINT pnlMFDRight_id = 13;
    const UINT pnlClockElapsedReset_id = 14;
    const UINT pnlClockTimerReset_id = 15;
    const UINT pnlAirBrake_id = 16;
    const UINT pnlAirBrakeIncrease_id = 17;
    const UINT pnlAirBrakeDecrease_id = 18;
    const UINT pnlAPUSwitch_id = 19;
    const UINT pnlScreenSwitch_id = 20;
    const UINT pnlLightNav_id = 21;
    const UINT pnlLightBeacon_id = 22;
    const UINT pnlLightStrobe_id = 23;
    const UINT pnlLightDock_id = 24;
    const UINT pnlDoorCanopy_id = 25;
    const UINT pnlDoorCargo_id = 26;
    const UINT pnlDoorHover_id = 27;
    const UINT pnlDoorRetro_id = 28;
    const UINT pnlPwrMain_id = 29;
    const UINT pnlPwrAvion_id = 30;
    const UINT pnlPwrCanopy_id = 31;
    const UINT pnlPwrCargo_id = 32;
    const UINT pnlPwrExtBus_id = 33;
    const UINT pnlPwrFCBus_id = 34;
    const UINT pnlPwrFC_id = 35;
    const UINT pnlLgtExtPwrAvail_id = 36;
    const UINT pnlLgtFCPwrAvail_id = 37;
    const UINT pnlLgtFCPwrOn_id = 38;
    const UINT pnlLgtExtPwrOn_id = 39;
    const UINT pnlAPMain_id = 40;
    const UINT pnlAPHeading_id = 41;
    const UINT pnlAPAltitude_id = 42;
    const UINT pnlAPKEAS_id = 43;
    const UINT pnlAPMACH_id = 44;
    const UINT pnlLH2Switch_id = 45;
    const UINT pnlLH2Avail_id = 46;
    const UINT pnlO2Switch_id = 47;
    const UINT pnlO2Avail_id = 48;
    const UINT pnlLandingGear_id = 49;
    const UINT pnlClockTimerMinute_id = 50;
    const UINT pnlClockHour_id = 51;
    const UINT pnlClockMinute_id = 52;
    const UINT pnlClockSecond_id = 53;
    const UINT pnlHydPress_id = 54;
    const UINT pnlLOXPress_id = 55;
    const UINT pnlLH2Press_id = 56;
    const UINT pnlAmpMeter_id = 57;
    const UINT pnlVoltMeter_id = 58;
    const double MainPanel_Width = 2560.0;
    const double MainPanel_Height = 1024.0;
    constexpr RECT pnlNavKillrot_RC = {847, 617, 917, 673};
    constexpr RECT pnlNavHorzLvl_RC = {847, 674, 917, 730};
    constexpr RECT pnlNavPrograde_RC = {919, 617, 989, 673};
    constexpr RECT pnlNavRetro_RC = {919, 674, 989, 730};
    constexpr RECT pnlNavNorm_RC = {991, 617, 1061, 673};
    constexpr RECT pnlNavAntiNorm_RC = {991, 674, 1061, 730};
    constexpr RECT pnlRCSRot_RC = {124, 25, 194, 81};
    constexpr RECT pnlRCSLin_RC = {44, 25, 114, 81};
    constexpr RECT pnlHUDDock_RC = {236, 28, 306, 84};
    constexpr RECT pnlHUDSurf_RC = {317, 32, 387, 88};
    constexpr RECT pnlHUDOrbit_RC = {410, 27, 480, 83};
    const double pnlMFDLeft_Width = 408.0;
    const double pnlMFDLeft_Height = 408.0000305175781;
    constexpr VECTOR3 pnlMFDRowTwoTop_location =     {596.2580, 584.7822, -1.0000};
    const double pnlMFDRight_Width = 408.0;
    const double pnlMFDRight_Height = 408.0000305175781;
    constexpr VECTOR3 pnlClockSecond_location =     {746.1749, 734.0087, -0.0039};
    constexpr RECT pnlClockElapsedReset_RC = {674, 780, 700, 806};
    constexpr RECT pnlClockTimerReset_RC = {799, 780, 816, 797};
    constexpr RECT pnlAirBrakeIncrease_RC = {18, 852, 78, 973};
    constexpr RECT pnlAirBrakeDecrease_RC = {17, 738, 78, 859};
    constexpr VECTOR3 pnlSpeedBrakeOff_location =     {47.6717, 748.9994, -0.0003};
    constexpr VECTOR3 pnlSpeedBrakeFull_location =     {47.6717, 959.5127, -0.0003};
    constexpr VECTOR3 pnlHydPress_location =     {100.6448, 208.8864, -0.0040};
    constexpr VECTOR3 pnlLOXPress_location =     {100.5536, 383.7163, -0.0040};
    constexpr VECTOR3 pnlLH2Press_location =     {100.6343, 555.8380, -0.0040};
    constexpr RECT pnlAPUSwitch_RC = {292, 907, 325, 995};
    constexpr RECT pnlScreenSwitch_RC = {376, 907, 409, 995};
    constexpr RECT pnlLightNav_RC = {778, 907, 810, 995};
    constexpr RECT pnlLightBeacon_RC = {858, 907, 890, 995};
    constexpr RECT pnlLightStrobe_RC = {938, 907, 970, 995};
    constexpr RECT pnlLightDock_RC = {1018, 907, 1051, 995};
    constexpr RECT pnlDoorCanopy_RC = {456, 907, 488, 995};
    constexpr RECT pnlDoorCargo_RC = {536, 907, 568, 995};
    constexpr RECT pnlDoorHover_RC = {616, 907, 648, 995};
    constexpr RECT pnlDoorRetro_RC = {696, 907, 729, 995};
    constexpr RECT pnlPwrMain_RC = {1981, 688, 2013, 776};
    constexpr RECT pnlPwrAvion_RC = {2061, 688, 2094, 776};
    constexpr RECT pnlPwrCanopy_RC = {2141, 688, 2174, 776};
    constexpr RECT pnlPwrCargo_RC = {2221, 688, 2254, 776};
    constexpr RECT pnlPwrExtBus_RC = {2063, 786, 2095, 874};
    constexpr RECT pnlPwrFCBus_RC = {2222, 786, 2255, 874};
    constexpr RECT pnlPwrFC_RC = {2303, 719, 2335, 807};
    constexpr RECT pnlLgtExtPwrAvail_RC = {1976, 807, 2022, 853};
    constexpr RECT pnlLgtFCPwrAvail_RC = {2297, 807, 2342, 853};
    constexpr RECT pnlLgtFCPwrOn_RC = {2167, 807, 2213, 853};
    constexpr RECT pnlLgtExtPwrOn_RC = {2105, 807, 2150, 853};
    constexpr VECTOR3 pnlAmpMeter_location =     {2466.1423, 740.5164, -0.0039};
    constexpr VECTOR3 pnlVoltMeter_location =     {2466.1423, 740.5164, -0.0039};
    constexpr RECT pnlAPMain_RC = {847, 738, 917, 794};
    constexpr RECT pnlAPHeading_RC = {991, 738, 1061, 794};
    constexpr RECT pnlAPAltitude_RC = {847, 795, 917, 851};
    constexpr RECT pnlAPKEAS_RC = {919, 795, 989, 851};
    constexpr RECT pnlAPMACH_RC = {991, 795, 1061, 851};
    constexpr RECT pnlLH2Switch_RC = {1823, 797, 1893, 853};
    constexpr RECT pnlLH2Avail_RC = {1838, 742, 1883, 788};
    constexpr RECT pnlO2Switch_RC = {1732, 798, 1802, 854};
    constexpr RECT pnlO2Avail_RC = {1746, 742, 1792, 788};

  }

// Scene SR71rVC

  namespace vc 
  {
    const NTVERTEX AttitudeIndicator_verts[4] = {
    {-0.0381f, 0.7333f, 15.2770f, 0.0000f, 0.0000f, -1.0000f, 0.3711f, 0.1478f},
    {0.0381f, 0.7333f, 15.2770f, 0.0000f, 0.0000f, -1.0000f, 0.5171f, 0.1478f},
    {-0.0381f, 0.6571f, 15.2770f, 0.0000f, 0.0000f, -1.0000f, 0.3711f, 0.2939f},
    {0.0381f, 0.6571f, 15.2770f, 0.0000f, 0.0000f, -1.0000f, 0.5172f, 0.2939f}    };
    const NTVERTEX vcNavAntiNorm_verts[4] = {
    {-0.1718f, 0.5913f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4252f, 0.2263f},
    {-0.1535f, 0.5913f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4606f, 0.2263f},
    {-0.1718f, 0.5766f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4252f, 0.2550f},
    {-0.1535f, 0.5766f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4606f, 0.2550f}    };
    const NTVERTEX vcNavRetro_verts[4] = {
    {-0.1894f, 0.5913f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4252f, 0.2553f},
    {-0.1711f, 0.5913f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4606f, 0.2553f},
    {-0.1894f, 0.5766f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4252f, 0.2837f},
    {-0.1711f, 0.5766f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4606f, 0.2837f}    };
    const NTVERTEX vcNavHorzLvl_verts[4] = {
    {-0.2072f, 0.5913f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4250f, 0.2838f},
    {-0.1889f, 0.5913f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4603f, 0.2838f},
    {-0.2072f, 0.5766f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4250f, 0.3126f},
    {-0.1889f, 0.5766f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4603f, 0.3126f}    };
    const NTVERTEX vcNavNorm_verts[4] = {
    {-0.1718f, 0.6062f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3473f, 0.2264f},
    {-0.1535f, 0.6062f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3826f, 0.2264f},
    {-0.1718f, 0.5915f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3473f, 0.2550f},
    {-0.1535f, 0.5915f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3826f, 0.2550f}    };
    const NTVERTEX vcNavProGrade_verts[4] = {
    {-0.1895f, 0.6062f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3471f, 0.2551f},
    {-0.1712f, 0.6062f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3824f, 0.2551f},
    {-0.1895f, 0.5915f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3471f, 0.2838f},
    {-0.1712f, 0.5915f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3824f, 0.2838f}    };
    const NTVERTEX SwAPMACH_verts[4] = {
    {0.3025f, 0.5355f, 15.2435f, -0.3487f, 0.0000f, -0.9372f, 0.3472f, 0.2262f},
    {0.3196f, 0.5355f, 15.2371f, -0.3487f, 0.0000f, -0.9372f, 0.3823f, 0.2262f},
    {0.3025f, 0.5502f, 15.2435f, -0.3487f, 0.0000f, -0.9372f, 0.3472f, 0.1975f},
    {0.3196f, 0.5502f, 15.2371f, -0.3487f, 0.0000f, -0.9372f, 0.3823f, 0.1975f}    };
    const NTVERTEX SwAPKEAS_verts[4] = {
    {0.2824f, 0.5355f, 15.2513f, -0.3487f, 0.0000f, -0.9372f, 0.4255f, 0.2263f},
    {0.2995f, 0.5355f, 15.2449f, -0.3487f, 0.0000f, -0.9372f, 0.4605f, 0.2263f},
    {0.2824f, 0.5502f, 15.2513f, -0.3487f, 0.0000f, -0.9372f, 0.4255f, 0.1974f},
    {0.2995f, 0.5502f, 15.2449f, -0.3487f, 0.0000f, -0.9372f, 0.4605f, 0.1974f}    };
    const NTVERTEX SwAPAltitude_verts[4] = {
    {0.2621f, 0.5355f, 15.2586f, -0.3487f, 0.0000f, -0.9372f, 0.3473f, 0.1975f},
    {0.2793f, 0.5355f, 15.2522f, -0.3487f, 0.0000f, -0.9372f, 0.3824f, 0.1975f},
    {0.2621f, 0.5502f, 15.2586f, -0.3487f, 0.0000f, -0.9372f, 0.3473f, 0.1689f},
    {0.2793f, 0.5502f, 15.2522f, -0.3487f, 0.0000f, -0.9372f, 0.3824f, 0.1689f}    };
    const NTVERTEX SwAPHeading_verts[4] = {
    {0.2421f, 0.5355f, 15.2658f, -0.3487f, 0.0000f, -0.9372f, 0.4253f, 0.1974f},
    {0.2592f, 0.5355f, 15.2595f, -0.3487f, 0.0000f, -0.9372f, 0.4604f, 0.1974f},
    {0.2421f, 0.5502f, 15.2658f, -0.3487f, 0.0000f, -0.9372f, 0.4253f, 0.1686f},
    {0.2592f, 0.5502f, 15.2595f, -0.3487f, 0.0000f, -0.9372f, 0.4604f, 0.1686f}    };
    const NTVERTEX SwAPMain_verts[4] = {
    {0.2217f, 0.5355f, 15.2733f, -0.3487f, 0.0000f, -0.9372f, 0.4254f, 0.1687f},
    {0.2389f, 0.5355f, 15.2670f, -0.3487f, 0.0000f, -0.9372f, 0.4605f, 0.1687f},
    {0.2217f, 0.5502f, 15.2733f, -0.3487f, 0.0000f, -0.9372f, 0.4254f, 0.1402f},
    {0.2389f, 0.5502f, 15.2670f, -0.3487f, 0.0000f, -0.9372f, 0.4605f, 0.1402f}    };
    const NTVERTEX gaugeVoltMeter_verts[4] = {
    {0.3816f, 0.4614f, 15.2140f, -0.3398f, -0.0000f, -0.9405f, 0.6538f, 0.0253f},
    {0.3580f, 0.4996f, 15.2229f, -0.3398f, -0.0000f, -0.9405f, 0.7386f, 0.0253f},
    {0.3726f, 0.4551f, 15.2172f, -0.3398f, -0.0000f, -0.9405f, 0.6538f, 0.0041f},
    {0.3490f, 0.4933f, 15.2261f, -0.3398f, -0.0000f, -0.9405f, 0.7386f, 0.0041f}    };
    const NTVERTEX FuelCellAvailableLight_verts[4] = {
    {0.3247f, 0.4609f, 15.2344f, -0.3428f, 0.0000f, -0.9394f, 0.5492f, 0.0273f},
    {0.3367f, 0.4609f, 15.2300f, -0.3428f, 0.0000f, -0.9394f, 0.5749f, 0.0273f},
    {0.3247f, 0.4736f, 15.2344f, -0.3428f, 0.0000f, -0.9394f, 0.5492f, 0.0016f},
    {0.3367f, 0.4736f, 15.2300f, -0.3428f, 0.0000f, -0.9394f, 0.5749f, 0.0016f}    };
    const NTVERTEX ExtAvailableLight_verts[4] = {
    {0.2242f, 0.4609f, 15.2721f, -0.3428f, 0.0000f, -0.9394f, 0.5494f, 0.0271f},
    {0.2361f, 0.4609f, 15.2677f, -0.3428f, 0.0000f, -0.9394f, 0.5751f, 0.0271f},
    {0.2242f, 0.4736f, 15.2721f, -0.3428f, 0.0000f, -0.9394f, 0.5494f, 0.0015f},
    {0.2361f, 0.4736f, 15.2677f, -0.3428f, 0.0000f, -0.9394f, 0.5751f, 0.0015f}    };
    const NTVERTEX FuelCellConnectedLight_verts[4] = {
    {0.2845f, 0.4609f, 15.2495f, -0.3428f, 0.0000f, -0.9394f, 0.5004f, 0.0271f},
    {0.2964f, 0.4609f, 15.2452f, -0.3428f, 0.0000f, -0.9394f, 0.5260f, 0.0271f},
    {0.2845f, 0.4736f, 15.2495f, -0.3428f, 0.0000f, -0.9394f, 0.5004f, 0.0015f},
    {0.2964f, 0.4736f, 15.2452f, -0.3428f, 0.0000f, -0.9394f, 0.5260f, 0.0015f}    };
    const NTVERTEX ExtConnectedLight_verts[4] = {
    {0.2643f, 0.4609f, 15.2575f, -0.3428f, 0.0000f, -0.9394f, 0.5004f, 0.0273f},
    {0.2763f, 0.4609f, 15.2531f, -0.3428f, 0.0000f, -0.9394f, 0.5261f, 0.0273f},
    {0.2643f, 0.4736f, 15.2575f, -0.3428f, 0.0000f, -0.9394f, 0.5004f, 0.0017f},
    {0.2763f, 0.4736f, 15.2531f, -0.3428f, 0.0000f, -0.9394f, 0.5261f, 0.0017f}    };
    const NTVERTEX LH2SupplyOnLight_verts[4] = {
    {0.3653f, 0.4101f, 15.2212f, -0.3428f, 0.0000f, -0.9394f, 0.5494f, 0.0273f},
    {0.3773f, 0.4101f, 15.2168f, -0.3428f, 0.0000f, -0.9394f, 0.5751f, 0.0273f},
    {0.3653f, 0.4228f, 15.2212f, -0.3428f, 0.0000f, -0.9394f, 0.5494f, 0.0017f},
    {0.3773f, 0.4228f, 15.2168f, -0.3428f, 0.0000f, -0.9394f, 0.5751f, 0.0017f}    };
    const NTVERTEX FuelSupplyOnLight_verts[4] = {
    {0.3248f, 0.4101f, 15.2357f, -0.3428f, 0.0000f, -0.9394f, 0.5490f, 0.0273f},
    {0.3367f, 0.4101f, 15.2314f, -0.3428f, 0.0000f, -0.9394f, 0.5746f, 0.0273f},
    {0.3248f, 0.4228f, 15.2357f, -0.3428f, 0.0000f, -0.9394f, 0.5490f, 0.0017f},
    {0.3367f, 0.4228f, 15.2314f, -0.3428f, 0.0000f, -0.9394f, 0.5746f, 0.0017f}    };
    const NTVERTEX LOXSupplyOnLight_verts[4] = {
    {0.3451f, 0.4101f, 15.2285f, -0.3428f, 0.0000f, -0.9394f, 0.5493f, 0.0275f},
    {0.3570f, 0.4101f, 15.2241f, -0.3428f, 0.0000f, -0.9394f, 0.5750f, 0.0275f},
    {0.3451f, 0.4228f, 15.2285f, -0.3428f, 0.0000f, -0.9394f, 0.5493f, 0.0018f},
    {0.3570f, 0.4228f, 15.2241f, -0.3428f, 0.0000f, -0.9394f, 0.5750f, 0.0018f}    };
    const NTVERTEX LH2ValveOpenSwitch_verts[4] = {
    {0.3622f, 0.4226f, 15.2217f, -0.3487f, 0.0000f, -0.9372f, 0.3473f, 0.1560f},
    {0.3794f, 0.4226f, 15.2153f, -0.3487f, 0.0000f, -0.9372f, 0.3824f, 0.1560f},
    {0.3622f, 0.4408f, 15.2217f, -0.3487f, 0.0000f, -0.9372f, 0.3473f, 0.1210f},
    {0.3794f, 0.4408f, 15.2153f, -0.3487f, 0.0000f, -0.9372f, 0.3824f, 0.1210f}    };
    const NTVERTEX LOXValveOpenSwitch_verts[4] = {
    {0.3423f, 0.4226f, 15.2290f, -0.3487f, 0.0000f, -0.9372f, 0.3469f, 0.1169f},
    {0.3594f, 0.4226f, 15.2226f, -0.3487f, 0.0000f, -0.9372f, 0.3819f, 0.1169f},
    {0.3423f, 0.4408f, 15.2290f, -0.3487f, 0.0000f, -0.9372f, 0.3469f, 0.0819f},
    {0.3594f, 0.4408f, 15.2226f, -0.3487f, 0.0000f, -0.9372f, 0.3819f, 0.0819f}    };
    const NTVERTEX FuelValveOpenSwitch_verts[4] = {
    {0.3221f, 0.4226f, 15.2368f, -0.3487f, 0.0000f, -0.9372f, 0.3468f, 0.0780f},
    {0.3392f, 0.4226f, 15.2304f, -0.3487f, 0.0000f, -0.9372f, 0.3819f, 0.0780f},
    {0.3221f, 0.4408f, 15.2368f, -0.3487f, 0.0000f, -0.9372f, 0.3468f, 0.0429f},
    {0.3392f, 0.4408f, 15.2304f, -0.3487f, 0.0000f, -0.9372f, 0.3819f, 0.0429f}    };
    const NTVERTEX FuelTransferSwitch_verts[4] = {
    {0.2215f, 0.4176f, 15.2732f, -0.3487f, 0.0000f, -0.9372f, 0.3468f, 0.0388f},
    {0.2387f, 0.4176f, 15.2668f, -0.3487f, 0.0000f, -0.9372f, 0.3819f, 0.0388f},
    {0.2215f, 0.4358f, 15.2732f, -0.3487f, 0.0000f, -0.9372f, 0.3468f, 0.0038f},
    {0.2387f, 0.4358f, 15.2668f, -0.3487f, 0.0000f, -0.9372f, 0.3819f, 0.0038f}    };
    const NTVERTEX VSIOffFlag_verts[4] = {
    {0.0595f, 0.6162f, 15.2756f, 0.0000f, 0.0000f, -1.0000f, 0.3456f, 0.6081f},
    {0.0722f, 0.6162f, 15.2756f, 0.0000f, 0.0000f, -1.0000f, 0.3694f, 0.6081f},
    {0.0595f, 0.6035f, 15.2756f, 0.0000f, 0.0000f, -1.0000f, 0.3456f, 0.6320f},
    {0.0722f, 0.6035f, 15.2756f, 0.0000f, 0.0000f, -1.0000f, 0.3694f, 0.6320f}    };
    const NTVERTEX AltimeterGround_verts[4] = {
    {0.1017f, 0.6797f, 15.2757f, 0.0000f, 0.0000f, -1.0000f, 0.3947f, 0.6081f},
    {0.1144f, 0.6797f, 15.2757f, 0.0000f, 0.0000f, -1.0000f, 0.4188f, 0.6081f},
    {0.1017f, 0.6670f, 15.2757f, 0.0000f, 0.0000f, -1.0000f, 0.3947f, 0.6321f},
    {0.1144f, 0.6670f, 15.2757f, 0.0000f, 0.0000f, -1.0000f, 0.4188f, 0.6321f}    };
    const NTVERTEX AltimeterOffFlag_verts[4] = {
    {0.0592f, 0.6794f, 15.2756f, 0.0000f, 0.0000f, -1.0000f, 0.3455f, 0.6081f},
    {0.0719f, 0.6794f, 15.2756f, 0.0000f, 0.0000f, -1.0000f, 0.3694f, 0.6081f},
    {0.0592f, 0.6667f, 15.2756f, 0.0000f, 0.0000f, -1.0000f, 0.3455f, 0.6320f},
    {0.0719f, 0.6667f, 15.2756f, 0.0000f, 0.0000f, -1.0000f, 0.3694f, 0.6320f}    };
    const NTVERTEX HudDockOn_verts[4] = {
    {-0.1300f, 0.6407f, 15.2760f, 0.0000f, 0.0000f, -1.0000f, 0.7021f, 0.0009f},
    {-0.1178f, 0.6407f, 15.2760f, 0.0000f, 0.0000f, -1.0000f, 0.7258f, 0.0009f},
    {-0.1300f, 0.6344f, 15.2760f, 0.0000f, 0.0000f, -1.0000f, 0.7021f, 0.0133f},
    {-0.1178f, 0.6344f, 15.2760f, 0.0000f, 0.0000f, -1.0000f, 0.7258f, 0.0133f}    };
    const NTVERTEX HudOrbOn_verts[4] = {
    {-0.1440f, 0.6407f, 15.2760f, 0.0000f, 0.0000f, -1.0000f, 0.7021f, 0.0009f},
    {-0.1318f, 0.6407f, 15.2760f, 0.0000f, 0.0000f, -1.0000f, 0.7258f, 0.0009f},
    {-0.1440f, 0.6344f, 15.2760f, 0.0000f, 0.0000f, -1.0000f, 0.7021f, 0.0133f},
    {-0.1318f, 0.6344f, 15.2760f, 0.0000f, 0.0000f, -1.0000f, 0.7258f, 0.0133f}    };
    const NTVERTEX HudSurfOn_verts[4] = {
    {-0.1579f, 0.6407f, 15.2760f, 0.0000f, 0.0000f, -1.0000f, 0.7021f, 0.0009f},
    {-0.1457f, 0.6407f, 15.2760f, 0.0000f, 0.0000f, -1.0000f, 0.7258f, 0.0009f},
    {-0.1579f, 0.6344f, 15.2760f, 0.0000f, 0.0000f, -1.0000f, 0.7021f, 0.0133f},
    {-0.1457f, 0.6344f, 15.2760f, 0.0000f, 0.0000f, -1.0000f, 0.7258f, 0.0133f}    };
    const NTVERTEX MsgLightSpeedBrake_verts[4] = {
    {0.0005f, 0.7462f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6034f, 0.6899f},
    {0.0183f, 0.7462f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6376f, 0.6899f},
    {0.0005f, 0.7406f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6034f, 0.7005f},
    {0.0183f, 0.7406f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6376f, 0.7005f}    };
    const NTVERTEX MsgLightAPU_verts[4] = {
    {0.0005f, 0.7527f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.4954f, 0.6898f},
    {0.0183f, 0.7527f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5296f, 0.6898f},
    {0.0005f, 0.7471f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.4954f, 0.7004f},
    {0.0183f, 0.7471f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5296f, 0.7004f}    };
    const NTVERTEX GlideSlopeNeedle_verts[4] = {
    {-0.0442f, 0.6010f, 15.2767f, 0.0000f, 0.0000f, -1.0000f, 0.9642f, 0.0161f},
    {-0.0366f, 0.6010f, 15.2767f, 0.0000f, 0.0000f, -1.0000f, 0.9799f, 0.0161f},
    {-0.0442f, 0.6086f, 15.2767f, 0.0000f, 0.0000f, -1.0000f, 0.9642f, 0.0004f},
    {-0.0366f, 0.6086f, 15.2767f, 0.0000f, 0.0000f, -1.0000f, 0.9799f, 0.0004f}    };
    const NTVERTEX MsgLightC2_verts[4] = {
    {-0.0182f, 0.7462f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5324f, 0.6900f},
    {-0.0004f, 0.7462f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5666f, 0.6900f},
    {-0.0182f, 0.7406f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5324f, 0.7006f},
    {-0.0004f, 0.7406f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5666f, 0.7006f}    };
    const NTVERTEX MsgLightRetro_verts[4] = {
    {-0.0370f, 0.7462f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.4956f, 0.6898f},
    {-0.0192f, 0.7462f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5297f, 0.6898f},
    {-0.0370f, 0.7406f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.4956f, 0.7004f},
    {-0.0192f, 0.7406f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5297f, 0.7005f}    };
    const NTVERTEX MsgLightThrustLimit_verts[4] = {
    {0.0191f, 0.7462f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6038f, 0.6901f},
    {0.0368f, 0.7462f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6379f, 0.6901f},
    {0.0191f, 0.7406f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6038f, 0.7007f},
    {0.0368f, 0.7406f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6379f, 0.7007f}    };
    const NTVERTEX MsgLightBattery_verts[4] = {
    {0.0191f, 0.7527f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.4958f, 0.6899f},
    {0.0368f, 0.7527f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5299f, 0.6899f},
    {0.0191f, 0.7471f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.4958f, 0.7005f},
    {0.0368f, 0.7471f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5299f, 0.7005f}    };
    const NTVERTEX MsgLightDock_verts[4] = {
    {-0.0182f, 0.7527f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.4958f, 0.6900f},
    {-0.0004f, 0.7527f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5299f, 0.6900f},
    {-0.0182f, 0.7471f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.4957f, 0.7006f},
    {-0.0004f, 0.7471f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5299f, 0.7006f}    };
    const NTVERTEX MsgLightCanopy_verts[4] = {
    {-0.0369f, 0.7527f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.4961f, 0.6898f},
    {-0.0192f, 0.7527f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5302f, 0.6899f},
    {-0.0370f, 0.7471f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.4961f, 0.7005f},
    {-0.0192f, 0.7471f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5302f, 0.7005f}    };
    const NTVERTEX MsgLightFuelWarn_verts[4] = {
    {0.0191f, 0.7591f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6038f, 0.6900f},
    {0.0368f, 0.7591f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6379f, 0.6900f},
    {0.0191f, 0.7535f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6038f, 0.7006f},
    {0.0368f, 0.7535f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6379f, 0.7007f}    };
    const NTVERTEX MsgLightKeasWarn_verts[4] = {
    {0.0004f, 0.7591f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6037f, 0.6899f},
    {0.0182f, 0.7591f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6379f, 0.6899f},
    {0.0004f, 0.7535f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6037f, 0.7005f},
    {0.0182f, 0.7535f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.6379f, 0.7005f}    };
    const NTVERTEX MsgLightBay_verts[4] = {
    {-0.0183f, 0.7591f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.4954f, 0.6900f},
    {-0.0005f, 0.7591f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5296f, 0.6900f},
    {-0.0183f, 0.7535f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.4954f, 0.7006f},
    {-0.0005f, 0.7535f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5296f, 0.7006f}    };
    const NTVERTEX MsgLightHover_verts[4] = {
    {-0.0369f, 0.7591f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.4952f, 0.6899f},
    {-0.0192f, 0.7591f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5293f, 0.6899f},
    {-0.0370f, 0.7535f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.4952f, 0.7005f},
    {-0.0192f, 0.7535f, 15.2629f, 0.0000f, 0.0000f, -1.0000f, 0.5293f, 0.7005f}    };
    const NTVERTEX vcNavKillRot_verts[4] = {
    {-0.2072f, 0.6062f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3472f, 0.2837f},
    {-0.1889f, 0.6062f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3826f, 0.2837f},
    {-0.2072f, 0.5915f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3472f, 0.3129f},
    {-0.1889f, 0.5915f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3826f, 0.3129f}    };
    const NTVERTEX vcRCSRot_verts[4] = {
    {-0.1782f, 0.6232f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3471f, 0.3126f},
    {-0.1599f, 0.6232f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3825f, 0.3126f},
    {-0.1782f, 0.6086f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3471f, 0.3415f},
    {-0.1599f, 0.6086f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3825f, 0.3415f}    };
    const NTVERTEX vcRCSLin_verts[4] = {
    {-0.1782f, 0.6383f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.4253f, 0.3126f},
    {-0.1599f, 0.6383f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.4607f, 0.3126f},
    {-0.1782f, 0.6237f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.4253f, 0.3416f},
    {-0.1599f, 0.6237f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.4607f, 0.3416f}    };
    const NTVERTEX vcHUDDock_verts[4] = {
    {-0.1399f, 0.6713f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3470f, 0.3414f},
    {-0.1216f, 0.6713f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3824f, 0.3414f},
    {-0.1399f, 0.6567f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3470f, 0.3705f},
    {-0.1216f, 0.6567f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.3824f, 0.3705f}    };
    const NTVERTEX vcHUDSURF_verts[4] = {
    {-0.1399f, 0.6561f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.4253f, 0.3414f},
    {-0.1216f, 0.6561f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.4607f, 0.3414f},
    {-0.1399f, 0.6414f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.4253f, 0.3705f},
    {-0.1216f, 0.6414f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.4607f, 0.3705f}    };
    const NTVERTEX vcHUDOrbit_verts[4] = {
    {-0.1588f, 0.6561f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.5032f, 0.3126f},
    {-0.1405f, 0.6561f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.5386f, 0.3126f},
    {-0.1588f, 0.6414f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.5032f, 0.3417f},
    {-0.1405f, 0.6414f, 15.2752f, 0.0000f, 0.0000f, -1.0000f, 0.5386f, 0.3417f}    };
    const NTVERTEX ForwardWindowInsideLeft_verts[3] = {
    {-0.3303f, 0.5991f, 15.2599f, 0.6322f, -0.6280f, -0.4539f, 0.8423f, 0.0694f},
    {-0.0241f, 0.8939f, 15.2785f, 0.6322f, -0.6280f, -0.4539f, 0.8719f, 0.0692f},
    {-0.0297f, 0.6076f, 15.6669f, 0.6322f, -0.6280f, -0.4539f, 0.8505f, 0.1224f}    };
    const NTVERTEX WindowSFI_verts[3] = {
    {0.3303f, 0.5991f, 15.2599f, -0.6322f, -0.6280f, -0.4539f, 0.8423f, 0.0694f},
    {0.0241f, 0.8939f, 15.2785f, -0.6322f, -0.6280f, -0.4539f, 0.8719f, 0.0692f},
    {0.0297f, 0.6076f, 15.6669f, -0.6322f, -0.6280f, -0.4539f, 0.8505f, 0.1224f}    };
    const NTVERTEX gaFuelFlow_verts[4] = {
    {0.1488f, 0.6506f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4257f, 0.5393f},
    {0.1559f, 0.6434f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4445f, 0.5393f},
    {0.1254f, 0.6273f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4257f, 0.6021f},
    {0.1326f, 0.6201f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4445f, 0.6021f}    };
    const NTVERTEX gaMainFuel_verts[4] = {
    {0.1306f, 0.5730f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4256f, 0.5393f},
    {0.1243f, 0.5810f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4446f, 0.5393f},
    {0.1566f, 0.5934f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4256f, 0.6022f},
    {0.1503f, 0.6014f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.4446f, 0.6022f}    };
    const NTVERTEX gaRCSFuel_verts[4] = {
    {0.1746f, 0.5723f, 15.2757f, 0.0000f, 0.0000f, -1.0000f, 0.4256f, 0.5393f},
    {0.1678f, 0.5799f, 15.2757f, 0.0000f, 0.0000f, -1.0000f, 0.4445f, 0.5393f},
    {0.1992f, 0.5943f, 15.2757f, 0.0000f, 0.0000f, -1.0000f, 0.4256f, 0.6021f},
    {0.1924f, 0.6019f, 15.2757f, 0.0000f, 0.0000f, -1.0000f, 0.4445f, 0.6021f}    };
    const NTVERTEX gaugeAmpMeter_verts[4] = {
    {0.3578f, 0.4552f, 15.2224f, -0.3398f, 0.0000f, -0.9405f, 0.6538f, 0.0253f},
    {0.3816f, 0.4932f, 15.2137f, -0.3398f, 0.0000f, -0.9405f, 0.7386f, 0.0253f},
    {0.3488f, 0.4615f, 15.2256f, -0.3398f, 0.0000f, -0.9405f, 0.6538f, 0.0041f},
    {0.3726f, 0.4995f, 15.2169f, -0.3398f, 0.0000f, -0.9405f, 0.7386f, 0.0041f}    };
    const NTVERTEX HSICourseNeedle_verts[4] = {
    {-0.0203f, 0.5766f, 15.2757f, 0.0000f, 0.0000f, -1.0000f, 0.5432f, 0.4708f},
    {0.0203f, 0.5766f, 15.2757f, 0.0000f, 0.0000f, -1.0000f, 0.6200f, 0.4708f},
    {-0.0203f, 0.6172f, 15.2757f, 0.0000f, 0.0000f, -1.0000f, 0.5432f, 0.3951f},
    {0.0203f, 0.6172f, 15.2757f, 0.0000f, 0.0000f, -1.0000f, 0.6200f, 0.3951f}    };
    const NTVERTEX HSIOffFlag_verts[4] = {
    {-0.0471f, 0.6283f, 15.2750f, 0.0000f, 0.0000f, -1.0000f, 0.3448f, 0.6333f},
    {-0.0344f, 0.6283f, 15.2750f, 0.0000f, 0.0000f, -1.0000f, 0.3692f, 0.6333f},
    {-0.0471f, 0.6156f, 15.2750f, 0.0000f, 0.0000f, -1.0000f, 0.3448f, 0.6576f},
    {-0.0344f, 0.6156f, 15.2750f, 0.0000f, 0.0000f, -1.0000f, 0.3692f, 0.6576f}    };
    const NTVERTEX SpeedVelocityFlag_verts[4] = {
    {-0.0730f, 0.6775f, 15.2751f, 0.0000f, 0.0000f, -1.0000f, 0.3950f, 0.6083f},
    {-0.0603f, 0.6775f, 15.2751f, 0.0000f, 0.0000f, -1.0000f, 0.4187f, 0.6083f},
    {-0.0730f, 0.6648f, 15.2751f, 0.0000f, 0.0000f, -1.0000f, 0.3950f, 0.6320f},
    {-0.0603f, 0.6648f, 15.2751f, 0.0000f, 0.0000f, -1.0000f, 0.4187f, 0.6320f}    };
    const NTVERTEX AttitudeFlagOff_verts[4] = {
    {-0.0362f, 0.7173f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.3455f, 0.6081f},
    {-0.0235f, 0.7173f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.3694f, 0.6081f},
    {-0.0362f, 0.7046f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.3455f, 0.6320f},
    {-0.0235f, 0.7046f, 15.2755f, 0.0000f, 0.0000f, -1.0000f, 0.3694f, 0.6320f}    };
    const NTVERTEX COMStatusPanel_verts[4] = {
    {0.0431f, 0.7210f, 15.2759f, 0.0000f, 0.0000f, -1.0000f, 0.5261f, 0.0268f},
    {0.0304f, 0.7210f, 15.2759f, 0.0000f, 0.0000f, -1.0000f, 0.5014f, 0.0268f},
    {0.0431f, 0.7337f, 15.2759f, 0.0000f, 0.0000f, -1.0000f, 0.5261f, 0.0021f},
    {0.0304f, 0.7337f, 15.2759f, 0.0000f, 0.0000f, -1.0000f, 0.5014f, 0.0021f}    };
    const NTVERTEX HSIExoFlag_verts[4] = {
    {-0.0474f, 0.6284f, 15.2749f, 0.0000f, 0.0000f, -1.0000f, 0.3939f, 0.6330f},
    {-0.0347f, 0.6284f, 15.2749f, 0.0000f, 0.0000f, -1.0000f, 0.4183f, 0.6330f},
    {-0.0474f, 0.6157f, 15.2749f, 0.0000f, 0.0000f, -1.0000f, 0.3939f, 0.6574f},
    {-0.0347f, 0.6157f, 15.2749f, 0.0000f, 0.0000f, -1.0000f, 0.4183f, 0.6574f}    };
    const NTVERTEX SpeedFlagOff_verts[4] = {
    {-0.1194f, 0.6702f, 15.2751f, 0.0000f, 0.0000f, -1.0000f, 0.3457f, 0.6083f},
    {-0.1071f, 0.6733f, 15.2751f, 0.0000f, 0.0000f, -1.0000f, 0.3694f, 0.6083f},
    {-0.1163f, 0.6579f, 15.2751f, 0.0000f, 0.0000f, -1.0000f, 0.3457f, 0.6319f},
    {-0.1040f, 0.6610f, 15.2751f, 0.0000f, 0.0000f, -1.0000f, 0.3694f, 0.6319f}    };
    const NTVERTEX CanopyWindowInsideLeft_verts[6] = {
    {-0.4317f, 0.7433f, 14.4864f, 0.7922f, -0.5886f, -0.1614f, 0.8405f, 0.1251f},
    {-0.2610f, 0.9771f, 14.5001f, 0.7922f, -0.5886f, -0.1614f, 0.8404f, 0.0677f},
    {-0.3712f, 0.6164f, 15.1668f, 0.7074f, -0.6178f, -0.3433f, 0.9667f, 0.1259f},
    {-0.0599f, 0.9239f, 15.2037f, 0.7074f, -0.6178f, -0.3433f, 0.9667f, 0.0675f},
    {-0.1872f, 0.9526f, 14.9490f, 0.7518f, -0.6070f, -0.2577f, 0.9147f, 0.0684f},
    {-0.4043f, 0.6517f, 14.9686f, 0.7500f, -0.6076f, -0.2614f, 0.9192f, 0.1256f}    };
    const NTVERTEX CanopyWindowSI_verts[6] = {
    {0.4317f, 0.7433f, 14.4864f, -0.7921f, -0.5887f, -0.1614f, 0.8405f, 0.1251f},
    {0.2610f, 0.9772f, 14.5001f, -0.7921f, -0.5887f, -0.1614f, 0.8404f, 0.0677f},
    {0.3712f, 0.6164f, 15.1668f, -0.7076f, -0.6179f, -0.3428f, 0.9667f, 0.1259f},
    {0.0597f, 0.9242f, 15.2044f, -0.7076f, -0.6179f, -0.3428f, 0.9667f, 0.0675f},
    {0.1871f, 0.9526f, 14.9490f, -0.7518f, -0.6070f, -0.2574f, 0.9147f, 0.0684f},
    {0.4044f, 0.6517f, 14.9687f, -0.7501f, -0.6076f, -0.2611f, 0.9192f, 0.1256f}    };
    const DWORD TXIDX_SR71_VCCockPit_dds = 1;
    const DWORD TXIDX_SR71R_100_VC1_dds = 2;
    const DWORD TXIDX_SR71R_100_VC2_dds = 3;
    const DWORD TXIDX_SR71rBody_dds = 4;
    constexpr auto MESH_NAME = "SR71rVC";

    const UINT CockpitPanelHood_id = 0;
    const UINT SwNavSelect_id = 1;
    const UINT SwExoSelect_id = 2;
    const UINT swRetroDoors_id = 3;
    const UINT swHoverDoor_id = 4;
    const UINT swShutter_id = 5;
    const UINT SwitchDockLights_id = 6;
    const UINT SwitchStrobeLights_id = 7;
    const UINT SwitchBeaconLights_id = 8;
    const UINT SwitchNavLights_id = 9;
    const UINT swDumpFuel_id = 10;
    const UINT swTransferSelect_id = 11;
    const UINT AirBrakeLever_id = 12;
    const UINT swFuelCellPower_id = 13;
    const UINT SwAPUPower_id = 14;
    const UINT SwCargoOpen_id = 15;
    const UINT SwCanopyOpen_id = 16;
    const UINT SwCargoPower_id = 17;
    const UINT swConnectFuelCell_id = 18;
    const UINT swConnectExternalPower_id = 19;
    const UINT swThrottleLimit_id = 20;
    const UINT SwCanopyPower_id = 21;
    const UINT SwAvionics_id = 22;
    const UINT swMainPower_id = 23;
    const UINT MessagesBae_id = 24;
    const UINT CockPitPanel_id = 25;
    const UINT VCMfdRight_id = 26;
    const UINT VCMfdRightFrame_id = 27;
    const UINT VCMfdLeftFrame_id = 28;
    const UINT LGHandle_id = 29;
    const UINT CanopyFI_id = 30;
    const UINT VCMfdLeft_id = 31;
    const UINT CanopyFO_id = 32;
    const UINT HUDBracketRight_id = 33;
    const UINT HUDBracketLeft_id = 34;
    const UINT ForwardCockpit_id = 35;
    const UINT LeftPanel_id = 36;
    const UINT PowerPanel_id = 37;
    const UINT HUDPanel_id = 38;
    const UINT LeftPanelControls_id = 39;
    const UINT FuelPanel_id = 40;
    const UINT CompassFace_id = 41;
    const UINT AttitudeIndicator_id = 42;
    const UINT CockpitPanelHoodTop_id = 43;
    const UINT vcNavAntiNorm_id = 44;
    const UINT vcNavRetro_id = 45;
    const UINT vcNavHorzLvl_id = 46;
    const UINT vcNavNorm_id = 47;
    const UINT vcNavProGrade_id = 48;
    const UINT AccelNeedle_id = 49;
    const UINT SwAPMACH_id = 50;
    const UINT SwAPKEAS_id = 51;
    const UINT SwAPAltitude_id = 52;
    const UINT SwAPHeading_id = 53;
    const UINT SwAPMain_id = 54;
    const UINT gaugeVoltMeter_id = 55;
    const UINT FuelCellAvailableLight_id = 56;
    const UINT ExtAvailableLight_id = 57;
    const UINT FuelCellConnectedLight_id = 58;
    const UINT ExtConnectedLight_id = 59;
    const UINT LH2SupplyOnLight_id = 60;
    const UINT FuelSupplyOnLight_id = 61;
    const UINT LOXSupplyOnLight_id = 62;
    const UINT LH2ValveOpenSwitch_id = 63;
    const UINT LOXValveOpenSwitch_id = 64;
    const UINT FuelValveOpenSwitch_id = 65;
    const UINT FuelTransferSwitch_id = 66;
    const UINT VSIOffFlag_id = 67;
    const UINT AltimeterGround_id = 68;
    const UINT AltimeterOffFlag_id = 69;
    const UINT TDIMachHund_id = 70;
    const UINT TDIMachTens_id = 71;
    const UINT TDIMachOne_id = 72;
    const UINT TDIAltTenThous_id = 73;
    const UINT TDIAltThous_id = 74;
    const UINT TDIAltHund_id = 75;
    const UINT TDIAltTens_id = 76;
    const UINT TDIAltOnes_id = 77;
    const UINT TDIKeasOnes_id = 78;
    const UINT TDIKeasTens_id = 79;
    const UINT TDIKeasHund_id = 80;
    const UINT MilesOnes_id = 81;
    const UINT MilesTens_id = 82;
    const UINT MilesHundred_id = 83;
    const UINT CrsHundreds_id = 84;
    const UINT CrsTens_id = 85;
    const UINT CrsOnes_id = 86;
    const UINT HudDockOn_id = 87;
    const UINT HudOrbOn_id = 88;
    const UINT HudSurfOn_id = 89;
    const UINT GuidanceComp_id = 90;
    const UINT MsgLightSpeedBrake_id = 91;
    const UINT MsgLightAPU_id = 92;
    const UINT SpeedNeedleMax_id = 93;
    const UINT GlideSlopeNeedle_id = 94;
    const UINT MsgLightC2_id = 95;
    const UINT MsgLightRetro_id = 96;
    const UINT MsgLightThrustLimit_id = 97;
    const UINT MsgLightBattery_id = 98;
    const UINT gaHydrogenLevel_id = 99;
    const UINT gaugeOxygenLevel_id = 100;
    const UINT gaHydPress_id = 101;
    const UINT MsgLightDock_id = 102;
    const UINT MsgLightCanopy_id = 103;
    const UINT MsgLightFuelWarn_id = 104;
    const UINT MsgLightKeasWarn_id = 105;
    const UINT MsgLightBay_id = 106;
    const UINT MsgLightHover_id = 107;
    const UINT TrimNeedle_id = 108;
    const UINT gaVSINeedle_id = 109;
    const UINT vcNavKillRot_id = 110;
    const UINT vcRCSRot_id = 111;
    const UINT vcRCSLin_id = 112;
    const UINT vcHUDDock_id = 113;
    const UINT vcHUDSURF_id = 114;
    const UINT vcHUDOrbit_id = 115;
    const UINT ForwardWindowInsideLeft_id = 116;
    const UINT WindowSFI_id = 117;
    const UINT gaFuelFlow_id = 118;
    const UINT gaMainFuel_id = 119;
    const UINT gaRCSFuel_id = 120;
    const UINT gaugeAmpMeter_id = 121;
    const UINT ClockTimerMinute_id = 122;
    const UINT SpeedIndicatorKies_id = 123;
    const UINT MessagesLabels_id = 124;
    const UINT RoseCompass_id = 125;
    const UINT HSIBearingArrow_id = 126;
    const UINT ClockHour_id = 127;
    const UINT SpeedNeedle_id = 128;
    const UINT ClockMinute_id = 129;
    const UINT HSICourse_id = 130;
    const UINT ClockSecond_id = 131;
    const UINT HSICourseNeedle_id = 132;
    const UINT PanelHorizonFace_id = 133;
    const UINT HSIOffFlag_id = 134;
    const UINT SpeedVelocityFlag_id = 135;
    const UINT AttitudeFlagOff_id = 136;
    const UINT COMStatusPanel_id = 137;
    const UINT HSICompassHeading_id = 138;
    const UINT AOANeedle_id = 139;
    const UINT gaAlt100Needle_id = 140;
    const UINT HSIExoFlag_id = 141;
    const UINT SpeedFlagOff_id = 142;
    const UINT CanopyWindowInsideLeft_id = 143;
    const UINT CanopyWindowSI_id = 144;
    const UINT gaAlt10Needle_id = 145;
    const UINT HUD_id = 146;
    const UINT gaAlt1Needle_id = 147;
    const UINT HUDGlass_id = 148;
    const UINT CompassFaceTop_id = 149;
    const UINT CylinderHeading_id = 150;
    const UINT Cylinder_id = 151;
    constexpr VECTOR3 MFCRightL1_location =     {0.0633, 0.5328, 15.2751};
    constexpr VECTOR3 MFCRightL2_location =     {0.0633, 0.5185, 15.2751};
    constexpr VECTOR3 MFCRightL3_location =     {0.0633, 0.5042, 15.2751};
    constexpr VECTOR3 MFCRightL4_location =     {0.0633, 0.4900, 15.2751};
    constexpr VECTOR3 MFCRightL5_location =     {0.0633, 0.4757, 15.2751};
    constexpr VECTOR3 MFCRightL6_location =     {0.0633, 0.4614, 15.2751};
    constexpr VECTOR3 MFCRightR6_location =     {0.2002, 0.4614, 15.2751};
    constexpr VECTOR3 MFCRightR5_location =     {0.2002, 0.4757, 15.2751};
    constexpr VECTOR3 MFCRightR4_location =     {0.2002, 0.4899, 15.2751};
    constexpr VECTOR3 MFCRightR3_location =     {0.2002, 0.5040, 15.2751};
    constexpr VECTOR3 MFCRightR2_location =     {0.2002, 0.5182, 15.2751};
    constexpr VECTOR3 MFCRightR1_location =     {0.2002, 0.5325, 15.2751};
    constexpr VECTOR3 MFCRightPWR_location =     {0.0874, 0.4328, 15.2751};
    constexpr VECTOR3 MFCRightSEL_location =     {0.1566, 0.4328, 15.2751};
    constexpr VECTOR3 MFCRightMNU_location =     {0.1763, 0.4328, 15.2751};
    constexpr VECTOR3 MFCLeftMNU_location =     {-0.0882, 0.4328, 15.2751};
    constexpr VECTOR3 MFCLeftSEL_location =     {-0.1079, 0.4328, 15.2751};
    constexpr VECTOR3 MFCLeftPWR_location =     {-0.1771, 0.4328, 15.2751};
    constexpr VECTOR3 MFCLeftR1_location =     {-0.0643, 0.5325, 15.2751};
    constexpr VECTOR3 MFCLeftR2_location =     {-0.0643, 0.5182, 15.2751};
    constexpr VECTOR3 MFCLeftR3_location =     {-0.0643, 0.5040, 15.2751};
    constexpr VECTOR3 MFCLeftR4_location =     {-0.0643, 0.4899, 15.2751};
    constexpr VECTOR3 MFCLeftR5_location =     {-0.0643, 0.4757, 15.2751};
    constexpr VECTOR3 MFCLeftR6_location =     {-0.0643, 0.4614, 15.2751};
    constexpr VECTOR3 MFCLeftL6_location =     {-0.2012, 0.4614, 15.2751};
    constexpr VECTOR3 MFCLeftL5_location =     {-0.2012, 0.4757, 15.2751};
    constexpr VECTOR3 MFCLeftL4_location =     {-0.2012, 0.4900, 15.2751};
    constexpr VECTOR3 MFCLeftL3_location =     {-0.2012, 0.5042, 15.2751};
    constexpr VECTOR3 MFCLeftL2_location =     {-0.2012, 0.5185, 15.2751};
    constexpr VECTOR3 MFCLeftL1_location =     {-0.2012, 0.5328, 15.2751};
    constexpr VECTOR3 vcNavAntiNorm_location =     {-0.1626, 0.5844, 15.2755};
    constexpr VECTOR3 vcNavRetro_location =     {-0.1803, 0.5844, 15.2755};
    constexpr VECTOR3 vcNavHorzLvl_location =     {-0.1980, 0.5844, 15.2755};
    constexpr VECTOR3 vcNavNorm_location =     {-0.1627, 0.5993, 15.2752};
    constexpr VECTOR3 vcNavProGrade_location =     {-0.1803, 0.5993, 15.2752};
    constexpr VECTOR3 AccelNeedle_location =     {-0.1379, 0.5817, 15.2753};
    constexpr VECTOR3 SwNavSelect_location =     {0.0372, 0.6533, 15.2708};
    constexpr VECTOR3 SwExoSelect_location =     {-0.0383, 0.6533, 15.2708};
    constexpr VECTOR3 gaFuelFlow_location =     {0.1407, 0.6353, 15.2755};
    constexpr VECTOR3 gaMainFuel_location =     {0.1405, 0.5872, 15.2755};
    constexpr VECTOR3 gaRCSFuel_location =     {0.1835, 0.5871, 15.2757};
    constexpr VECTOR3 gaugeAmpMeter_location =     {0.3652, 0.4774, 15.2196};
    constexpr VECTOR3 SwAPMACH_location =     {0.3111, 0.5429, 15.2403};
    constexpr VECTOR3 SwAPKEAS_location =     {0.2909, 0.5429, 15.2481};
    constexpr VECTOR3 SwAPAltitude_location =     {0.2707, 0.5429, 15.2554};
    constexpr VECTOR3 SwAPHeading_location =     {0.2506, 0.5429, 15.2627};
    constexpr VECTOR3 GCKeyHome_location =     {0.0242, 0.4570, 15.2751};
    constexpr VECTOR3 SwAPMain_location =     {0.2303, 0.5429, 15.2702};
    constexpr VECTOR3 gaugeVoltMeter_location =     {0.3653, 0.4774, 15.2200};
    constexpr VECTOR3 LH2ValveOpenSwitch_location =     {0.3708, 0.4317, 15.2185};
    constexpr VECTOR3 LOXValveOpenSwitch_location =     {0.3509, 0.4317, 15.2258};
    constexpr VECTOR3 FuelValveOpenSwitch_location =     {0.3307, 0.4317, 15.2336};
    constexpr VECTOR3 FuelTransferSwitch_location =     {0.2301, 0.4267, 15.2700};
    constexpr VECTOR3 SpeedAxisBack_location =     {-0.0883, 0.6591, 15.2829};
    constexpr VECTOR3 swRetroDoors_location =     {-0.2927, 0.4209, 15.2482};
    constexpr VECTOR3 swHoverDoor_location =     {-0.3118, 0.4209, 15.2410};
    constexpr VECTOR3 SpeedAxis_location =     {-0.0883, 0.6591, 15.2583};
    constexpr VECTOR3 AOAAxis_location =     {-0.0661, 0.7037, 15.2613};
    constexpr VECTOR3 TrimAxis_location =     {-0.1379, 0.6141, 15.2705};
    constexpr VECTOR3 AccelAxis_location =     {-0.1379, 0.5817, 15.2616};
    constexpr VECTOR3 TDIMachAxis_location =     {-0.0639, 0.5802, 15.2847};
    constexpr VECTOR3 TDIMachHund_location =     {-0.0935, 0.5802, 15.2847};
    constexpr VECTOR3 TDIMachTens_location =     {-0.0886, 0.5802, 15.2847};
    constexpr VECTOR3 TDIMachOne_location =     {-0.0826, 0.5802, 15.2847};
    constexpr VECTOR3 TDIAltAxis_location =     {-0.0637, 0.5992, 15.2847};
    constexpr VECTOR3 TDIAltTenThous_location =     {-0.0983, 0.5992, 15.2847};
    constexpr VECTOR3 TDIAltThous_location =     {-0.0933, 0.5992, 15.2847};
    constexpr VECTOR3 TDIAltHund_location =     {-0.0884, 0.5992, 15.2847};
    constexpr VECTOR3 TDIAltTens_location =     {-0.0834, 0.5992, 15.2847};
    constexpr VECTOR3 TDIAltOnes_location =     {-0.0784, 0.5992, 15.2847};
    constexpr VECTOR3 KEASAxis_location =     {-0.0635, 0.6157, 15.2849};
    constexpr VECTOR3 TDIKeasOnes_location =     {-0.0834, 0.6157, 15.2849};
    constexpr VECTOR3 TDIKeasTens_location =     {-0.0884, 0.6157, 15.2849};
    constexpr VECTOR3 TDIKeasHund_location =     {-0.0934, 0.6157, 15.2849};
    constexpr VECTOR3 MilesOnes_location =     {-0.0259, 0.6274, 15.2857};
    constexpr VECTOR3 MilesTens_location =     {-0.0308, 0.6274, 15.2857};
    constexpr VECTOR3 MilesHundred_location =     {-0.0357, 0.6274, 15.2857};
    constexpr VECTOR3 CrsHundreds_location =     {0.0241, 0.6274, 15.2860};
    constexpr VECTOR3 CrsTens_location =     {0.0294, 0.6274, 15.2860};
    constexpr VECTOR3 CrsOnes_location =     {0.0346, 0.6274, 15.2860};
    constexpr VECTOR3 CrsOdoAxis_location =     {0.0543, 0.6274, 15.2860};
    constexpr VECTOR3 VSIAxis_location =     {0.0864, 0.5973, 15.2427};
    constexpr VECTOR3 AltimeterAxis_location =     {0.0866, 0.6607, 15.2227};
    constexpr VECTOR3 HSIAxis_location =     {0.0000, 0.5969, 15.2618};
    constexpr VECTOR3 SwExoSelectAxis_location =     {-0.0383, 0.6533, 15.2586};
    constexpr VECTOR3 HSIBearingArrow_location =     {0.0000, 0.5969, 15.2763};
    constexpr VECTOR3 SwNavSelectAxis_location =     {0.0371, 0.6533, 15.2581};
    constexpr VECTOR3 HUDSurfTarget_location =     {-0.1515, 0.6405, 15.2763};
    constexpr VECTOR3 HUDOrbitTarget_location =     {-0.1376, 0.6405, 15.2763};
    constexpr VECTOR3 HUDDockTarget_location =     {-0.1237, 0.6405, 15.2763};
    constexpr VECTOR3 GCKeyFunc10_location =     {0.0413, 0.4973, 15.2751};
    constexpr VECTOR3 GCKeyFunc9_location =     {0.0413, 0.5076, 15.2751};
    constexpr VECTOR3 GCKeyFunc8_location =     {0.0413, 0.5175, 15.2751};
    constexpr VECTOR3 GCKeyFunc7_location =     {0.0413, 0.5278, 15.2751};
    constexpr VECTOR3 GCKeyFunc6_location =     {0.0413, 0.5377, 15.2751};
    constexpr VECTOR3 GCKeyFunc5_location =     {-0.0412, 0.4977, 15.2751};
    constexpr VECTOR3 GCKeyFunc4_location =     {-0.0412, 0.5076, 15.2751};
    constexpr VECTOR3 GCKeyFunc3_location =     {-0.0412, 0.5177, 15.2751};
    constexpr VECTOR3 GCKeyFunc2_location =     {-0.0412, 0.5283, 15.2751};
    constexpr VECTOR3 GCKeyFunc1_location =     {-0.0412, 0.5380, 15.2751};
    constexpr VECTOR3 GCKeyPrev_location =     {0.0242, 0.4733, 15.2751};
    constexpr VECTOR3 GCKeyNext_location =     {0.0403, 0.4733, 15.2751};
    constexpr VECTOR3 GCKeyHUD_location =     {0.0403, 0.4570, 15.2751};
    constexpr VECTOR3 GCKeyDecimal_location =     {0.0077, 0.4570, 15.2751};
    constexpr VECTOR3 GCKeyEnter_location =     {0.0244, 0.4409, 15.2751};
    constexpr VECTOR3 GCKeyClear_location =     {0.0408, 0.4410, 15.2751};
    constexpr VECTOR3 GCKey9_location =     {-0.0079, 0.4732, 15.2751};
    constexpr VECTOR3 GCKey6_location =     {-0.0078, 0.4567, 15.2751};
    constexpr VECTOR3 GCKey3_location =     {-0.0078, 0.4410, 15.2751};
    constexpr VECTOR3 GCKey2_location =     {-0.0239, 0.4410, 15.2751};
    constexpr VECTOR3 GCKey5_location =     {-0.0237, 0.4564, 15.2751};
    constexpr VECTOR3 GCKey8_location =     {-0.0236, 0.4732, 15.2751};
    constexpr VECTOR3 GCKeyPlusMinus_location =     {0.0084, 0.4732, 15.2751};
    constexpr VECTOR3 GCKey0_location =     {0.0088, 0.4410, 15.2751};
    constexpr VECTOR3 GCKey1_location =     {-0.0401, 0.4410, 15.2751};
    constexpr VECTOR3 GCKey7_location =     {-0.0404, 0.4733, 15.2751};
    constexpr VECTOR3 GCKey4_location =     {-0.0402, 0.4566, 15.2751};
    constexpr VECTOR3 swShutter_location =     {-0.3690, 0.4206, 15.2200};
    constexpr VECTOR3 ClockTimerReset_location =     {0.0892, 0.6957, 15.2753};
    constexpr VECTOR3 ClockElapsedReset_location =     {0.0585, 0.6946, 15.2753};
    constexpr VECTOR3 ClockAxisBack_location =     {0.0735, 0.7095, 15.2787};
    constexpr VECTOR3 ClockAxisFront_location =     {0.0735, 0.7095, 15.2585};
    constexpr VECTOR3 ClockTimerMinute_location =     {0.0735, 0.7095, 15.2751};
    constexpr VECTOR3 ClockSecond_location =     {0.0735, 0.7095, 15.2741};
    constexpr VECTOR3 ClockMinute_location =     {0.0735, 0.7095, 15.2744};
    constexpr VECTOR3 ClockHour_location =     {0.0735, 0.7095, 15.2747};
    constexpr VECTOR3 RCSLevelAxisFront_location =     {0.1835, 0.5871, 15.2647};
    constexpr VECTOR3 FuelLevelAxisFront_location =     {0.1405, 0.5872, 15.2496};
    constexpr VECTOR3 FuelFlowAxisFront_location =     {0.1407, 0.6353, 15.2656};
    constexpr VECTOR3 LightsRightAxis_location =     {-0.2972, 0.4619, 15.2468};
    constexpr VECTOR3 DoorsRightAxis_location =     {-0.2804, 0.4216, 15.2533};
    constexpr VECTOR3 SwitchDockLights_location =     {-0.3115, 0.4618, 15.2415};
    constexpr VECTOR3 SwitchStrobeLights_location =     {-0.3310, 0.4618, 15.2341};
    constexpr VECTOR3 SwitchBeaconLights_location =     {-0.3498, 0.4618, 15.2271};
    constexpr VECTOR3 SwitchNavLights_location =     {-0.3687, 0.4618, 15.2200};
    constexpr VECTOR3 FuelTransferRightAxis_location =     {0.3145, 0.4261, 15.2372};
    constexpr VECTOR3 swDumpFuel_location =     {0.2907, 0.4261, 15.2487};
    constexpr VECTOR3 swTransferSelect_location =     {0.2504, 0.4261, 15.2635};
    constexpr VECTOR3 SpBrakeAxisRight_location =     {-0.2540, 0.4418, 15.3152};
    constexpr VECTOR3 SpBrakeAxisLeft_location =     {-0.2600, 0.4418, 15.3130};
    constexpr VECTOR3 ABTargetDecrease_location =     {-0.2396, 0.4599, 15.2645};
    constexpr VECTOR3 ABTargetIncrease_location =     {-0.2396, 0.4223, 15.2645};
    constexpr VECTOR3 SpeedIndicatorKies_location =     {-0.0883, 0.6591, 15.2754};
    constexpr VECTOR3 SpeedNeedleMax_location =     {-0.0883, 0.6591, 15.2755};
    constexpr VECTOR3 APBankRightTarget_location =     {0.3828, 0.5111, 15.2102};
    constexpr VECTOR3 APBankLeftTarget_location =     {0.3598, 0.5111, 15.2172};
    constexpr VECTOR3 CourseKnob_location =     {0.0374, 0.5680, 15.2658};
    constexpr VECTOR3 HSICourseNeedle_location =     {0.0000, 0.5969, 15.2757};
    constexpr VECTOR3 HSICourse_location =     {0.0000, 0.5969, 15.2760};
    constexpr VECTOR3 axisOxygenLevel_location =     {-0.3086, 0.5101, 15.2326};
    constexpr VECTOR3 axisHydrogenLevel_location =     {-0.2662, 0.5101, 15.2489};
    constexpr VECTOR3 axisHydPress_location =     {-0.3504, 0.5102, 15.2152};
    constexpr VECTOR3 gaHydrogenLevel_location =     {-0.2689, 0.5101, 15.2562};
    constexpr VECTOR3 gaugeOxygenLevel_location =     {-0.3114, 0.5101, 15.2403};
    constexpr VECTOR3 gaHydPress_location =     {-0.3539, 0.5102, 15.2247};
    constexpr VECTOR3 swFuelCellPower_location =     {0.3310, 0.4971, 15.2340};
    constexpr VECTOR3 SwAPUPower_location =     {-0.2686, 0.5623, 15.2565};
    constexpr VECTOR3 VoltMeterFrontAxis_location =     {0.3604, 0.4774, 15.2064};
    constexpr VECTOR3 ShowVoltsTarget_location =     {0.3494, 0.4560, 15.2251};
    constexpr VECTOR3 LeftPanelTopRightAxis_location =     {-0.2557, 0.5631, 15.2633};
    constexpr VECTOR3 SwCargoOpen_location =     {-0.3305, 0.4209, 15.2341};
    constexpr VECTOR3 SwCanopyOpen_location =     {-0.3498, 0.4196, 15.2272};
    constexpr VECTOR3 SwCargoPower_location =     {0.2910, 0.4971, 15.2487};
    constexpr VECTOR3 PowerBottomRightAxis_location =     {0.3237, 0.4667, 15.2364};
    constexpr VECTOR3 TopRowSwitchRightAxis_location =     {0.2456, 0.5780, 15.2658};
    constexpr VECTOR3 swConnectFuelCell_location =     {0.3114, 0.4667, 15.2429};
    constexpr VECTOR3 swConnectExternalPower_location =     {0.2510, 0.4667, 15.2648};
    constexpr VECTOR3 swThrottleLimit_location =     {0.2301, 0.5780, 15.2704};
    constexpr VECTOR3 SwCanopyPower_location =     {0.2708, 0.4971, 15.2563};
    constexpr VECTOR3 SwAvionics_location =     {0.2505, 0.4971, 15.2636};
    constexpr VECTOR3 PowerTopRightAxis_location =     {0.3544, 0.4971, 15.2248};
    constexpr VECTOR3 swMainPower_location =     {0.2306, 0.4971, 15.2712};
    constexpr VECTOR3 HeadingKnob_location =     {-0.0380, 0.5677, 15.2658};
    constexpr VECTOR3 HSICompassHeading_location =     {0.0000, 0.5969, 15.2754};
    constexpr VECTOR3 GearLeverUpTarget_location =     {-0.2228, 0.5328, 15.2369};
    constexpr VECTOR3 GearLeverDownTarget_location =     {-0.2228, 0.5699, 15.2370};
    constexpr VECTOR3 TrimNeedle_location =     {-0.1379, 0.6141, 15.2753};
    constexpr VECTOR3 LGHandleAxis_location =     {-0.2446, 0.5513, 15.2832};
    constexpr VECTOR3 LGHandle_location =     {-0.2354, 0.5512, 15.2866};
    constexpr VECTOR3 AOANeedle_location =     {-0.0661, 0.7037, 15.2753};
    constexpr VECTOR3 gaVSINeedle_location =     {0.0864, 0.5973, 15.2753};
    constexpr VECTOR3 vcNavKillRot_location =     {-0.1980, 0.5993, 15.2752};
    constexpr VECTOR3 SpeedNeedle_location =     {-0.0883, 0.6591, 15.2752};
    constexpr VECTOR3 gaAlt1Needle_location =     {0.0866, 0.6607, 15.2748};
    constexpr VECTOR3 gaAlt10Needle_location =     {0.0866, 0.6607, 15.2751};
    constexpr VECTOR3 gaAlt100Needle_location =     {0.0866, 0.6607, 15.2755};
    constexpr VECTOR3 RoseCompass_location =     {0.0000, 0.5969, 15.2766};
    constexpr VECTOR3 vcRCSRot_location =     {-0.1691, 0.6163, 15.2752};
    constexpr VECTOR3 vcRCSLin_location =     {-0.1691, 0.6314, 15.2752};
    constexpr VECTOR3 vcHUDDock_location =     {-0.1308, 0.6645, 15.2752};
    constexpr VECTOR3 vcHUDSURF_location =     {-0.1308, 0.6492, 15.2752};
    constexpr VECTOR3 vcHUDOrbit_location =     {-0.1497, 0.6492, 15.2752};
    constexpr VECTOR3 PilotPOVRight_location =     {0.0758, 0.8204, 14.7813};
    constexpr VECTOR3 PilotPOVLeft_location =     {-0.0743, 0.8204, 14.7813};
    constexpr VECTOR3 PilotPOV_location =     {0.0000, 0.8204, 14.7813};

  }

}
#endif
