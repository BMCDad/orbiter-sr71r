//	VCToggleSwitch - bco Orbiter Library
//	Copyright(C) 2017  Blake Christensen
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

#include "bco.h"
#include <functional>
#include "IAnimationState.h"
#include "EventTarget.h"
#include "OnOffSwitch.h"
#include "OrbiterAPI.h"
#include "BaseVessel.h"
#include "Animation.h"

namespace bc_orbiter
{
    /**	VCToggleSwitch
    */
    class VCToggleSwitch : public OnOffSwitch
    {
    public:
        VCToggleSwitch(
            UINT id, 
            VECTOR3 target,             // The event target and the base for rotation
            VECTOR3 axis,               // vector that along with target defines the rotation axis
            double angle = 1.5708,      // rotation angle, default 90 deg (RAD*20)
            double speed = 10.0) :      // Default speed
            OnOffSwitch(target, 0.01),
            group_({ id }, target, axis, angle, 0.0, 1.0)
        {
        }

        /**
        Setup
        Called from SetClassCaps, this can probably be moved into BaseVessel (call Setup from BaseVessel::SetClassCaps)
        */
        void Setup(BaseVessel* vessel);
    private:

        AnimationGroup     group_;
    };

    inline void VCToggleSwitch::Setup(BaseVessel* vessel)
    {
        auto aid = vessel->CreateVesselAnimation(this, 10.0);
        vessel->AddVesselAnimationComponent(aid, vessel->GetVCMeshIndex(), &group_);

        vessel->RegisterVCEventTarget(this);
    }
}
