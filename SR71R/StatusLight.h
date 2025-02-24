#pragma once

#include <concepts>

#include "..\bc_orbiter\IVessel.h"
#include "..\bc_orbiter\Tools.h"

#include "OrbiterAPI.h"

namespace bco = bc_orbiter;

template <typename T>
concept StatusContract = requires(T t) {
    { t.VCMeshName }    -> std::convertible_to<const char*>;
    { t.PanelMeshName } -> std::convertible_to<const char*>;
    { t.VCGroupID }     -> std::convertible_to<UINT>;
    { t.VCVerts }       -> std::convertible_to<const NTVERTEX*>;
    { t.VCID }          -> std::convertible_to<int>;
    { t.PanelGroupId }  -> std::convertible_to<UINT>;
    { t.PanelVerts }    -> std::convertible_to<const NTVERTEX*>;
    { t.PanelID }       -> std::convertible_to<int>;
};

struct StatusData {
    const char*     VCMeshName;
    const char*     PanelMeshName;
    UINT            VCGroupID;
    const NTVERTEX* VCVerts;
    int             VCID;
    UINT            PanelGroupId;
    const NTVERTEX* PanelVerts;
    int             PanelID;
};

template <StatusContract T>
class StatusLight
{
public:
    StatusLight(const T& data) : data_(data) {}

    auto Register(bco::IVessel& vessel) -> void;

    auto UpdateStatus(bco::IVessel& vessel, double status) -> void {
        if (state_ != status) {
            state_ = status;
            vessel.RequestPanelRedraw(data_.PanelID, panelRedrawId_);
            vessel.RequestVCRedraw(data_.VCID, vcRedrawId_);
        }
    }
private:

    void onPanelRedraw(bco::IVessel& v) {
        bco::DrawPanelOffset(meshPanel_, data_.PanelGroupId, data_.PanelVerts, textureOffset_ * state_);
    }

    void onVCRedraw(bco::IVessel& v) {
        auto vcMesh = v.GetDeviceMesh(0);
        bco::DrawVCOffset(vcMesh, data_.VCGroupID, data_.VCVerts, state_ * textureOffset_);
    }


    T data_;

    UINT            panelRedrawId_{ (UINT)-1};
    UINT            vcRedrawId_{ (UINT)-1};
    double          textureOffset_{ 0.0 };
    double          state_{ 0.0 };
    MESHHANDLE      meshPanel_{ nullptr };
    MESHHANDLE      meshVC_{ nullptr };

};

inline void StatusLight<StatusData>::Register(bco::IVessel& vessel)
{
    panelRedrawId_ = vessel.RegisterForPanelRedraw(data_.PanelID, [&](bco::IVessel& v) {onPanelRedraw(v); });
    vcRedrawId_ = vessel.RegisterForVCRedraw(data_.VCID, [&](bco::IVessel& v) {onVCRedraw(v); });

    meshPanel_ = vessel.GetpanelMeshHandle(data_.PanelID);
//    meshVC_ = vessel.GetMeshHandle(data_.VCMeshName);

    textureOffset_ = bco::UVOffset(data_.PanelVerts);
}

