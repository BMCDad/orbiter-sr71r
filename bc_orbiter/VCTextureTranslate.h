/*
VCTextureTranslate - SR-71r Orbiter Addon
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
#include "Vessel.h"

namespace bc_orbiter
{
    class Vessel;

    /**
    \brief Class to model a texture translation for a virtual cockpit control.  The texture is translated
    horizontally based on a state value between 0.0 and n, where n represents a translation to the
    right equal to the width of the texture in the U direction.  For example, a simple light would
    be modeled with a texture that has the 'off' state on the left side and the 'on' state on the right side.
    The left side is state 0, and the right side is state 1.  More states can be added by adding more frames
    to the texture.  A 3 state texture would have states 0, 1, and 2.
    */
    class VCTextureTranslate
    {
    public:
        /**
        \brief Constructor for a VCTextureTranslate control.
        \param group The mesh group ID.
        \param vertices The NTVERTEX array for the mesh group.  Must have 4 vertices.
        \param panelId The panel ID (main or right) that this control is part of.
        */
        VCTextureTranslate(
            const UINT group,
            const NTVERTEX* vertices) :
            group_(group),
            vertices_(vertices)
        {
            texOffset_ = UVOffset(vertices);
        }

        /**
        \brief Called during the setup of the implementing control.  The control ID is passed in
        so that the texture translation can register for VC redraw events.
        \param vessel The vessel the UI control is part of.
        \param ctrlId The control ID for this UI control.
        */
        virtual void Setup(Vessel& vessel, int ctrlId)
        {
            ctrlId_ = ctrlId;
            vessel.RegisterVCRedrawEvent(ctrlId_, [this](int, int, SURFHANDLE, DEVMESHHANDLE mesh) { OnRedrawVCAreaEvent(mesh); });
        }

        /**
        \brief Called when a 2D panel is loading.  Does nothing if the panel ID does not match.
        \param vessel The vessel the UI control is part of.
        \param panelId The ID of the panel that is loading.
        \param handle The panel handle.
        */
        void LoadVC(int vcId, Vessel& vessel)
        {
            if (vcId != 0) return; // Only one VC supported for now.    
            oapiVCRegisterArea(ctrlId_, PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE);
        }

        /**
        \brief Sets the state of the texture translation.  The state is a double value between 0.0 and n,
        where n represents a translation to the right equal to the width of the texture in the U direction.
        \param vessel The vessel the UI control is part of.
        \param state The state of the texture translation.
        */
        void SetState(Vessel& vessel, double state)
        {
            state_ = state;
            vessel.TriggerRedrawArea(-1, 0, ctrlId_);
        }

    private:
        int             ctrlId_{ -1 };
        double          state_{ 0.0 };
        UINT            group_;
        const NTVERTEX* vertices_;
        double          texOffset_{ 0.0 };

        void OnRedrawVCAreaEvent(DEVMESHHANDLE mesh)
        {
            NTVERTEX* delta = new NTVERTEX[4];

            float offset = texOffset_ * state_;
            TransformUV2d(vertices_, delta, 4, _V(offset, 0.0, 0.0), 0.0);

            GROUPEDITSPEC change{};
            change.flags = GRPEDIT_VTXTEX;
            change.nVtx = 4;
            change.vIdx = NULL; //Just use the mesh order
            change.Vtx = delta;
            auto res = oapiEditMeshGroup(mesh, group_, &change);
            delete[] delta;
        }

    };
}
