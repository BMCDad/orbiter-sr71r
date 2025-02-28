#pragma once

#include <concepts>
#include <functional>
#include <memory>

#include "..\bc_orbiter\IVessel.h"
#include "..\bc_orbiter\Animation.h"
#include "..\bc_orbiter\Tools.h"
#include "..\bc_orbiter\Types.h"

#include "Orbitersdk.h"



namespace bco = bc_orbiter;
namespace con = bc_orbiter::contracts;
namespace dta = bc_orbiter::data_type;

template <con::Cockpit Tvc, con::RotateControl TvcCtrl, con::Cockpit Tpnl, con::StaticControl TpnlCtrl>
class ToggleSwitch
{
public:
    explicit ToggleSwitch(const Tvc& vct, const TvcCtrl& vcc, const Tpnl& pnlt, TpnlCtrl& pnlc) 
//    explicit ToggleSwitch(dta::Cockpit& vct, dta::RotateControl& vcc, dta::Cockpit& pnlt, dta::StaticControl& pnlc)
      : vcCockpitData_(vct),
        vcControlData_(vcc),
        pnlCockpitData_(pnlt),
        pnlControlData_(pnlc)
    { }

    
    void Register(bco::IVessel& vessel);
    void Step(bco::IVessel& vessel, double simdt);

    void RequestRedraw(bco::IVessel& vessel) { vessel.RequestPanelRedraw(pnlCockpitData_.ID, panelRedrawId_); }

    void SetState(bool state) { isOn_ = state; };
    bool IsOn() const { return isOn_; };

private:

    void onHitEvent(bco::IVessel& vessel) {
        isOn_ = !isOn_;
        vessel.RequestPanelRedraw(pnlCockpitData_.ID, panelRedrawId_);
    }


    void onPanelRedraw(bco::IVessel&) {
        bco::DrawPanelOffset(meshPanel_, pnlControlData_.Group, pnlControlData_.Verts, textureOffset_ * (isOn_ ? 1.0 : 0.0));
    }

    Tvc         vcCockpitData_;
    TvcCtrl     vcControlData_;
    Tpnl        pnlCockpitData_;
    TpnlCtrl    pnlControlData_;

    //dta::Cockpit&       vcCockpitData_;
    //dta::RotateControl& vcControlData_;
    //dta::Cockpit&       pnlCockpitData_;
    //dta::StaticControl& pnlControlData_;


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

inline void ToggleSwitch<dta::Cockpit, dta::RotateControl, dta::Cockpit, dta::StaticControl>::Register(bco::IVessel& vessel)
//inline void ToggleSwitch::Register(bco::IVessel& vessel)
{
    auto vcMeshIndex = vessel.GetMeshIndex(vcCockpitData_.MeshName);
    VECTOR3 axis = vcControlData_.LocationB - vcControlData_.LocationA;
    normalise(axis);

    //group_ = std::make_unique<MGROUP_ROTATE>(
    //    vcMeshIndex, 
    //    &vcControlData_.MeshGroup, 
    //    1, 
    //    vcControlData_.LocationA, 
    //    axis, 
    //    Angle);

    animId_ = vessel.AddAnimation(vcMeshIndex, group_.get());

    auto vcEventId = vessel.RegisterForVCEvent(
        vcCockpitData_.ID, 
        vcControlData_.LocationA, 
        HitRadius, 
        [&](bco::IVessel& v) {onHitEvent(v); });
    
    vessel.RegisterForPanelEvent(
        pnlCockpitData_.ID, 
        pnlControlData_.HitRC, 
        [&](bco::IVessel& v) {onHitEvent(v); });

    panelRedrawId_ = vessel.RegisterForPanelRedraw(
        pnlCockpitData_.ID, 
        [&](bco::IVessel& v) {onPanelRedraw(v); });

    meshPanel_ = vessel.GetpanelMeshHandle(pnlCockpitData_.ID);
    textureOffset_ = bco::UVOffset(pnlControlData_.Verts);
}

inline void ToggleSwitch<dta::Cockpit, dta::RotateControl, dta::Cockpit, dta::StaticControl>::Step(bco::IVessel& vessel, double simdt)
//inline void ToggleSwitch::Step(bco::IVessel& vessel, double simdt)
{
    state_.target_state_ = isOn_ ? 1.0 : 0.0;
    animUpdate_.UpdateState(state_, simdt);
    vessel.SetAnimationState(animId_, state_.state_);
}

using Toggle = ToggleSwitch<dta::Cockpit, dta::RotateControl, dta::Cockpit, dta::StaticControl>;