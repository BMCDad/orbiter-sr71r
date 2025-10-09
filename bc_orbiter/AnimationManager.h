//	AnimationManager - SR-71r Orbiter Addon
//	Copyright(C) 2025  Blake Christensen
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

#include <map>

#include <functional>

#include "OrbiterSDK.h"
#include "AnimationGroup.h"

namespace bc_orbiter
{
    class AnimationManager
    {
    public:
        AnimationManager() = default;
        ~AnimationManager() = default;

        void RegisterAnimation(VESSEL4& vessel, IAnimationGroup& group)
        {
            auto meshIndex = GetMeshIndex(group.MeshName());
            if (meshIndex == 0)
            {
                // Mesh not found, cannot register animation.
                return;
            }

            // Create the animation in Orbiter and get the ID.
            auto aid = vessel.CreateAnimation(0);

            // Assign the mesh index the animation belongs to.
            group.UpdateMeshIndex(meshIndex);

            // Add the animation to the vessel.
            vessel.AddAnimationComponent(aid, group.Start(), group.Stop(), group.Transform());

            animations_[aid] = std::make_unique<IAnimationGroup*>(&group);
        }

        void OnStep(VESSEL4& vessel, double simt, double simdt, double mjd)  
        {  
            for (auto& [aid, anim] : animations_)  
            {  
                // Dereference the unique_ptr to get the actual IUpdateState*  
                auto newState = (*anim)->UpdateState(simdt);  

                // Set the new animation state.  
                vessel.SetAnimation(aid, newState);  
            }  
        }

        void AddMeshIndex(const std::string& meshName, UINT meshIndex)
        {
            meshIndices_[meshName] = meshIndex;
        }
    private:

        UINT GetMeshIndex(const std::string& meshName) const
        {
            auto it = meshIndices_.find(meshName);
            if (it != meshIndices_.end())
            {
                return it->second;
            }
            return 0;
        }

        std::map<int, std::unique_ptr<IAnimationGroup*>> animations_;
        std::map<std::string, UINT> meshIndices_;
    };

}