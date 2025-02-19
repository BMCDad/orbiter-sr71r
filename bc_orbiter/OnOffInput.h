//	OnOffInput - bco Orbiter Library
//	Copyright(C) 2023  Blake Christensen
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

#pragma once

#include "control.h"

namespace bc_orbiter {

    using anim_ids = std::initializer_list<UINT>;

    /**
    * OnOffInputMeta
    * Meta data for OnOffInput.  This facilitates setting up a bunch of similar input controls.
    */
    struct OnOffInputMeta {
        double animRotation;
        double animSpeed;
        double animStart;
        double animEnd;
        double hitRadius;
        double pnlOffset;
        int	vcRedrawFlags;
        int vcMouseFlags;
        int pnlRedrawFlags;
        int pnlMouseFlags;
    };

    /**
    OnOffInput
    Provides an input only Control, such as a physical toggle switch.  VC UI is an Animation group, so
    a rotation axis and angle must be provided.  The Panel input is a texture, so the vertex and offset
    are required.
    Since much of the metadata will be the same, many of the metrics are provided via a control_data structure
    that can be reused for many Control. A bank of switches for example.
    */
    class OnOffInput 
      : public Control,
        public VCAnimation,
        public VCEventTarget,
        public PanelEventTarget,
        public OneWaySwitch,
        public signaller
    {
    public:
        OnOffInput(
            anim_ids const& vcAnimGroupIds,
            const VECTOR3& vcLocation,
            const VECTOR3& vcAxisLocation,
            const OnOffInputMeta& vcData,
            const UINT                  pnlGroup,
            const NTVERTEX* pnlVerts,
            const RECT& pnl,
            const int                   pnlId = 0
        ) :
            vcData_(vcData),
            vcAnimGroup_(
                vcAnimGroupIds,
                vcLocation, vcAxisLocation,
                vcData.animRotation,
                vcData.animStart, vcData.animEnd),
            pnlGroup_(pnlGroup),
            pnlVerts_(pnlVerts),
            pnlRect_(pnl),
            pnlOffset_(vcData.pnlOffset),
            animVC_(vcData_.animSpeed),
            pnlId_(pnlId)
        {
        }

        bool IsOn() const override { return state_; }

        // VCAnimation
        AnimationGroup*     VCAnimationGroup()        override { return &vcAnimGroup_; }
        double              VCAnimationSpeed() const  override { return vcData_.animSpeed; }
        double              VCStep(double simdt) override {
            animVC_.Step(state_ ? 1.0 : 0.0, simdt);
            return animVC_.GetState();
        }

        // VCEventTarget
        VECTOR3             VCEventLocation()         override { return vcAnimGroup_.location_; }
        double              VCEventRadius()           override { return vcData_.hitRadius; }
        int                 VCMouseFlags()            override { return vcData_.vcMouseFlags; }
        int                 VCRedrawFlags()           override { return vcData_.vcRedrawFlags; }

        // PanelEventTarget
        RECT                PanelRect()                override { return pnlRect_; }
        int                 PanelMouseFlags()         override { return vcData_.pnlMouseFlags; }
        int                 PanelRedrawFlags()        override { return vcData_.pnlRedrawFlags; }
        int                 PanelId()                  override { return pnlId_; }

        void OnPanelRedraw(MESHHANDLE meshPanel) override {
            DrawPanelOnOff(meshPanel, pnlGroup_, pnlVerts_, IsOn(), pnlOffset_);
        }

        // MouseEventTarget
        bool OnMouseClick(VESSEL4& vessel, int id, int event) override {
            //state_ = !state_;
            //fire();
            toggle_state(vessel);
            return true;
        }

        void AttachOnChange(const std::function<void()>& func) override {
            attach(func);
        }

        void toggle_state(VESSEL4& vessel) {
            state_ = !state_;
//            fire();
            vessel.TriggerRedrawArea(pnlId_, 0, GetId());
        }

        friend std::istream& operator>>(std::istream& input, OnOffInput& obj) {
            if (input) {
                bool isEnabled;
                input >> isEnabled;
                obj.state_ = isEnabled;
                obj.animVC_.SetState(isEnabled ? 1.0 : 0.0);
                obj.fire();
            }

            return input;
        }

        friend std::ostream& operator<<(std::ostream& output, OnOffInput& obj) {
            output << obj.state_ ? "1" : "0";
            return output;
        }

    private:
        AnimationGroup     vcAnimGroup_;
        bool                state_{ false };
        OnOffInputMeta   vcData_;
        UINT                pnlGroup_;
        const NTVERTEX* pnlVerts_;
        RECT                pnlRect_;
        double              pnlOffset_;
        AnimationTarget    animVC_;
        int                 pnlId_;
    };
}