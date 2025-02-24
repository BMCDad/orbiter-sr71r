#pragma once

#include <concepts>
#include <functional>
#include <memory>

#include "..\bc_orbiter\IVessel.h"
#include "..\bc_orbiter\Animation.h"

#include "Orbitersdk.h"

namespace bco = bc_orbiter;

template <typename T>
concept ToggleContract = requires(T t) {
    { t.VCMeshName }        -> std::convertible_to<const char*>;
    { t.PanelMeshName }     -> std::convertible_to<const char*>;
    { t.VCMeshGroup }       -> std::convertible_to<UINT>;
    { t.VCLocationA }       -> std::convertible_to<const VECTOR3&>;
    { t.VCLocationB }       -> std::convertible_to<const VECTOR3&>;
    { t.VCID }              -> std::convertible_to<int>;
    { t.PanelID }           -> std::convertible_to<int>;
    { t.PanelHitRC }        -> std::convertible_to<const RECT&>;
    { t.PanelGroup }        -> std::convertible_to<UINT>;
    { t.PanelVerts }        -> std::convertible_to<const NTVERTEX*>;
};

struct ToggleData {
    const char*     VCMeshName;
    const char*     PanelMeshName;
    UINT            VCMeshGroup;
    const VECTOR3&  VCLocationA;
    const VECTOR3&  VCLocationB;
    int             VCID;
    int             PanelID;
    const RECT&     PanelHitRC;
    UINT            PanelGroup;
    const NTVERTEX* PanelVerts;
};

template <ToggleContract T>
class ToggleSwitch
{
public:
    explicit ToggleSwitch(const T& data) : data_(data) {}

    void Register(bco::IVessel& vessel);

    void Step(bco::IVessel& vessel, double simdt);

    void RequestRedraw(bco::IVessel& vessel) { vessel.RequestPanelRedraw(data_.PanelID, panelRedrawId_); }

    void SetState(bool state) { isOn_ = state; };
    bool IsOn() const { return isOn_; };

private:

    void onHitEvent(bco::IVessel& vessel) {
        isOn_ = !isOn_;
        vessel.RequestPanelRedraw(data_.PanelID, panelRedrawId_);
    }


    void onPanelRedraw(bco::IVessel&) {
        bco::DrawPanelOffset(meshPanel_, data_.PanelGroup, data_.PanelVerts, textureOffset_ * (isOn_ ? 1.0 : 0.0));
    }

    T data_;
    const double            HitRadius   = 0.01;
    const double            Angle       = 1.5708;

    double                  textureOffset_{ 0.0 };

    UINT panelRedrawId_{ (UINT)-1 };

    int                     animId_{ 0 };
    bco::StateUpdateTarget  animUpdate_;
    bco::StateUpdate        state_{ 10.0, 0.0, 0.0 };
    bool                    isOn_{ false };

    std::unique_ptr<MGROUP_ROTATE>  group_;
    UINT                            meshId_{ 0 };
    MESHHANDLE                      meshPanel_{ nullptr };
};

inline void ToggleSwitch<ToggleData>::Register(bco::IVessel& vessel) 
{
    auto vcMeshIndex = vessel.GetMeshIndex(data_.VCMeshName);
    VECTOR3 axis = data_.VCLocationB - data_.VCLocationA;
    normalise(axis);

    group_ = std::make_unique<MGROUP_ROTATE>(vcMeshIndex, &data_.VCMeshGroup, 1, data_.VCLocationA, axis, Angle);
    animId_ = vessel.AddAnimation(vcMeshIndex, group_.get());

    auto vcEventId = vessel.RegisterForVCEvent(data_.VCID, data_.VCLocationA, HitRadius, [&](bco::IVessel& v) {onHitEvent(v); });
    vessel.RegisterForPanelEvent(data_.PanelID, data_.PanelHitRC, [&](bco::IVessel& v) {onHitEvent(v); });
    panelRedrawId_ = vessel.RegisterForPanelRedraw(data_.PanelID, [&](bco::IVessel& v) {onPanelRedraw(v); });

    meshPanel_ = vessel.GetpanelMeshHandle(data_.PanelID);
    textureOffset_ = bco::UVOffset(data_.PanelVerts);
}

inline void ToggleSwitch<ToggleData>::Step(bco::IVessel& vessel, double simdt) 
{
    state_.target_state_ = isOn_ ? 1.0 : 0.0;
    animUpdate_.UpdateState(state_, simdt);
    vessel.SetAnimationState(animId_, state_.state_);
}

