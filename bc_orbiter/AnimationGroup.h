/*
AnimationGroup - SR-71r Orbiter Addon
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

#include "OrbiterAPI.h"
#include "AnimationUpdate.h"

namespace bc_orbiter
{
    using UpdateTargetFunction = std::function<double()>;

    class AnimationGroup
    {
    public:
        /**
        * \brief Constructor to create a rotating animation.
        * \param meshName The name of the mesh where the animation group is located.  This should be the same name
        * used when loading the mesh.
        * \param grp A list of mesh group ids that will be part of the animation.
        * \param locA The location of the 'root' of the animation axis.
        * \param locB The location that along with locA defines the rotation axis vector.
        * \param angle The angle of rotation.
        * \param start The point in an animation sequence where this animation starts.  0 to 1.
        * \param stop The point in an animation sequence where this animation stop.  0 to 1.
        * \param speed The speed of the animation.
        * \param funcTarget The function that will be called to update the animation target.
        */
        //AnimationGroup(
        //    const char* meshName,
        //    std::initializer_list<UINT> const& grp,
        //    const VECTOR3& locA, const VECTOR3& locB,
        //    double angle,
        //    double start, double stop,
        //    double speed,
        //    UpdateTargetFunction funcTarget) :
        //    group_(grp),
        //    start_(start),
        //    stop_(stop),
        //    location_(locA),
        //    speed_(speed),
        //    targetFunc_(funcTarget),
        //    meshName_(meshName)
        //{
        //    VECTOR3 axis = locB - locA;
        //    normalise(axis);
        //    transform_ = std::make_unique<MGROUP_ROTATE>(0, group_.data(), group_.size(), locA, axis, (float)angle);
        //    state_.Speed = speed_;
        //}

        AnimationGroup(
            int meshIndex,
            std::initializer_list<UINT> const& grp,
            const VECTOR3& locA, const VECTOR3& locB,
            double angle,
            double start, double stop) :
            group_(grp),
            start_(start),
            stop_(stop),
            location_(locA)
        {
            VECTOR3 axis = locB - locA;
            normalise(axis);
            transform_ = std::make_unique<MGROUP_ROTATE>(0, group_.data(), group_.size(), locA, axis, (float)angle);
            transform_->mesh = meshIndex; // Set the mesh index for the animation
        }

        /**
        * \brief Constructor to create a linear animation.
        * \param meshName The name of the mesh where the animation group is located.  This should be the same name
        * used when loading the mesh.
        * \param grp A list of mesh group ids that will be part of the animation.
        * \param translate The 3D vector that defines the animation 'route'.
        * \param start The point in an animation sequence where this animation starts.  0 to 1.
        * \param stop The point in an animation sequence where this animation stop.  0 to 1.
        * \param speed The speed of the animation.
        * \param funcTarget The function that will be called to update the animation target.
        */
        AnimationGroup(
            const char* meshName,
            std::initializer_list<UINT> const& grp,
            const VECTOR3& translate,
            double start, double stop) :
            group_(grp),
            start_(start),
            stop_(stop),
            location_(_V(0.0, 0.0, 0.0))
        {
            transform_ = std::make_unique<MGROUP_TRANSLATE>(0, group_.data(), group_.size(), translate);
        }

        double Start() const { return start_; }
        double Stop() const { return stop_; }
        MGROUP_TRANSFORM* Transform() const { return transform_.get(); }

    private:
        std::vector<UINT>                   group_;
        VECTOR3                             location_{ _V(0.0,0.0,0.0) };
        std::unique_ptr<MGROUP_TRANSFORM>   transform_;
        double                              start_;
        double                              stop_;
        int                                 meshIndex_{ 0 }; // Mesh index for the animation
    };
}