#pragma once

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

//template <con::Cockpit Tvc, con::RotateControl TvcCtrl, con::Cockpit Tpnl, con::Texture TpnlCtrl>
class Gauge
{
public:
    explicit Gauge(dta::Cockpit& vct, dta::Texture& vcc, dta::Cockpit& pnlt, dta::Texture& pnlc, double range, double speed)
      : vcCockpitData_(vct),
        vcControlData_(vcc),
        pnlCockpitData_(pnlt),
        pnlControlData_(pnlc),
        range_(range),
        speed_(speed)
    { }

    auto Register(bco::IVessel& vessel) -> void;
    auto Step(bco::IVessel& vessel, double simdt) -> void;

    auto SetLevel(double level) -> void { level_ = level; }
private:
    //Tvc         vcCockpitData_;
    //TvcCtrl     vcControlData_;
    //Tpnl        pnlCockpitData_;
    //TpnlCtrl    pnlControlData_;

    dta::Cockpit    vcCockpitData_;
    dta::Texture    vcControlData_;
    dta::Cockpit    pnlCockpitData_;
    dta::Texture    pnlControlData_;

    double      range_{ 0.0 };
    double      speed_{ 1.0 };
    double      level_{ 0.0 };

    int                     animId_{ 0 };
    bco::StateUpdateTarget  animUpdate_;
    bco::StateUpdate        state_{ 10.0, 0.0, 0.0 };

    std::unique_ptr<MGROUP_ROTATE>  group_;
    UINT                            meshId_{ 0 };
    MESHHANDLE                      meshPanel_{ nullptr };

};

//inline void Gauge<dta::Cockpit, dta::RotateControl, dta::Cockpit, dta::Texture>::Register(bco::IVessel& vessel)
inline void Gauge::Register(bco::IVessel& vessel)
{
    //auto vcMeshIndex = vessel.GetMeshIndex(vcCockpitData_.MeshName);
    //VECTOR3 axis = vcControlData_.LocationB - vcControlData_.LocationA;
    //normalise(axis);

    //group_ = std::make_unique<MGROUP_ROTATE>(
    //    vcMeshIndex,
    //    &vcControlData_.MeshGroup,
    //    1,
    //    vcControlData_.LocationA,
    //    axis,
    //    range_);

    //animId_ = vessel.AddAnimation(vcMeshIndex, group_.get());
    //meshPanel_ = vessel.GetpanelMeshHandle(pnlCockpitData_.ID);
}

inline void Gauge::Step(bco::IVessel& vessel, double simdt)
{
    //state_.target_state_ = level_;
    //animUpdate_.UpdateState(state_, simdt);
    //vessel.SetAnimationState(animId_, state_.state_);

//    bco::RotateMesh(meshPanel_, pnlControlD
}
