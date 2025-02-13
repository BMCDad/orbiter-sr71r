// Auto generated code file.  Blender: 4.3.2  Blender Tools: (2, 2, 0)
// Date: Tue Feb 11 17:36:44 2025


#include "orbitersdk.h"

#ifndef __SR71r_H
#define __SR71r_H

namespace bm 
{

// Scene SR71r

  namespace main 
  {
    const NTVERTEX FrontWindowInside_vrt[6] = {
    {0.3303f, 0.5991f, 15.2599f, -0.6322f, -0.6280f, -0.4539f, 0.0f, 0.0f},
    {0.0241f, 0.8939f, 15.2785f, -0.6322f, -0.6280f, -0.4539f, 0.0f, 0.0f},
    {0.0297f, 0.6076f, 15.6669f, -0.6322f, -0.6280f, -0.4539f, 0.0f, 0.0f},
    {-0.3303f, 0.5991f, 15.2599f, 0.6322f, -0.6280f, -0.4539f, 0.0f, 0.0f},
    {-0.0241f, 0.8939f, 15.2785f, 0.6322f, -0.6280f, -0.4539f, 0.0f, 0.0f},
    {-0.0297f, 0.6076f, 15.6669f, 0.6322f, -0.6280f, -0.4539f, 0.0f, 0.0f}    };
    const NTVERTEX CanopyWindowInside_vrt[12] = {
    {-0.4317f, 0.7433f, 14.4864f, 0.7984f, -0.5807f, -0.1592f, 0.0f, 0.0f},
    {-0.2610f, 0.9771f, 14.5001f, 0.7984f, -0.5807f, -0.1592f, 0.0f, 0.0f},
    {-0.3712f, 0.6164f, 15.1668f, 0.7149f, -0.6112f, -0.3397f, 0.0f, 0.0f},
    {-0.0599f, 0.9239f, 15.2037f, 0.7149f, -0.6112f, -0.3397f, 0.0f, 0.0f},
    {-0.1872f, 0.9526f, 14.9490f, 0.7588f, -0.5996f, -0.2543f, 0.0f, 0.0f},
    {-0.4043f, 0.6517f, 14.9686f, 0.7568f, -0.6003f, -0.2586f, 0.0f, 0.0f},
    {0.4317f, 0.7433f, 14.4864f, -0.7984f, -0.5807f, -0.1592f, 0.0f, 0.0f},
    {0.2610f, 0.9771f, 14.5001f, -0.7984f, -0.5807f, -0.1592f, 0.0f, 0.0f},
    {0.3712f, 0.6164f, 15.1668f, -0.7149f, -0.6112f, -0.3397f, 0.0f, 0.0f},
    {0.0599f, 0.9239f, 15.2037f, -0.7149f, -0.6112f, -0.3397f, 0.0f, 0.0f},
    {0.1872f, 0.9526f, 14.9490f, -0.7588f, -0.5996f, -0.2543f, 0.0f, 0.0f},
    {0.4043f, 0.6517f, 14.9686f, -0.7568f, -0.6003f, -0.2586f, 0.0f, 0.0f}    };
    const DWORD TXIDX_SR71_VCCockPit_dds = 1;
    const DWORD TXIDX_SR71R_200_Body_dds = 2;
    constexpr auto MESH_NAME = "SR71r";

