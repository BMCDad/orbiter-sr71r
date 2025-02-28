#pragma once

#include "Orbitersdk.h"

namespace bc_orbiter
{
    namespace contracts
    {
        //template <typename T>
        //concept Cockpit = requires(T t) {
        //    { t.MeshName }      -> std::convertible_to<const char*>;        // Name of the mesh file to load the panel or VC.
        //    { t.ID }            -> std::convertible_to<int>;                // The ID for the VC or Panel cockpit.
        //};

        //template <typename T>
        //concept RotateControl = requires(T t) {
        //    { t.MeshGroup }     -> std::convertible_to<UINT>;               // Mesh group id.
        //    { t.LocationA }     -> std::convertible_to<const VECTOR3&>;     // Base location for the control
        //    { t.LocationB }     -> std::convertible_to<const VECTOR3&>;     // Location that defines the rotation axis.
        //};

        //template <typename T>
        //concept StaticControl = requires(T t) {
        //    { t.Group }         -> std::convertible_to<UINT>;               // Mesh group id
        //    { t.HitRC }         -> std::convertible_to<const RECT&>;        // Mouse hit rectangle.
        //    { t.Verts }         -> std::convertible_to<const NTVERTEX*>;    // Texture vertex
        //};

        //template <typename T>
        //concept Texture = requires(T t) {
        //    { t.GroupID }       -> std::convertible_to<UINT>;               // Mesh group id.
        //    { t.Verts }         -> std::convertible_to<const NTVERTEX*>;    // Mesh verts, assumed to have four members.
        //};
    }

    namespace data_type
    {
        struct Cockpit {
            const char* MeshName;
            int             ID;
        };

        struct RotateControl {
            UINT            MeshGroup;
            const VECTOR3& LocationA;
            const VECTOR3& LocationB;
        };

        struct StaticControl {
            UINT            Group;
            const RECT& HitRC;
            const NTVERTEX* Verts;
        };

        struct Texture {
            UINT            GroupID;
            const NTVERTEX* Verts;
        };
    }
}