    const UINT CockpitPanelHood_id = 0;
    const UINT HoverDoorSA_id = 1;
    const UINT HoverDoorPA_id = 2;
    const UINT HoverDoorSF_id = 3;
    const UINT HoverDoorPF_id = 4;
    const UINT ElevonSIT_id = 5;
    const UINT ElevonPIT_id = 6;
    const UINT Body_id = 7;
    const UINT LGDoorSI_id = 8;
    const UINT LGDoorPI_id = 9;
    const UINT LGDoorSO_id = 10;
    const UINT LGDoorPO_id = 11;
    const UINT LGDoorPF_id = 12;
    const UINT LGDoorSF_id = 13;
    const UINT ElevonSIB_id = 14;
    const UINT ElevonSO_id = 15;
    const UINT ElevonPIB_id = 16;
    const UINT ElevonPO_id = 17;
    const UINT OuterWings_id = 18;
    const UINT Engines_id = 19;
    const UINT RudderBase_id = 20;
    const UINT RudderP_id = 21;
    const UINT RudderS_id = 22;
    const UINT EngineExhaust_id = 23;
    const UINT EngineCone_id = 24;
    const UINT BayDoorSA_id = 25;
    const UINT BayDoorSF_id = 26;
    const UINT BayDoorPA_id = 27;
    const UINT BayDoorPF_id = 28;
    const UINT Canopy_id = 29;
    const UINT DockConnector_id = 30;
    const UINT ForwardCockpit_id = 31;
    const UINT CargoBay_id = 32;
    const UINT LGRCross_id = 33;
    const UINT LGRArm_id = 34;
    const UINT LGRFork_id = 35;
    const UINT LGRWheel1_id = 36;
    const UINT LGRWheel3_id = 37;
    const UINT LGRWheel2_id = 38;
    const UINT LGLWheel2_id = 39;
    const UINT LGLWheel1_id = 40;
    const UINT LGLWheel3_id = 41;
    const UINT LGCrossP_id = 42;
    const UINT LGLArm_id = 43;
    const UINT LGLFork_id = 44;
    const UINT LGCrossF_id = 45;
    const UINT LGFrontArm_id = 46;
    const UINT LGFrontAxel_id = 47;
    const UINT LGFrontLeftWheel_id = 48;
    const UINT LGFrontRightWheel_id = 49;
    const UINT CockpitPanelF_id = 50;
    const UINT FrontWindowOutside_id = 51;
    const UINT CockpitPanelHoodTop_id = 52;
    const UINT FrontWindowInside_id = 53;
    const UINT CanopyWindowOutside_id = 54;
    const UINT CanopyWindowInside_id = 55;
    constexpr VECTOR3 AileronAxisPI_loc =     {-5.5504, 0.0862, -8.6488};
    constexpr VECTOR3 AileronAxisPO_loc =     {-8.5592, -0.0012, -8.3760};
    constexpr VECTOR3 AileronAxisSI_loc =     {5.5504, 0.0862, -8.6488};
    constexpr VECTOR3 AileronAxisSO_loc =     {8.5592, -0.0012, -8.3760};
    constexpr VECTOR3 AirBrakeAxisPBI_loc =     {-0.6781, 0.2195, -10.2013};
    constexpr VECTOR3 AirBrakeAxisPBO_loc =     {-3.4266, 0.1097, -9.4846};
    constexpr VECTOR3 AirBrakeAxisPTI_loc =     {-0.6781, 0.2879, -10.2013};
    constexpr VECTOR3 AirBrakeAxisPTO_loc =     {-3.4266, 0.1338, -9.4846};
    constexpr VECTOR3 AirBrakeAxisSBI_loc =     {0.6782, 0.2195, -10.2013};
    constexpr VECTOR3 AirBrakeAxisSBO_loc =     {3.4266, 0.1097, -9.4846};
    constexpr VECTOR3 AirBrakeAxisSTI_loc =     {0.6782, 0.2879, -10.2013};
    constexpr VECTOR3 AirBrakeAxisSTO_loc =     {3.4266, 0.1338, -9.4846};
    constexpr VECTOR3 Bay1AxisPA_loc =     {-0.8046, 0.5370, 12.2323};
    constexpr VECTOR3 Bay1AxisPF_loc =     {-0.6661, 0.5370, 13.5433};
    constexpr VECTOR3 Bay1AxisSA_loc =     {0.8046, 0.5370, 12.2323};
    constexpr VECTOR3 Bay1AxisSF_loc =     {0.6661, 0.5370, 13.5433};
    constexpr VECTOR3 Bay2AxisPA_loc =     {-0.8797, 0.5366, 5.6548};
    constexpr VECTOR3 Bay2AxisPF_loc =     {-0.8600, 0.5366, 11.4888};
    constexpr VECTOR3 Bay2AxisSA_loc =     {0.8797, 0.5366, 5.6548};
    constexpr VECTOR3 Bay2AxisSF_loc =     {0.8600, 0.5366, 11.4888};
    constexpr VECTOR3 BeaconBottom_loc =     {-0.0042, -0.6031, 2.5301};
    constexpr VECTOR3 BeaconTop_loc =     {-0.0042, 1.1307, 2.5301};
    constexpr VECTOR3 BrakeDragPoint_loc =     {0.0000, 0.0000, -10.1577};
    constexpr VECTOR3 CockpitAxisP_loc =     {-0.2700, 1.0463, 13.8123};
    constexpr VECTOR3 CockpitAxisS_loc =     {0.2700, 1.0463, 13.8123};
    constexpr VECTOR3 DockingPort_loc =     {0.0000, 0.5292, 12.1462};
    constexpr VECTOR3 ThrustHoverP_loc =     {-3.0000, 0.0050, -5.0000};
    constexpr VECTOR3 ThrustHoverS_loc =     {3.0000, 0.0050, -5.0000};
    constexpr VECTOR3 ThrustRetroP_loc =     {-4.3800, 0.0000, 1.9977};
    constexpr VECTOR3 ThrustRetroS_loc =     {4.3800, 0.0000, 1.9977};
    constexpr VECTOR3 TPHood_loc =     {0.0000, 1.0369, 15.1190};
    constexpr VECTOR3 TPNose_loc =     {0.0000, -0.1994, 20.0974};
    constexpr VECTOR3 TPRudder_loc =     {0.0000, 3.4638, -9.5882};
    constexpr VECTOR3 TPTail_loc =     {0.0000, 0.4476, -12.7113};
    constexpr VECTOR3 TPWingP_loc =     {-8.8078, -0.0127, -8.8233};
    constexpr VECTOR3 TPWingS_loc =     {8.8078, -0.0127, -8.8233};
    constexpr VECTOR3 VertAirfoilRef_loc =     {0.0000, 1.9796, -7.8691};
    constexpr VECTOR3 PilotPOV_loc =     {0.0000, 0.8204, 14.7813};
    constexpr VECTOR3 RCS_FL_DOWN_loc =     {-0.4769, -0.3031, 17.4846};
    constexpr VECTOR3 RCS_FL_LEFT_loc =     {-0.4611, 0.0000, 17.5924};
    constexpr VECTOR3 RCS_FL_UP_loc =     {-0.4761, 0.0149, 17.5113};
    constexpr VECTOR3 RCS_FR_DOWN_loc =     {0.4769, -0.3031, 17.4977};
    constexpr VECTOR3 RCS_FR_RIGHT_loc =     {0.4599, 0.0000, 17.5924};
    constexpr VECTOR3 RCS_FR_UP_loc =     {0.4761, 0.0149, 17.5244};
    constexpr VECTOR3 RCS_L_BOTTOM_loc =     {-5.5000, -0.0007, 0.0000};
    constexpr VECTOR3 RCS_L_FORWARD_loc =     {-0.4603, 0.0202, 17.6352};
    constexpr VECTOR3 RCS_L_TOP_loc =     {-5.5000, 0.1094, 0.0000};
    constexpr VECTOR3 RCS_R_BOTTOM_loc =     {5.5000, -0.0007, 0.0000};
    constexpr VECTOR3 RCS_R_FORWARD_loc =     {0.4603, 0.0202, 17.6352};
    constexpr VECTOR3 RCS_R_TOP_loc =     {5.5000, 0.1094, 0.0000};
    constexpr VECTOR3 RCS_RL_DOWN_loc =     {-0.5044, -0.3530, -8.0000};
    constexpr VECTOR3 RCS_RL_LEFT_loc =     {-0.1891, 0.0000, -10.9224};
    constexpr VECTOR3 RCS_RL_UP_loc =     {-0.5044, 0.8613, -8.0000};
    constexpr VECTOR3 RCS_RR_DOWN_loc =     {0.5044, -0.3530, -8.0521};
    constexpr VECTOR3 RCS_RR_RIGHT_loc =     {0.1891, 0.0000, -10.9224};
    constexpr VECTOR3 RCS_RR_UP_loc =     {0.5044, 0.8613, -8.0000};
    constexpr VECTOR3 RudderAxisPB_loc =     {-4.1468, 1.9649, -7.8293};
    constexpr VECTOR3 RudderAxisPT_loc =     {-3.6518, 3.7517, -7.8293};
    constexpr VECTOR3 RudderAxisSB_loc =     {4.1468, 1.9649, -7.8293};
    constexpr VECTOR3 RudderAxisST_loc =     {3.6518, 3.7517, -7.8293};
    constexpr VECTOR3 StrobeLightP_loc =     {-3.6518, 3.4787, -7.8293};
    constexpr VECTOR3 StrobeLightS_loc =     {3.6518, 3.4787, -7.8293};
    constexpr VECTOR3 ThrusterP_loc =     {-4.6234, 0.0000, -9.0000};
    constexpr VECTOR3 ThrusterS_loc =     {4.6234, 0.0000, -9.0000};
    constexpr VECTOR3 ThrustHoverF_loc =     {0.0000, -0.2311, 5.0000};
    constexpr VECTOR3 NavLightP_loc =     {-8.5460, 0.0000, -7.9544};
    constexpr VECTOR3 NavLightS_loc =     {8.5460, 0.0000, -7.9544};
    constexpr VECTOR3 NavLightTail_loc =     {0.0000, 0.4364, -12.7415};
    constexpr VECTOR3 LGDoorAxisPIA_loc =     {-0.1629, -0.5319, -2.6792};
    constexpr VECTOR3 LGDoorAxisPIF_loc =     {-0.1629, -0.5319, -1.8050};
    constexpr VECTOR3 LGDoorAxisPOA_loc =     {-2.8321, -0.0784, -2.6401};
    constexpr VECTOR3 LGDoorAxisPOF_loc =     {-2.8315, -0.0770, -1.8050};
    constexpr VECTOR3 LGDoorAxisSIA_loc =     {0.1629, -0.5319, -2.6792};
    constexpr VECTOR3 LGDoorAxisSIF_loc =     {0.1629, -0.5319, -1.8050};
    constexpr VECTOR3 LGDoorAxisSOA_loc =     {2.8329, -0.0794, -2.6401};
    constexpr VECTOR3 LGDoorAxisSOF_loc =     {2.8329, -0.0812, -1.8050};
    constexpr VECTOR3 LandingTouchF_loc =     {0.0000, -2.0381, 9.9099};
    constexpr VECTOR3 LandingTouchP_loc =     {-2.6492, -2.0381, -2.2199};
    constexpr VECTOR3 LandingTouchS_loc =     {2.6492, -2.0381, -2.2199};
    constexpr VECTOR3 LGCrossAxisPA_loc =     {-2.6575, 0.0463, -2.6266};
    constexpr VECTOR3 LGCrossAxisPF_loc =     {-2.6575, 0.0463, -1.7983};
    constexpr VECTOR3 LGCrossAxisSA_loc =     {2.6575, 0.0463, -2.6266};
    constexpr VECTOR3 LGCrossAxisSF_loc =     {2.6575, 0.0463, -1.7983};
    constexpr VECTOR3 HoverDoorAxisPA_loc =     {-3.5070, -0.0182, -5.5110};
    constexpr VECTOR3 HoverDoorAxisPF_loc =     {-3.5070, -0.0354, -4.5090};
    constexpr VECTOR3 HoverDoorAxisPFA_loc =     {-0.5010, -0.4450, 4.5090};
    constexpr VECTOR3 HoverDoorAxisPFF_loc =     {-0.5010, -0.4450, 5.5110};
    constexpr VECTOR3 HoverDoorAxisSA_loc =     {3.5070, -0.0182, -5.5110};
    constexpr VECTOR3 HoverDoorAxisSF_loc =     {3.5070, -0.0354, -4.5090};
    constexpr VECTOR3 HoverDoorAxisSFA_loc =     {0.5010, -0.4450, 4.5090};
    constexpr VECTOR3 HoverDoorAxisSFF_loc =     {0.5010, -0.4450, 5.5110};
    constexpr VECTOR3 LGFRBackPivot_loc =     {0.3505, -0.4900, 9.7597};
    constexpr VECTOR3 LGFrontRightPivot_loc =     {-0.3107, -0.0655, 9.9842};
    constexpr VECTOR3 LGFrontLeftPivot_loc =     {0.3064, -0.0655, 9.9842};
    constexpr VECTOR3 LGFLFrontPivot_loc =     {-0.3411, -0.4900, 12.0132};
    constexpr VECTOR3 LGFLBackPivot_loc =     {-0.3505, -0.4900, 9.7597};
    constexpr VECTOR3 LGFRFrontPivot_loc =     {0.3411, -0.4900, 12.0132};

  }

}
#endif
