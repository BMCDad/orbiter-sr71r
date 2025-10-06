/*
IUIController - SR-71r Orbiter Addon
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
    \brief Class to model a texture translation for a 2D panel control.  The texture is translated
    horizontally based on a state value between 0.0 and n, where n represents a translation to the
    right equal to the width of the texture in the U direction.  For example, a simple light would
    be modeled with a texture that has the 'off' state on the left side and the 'on' state on the right side.
    The left side is state 0, and the right side is state 1.  More states can be added by adding more frames
    to the texture.  A 3 state texture would have states 0, 1, and 2.
    */
    class PanelTextureTranslate
    {
    public:
        /**
        \brief Constructor for a PanelTextureTranslate control.
        \param group The mesh group ID.
        \param vertices The NTVERTEX array for the mesh group.  Must have 4 vertices.
        \param panelId The panel ID (main or right) that this control is part of.
        */
        PanelTextureTranslate(
            const UINT group,
            const NTVERTEX* vertices,
            int panelId) :
            group_(group),
            vertices_(vertices),
            panelId_(panelId)
        {
            texOffset_ = UVOffset(vertices);
        }

        /**
        \brief Called during the setup of the implementing control.  The control ID is passed in
        so that the texture translation can register for panel redraw events.
        \param vessel The vessel the UI control is part of.
        \param ctrlId The control ID for this UI control.
        */
        virtual void Setup(Vessel& vessel, int ctrlId)
        {
            ctrlId_ = ctrlId;
            vessel.AddPanelRedrawEvent(ctrlId_, [this](int, int, SURFHANDLE) { OnRedrawPanelEvent(); });
        }

        /**
        \brief Called when a 2D panel is loading.  Does nothing if the panel ID does not match.
        \param vessel The vessel the UI control is part of.
        \param panelId The ID of the panel that is loading.
        \param handle The panel handle.
        */
        void LoadPanel(Vessel& vessel, int panelId, PANELHANDLE handle)
        {
            if (panelId != panelId_) return;

            vessel.RegisterPanelArea(
                handle,
                ctrlId_,
                _R(0, 0, 0, 0),
                PANEL_REDRAW_USER,
                PANEL_MOUSE_IGNORE);

            // Get the mesh handle for the panel here so we have it during redraw events.
            meshPanel_ = vessel.GetPanelMeshHandle(panelId_);
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
            vessel.TriggerPanelRedrawArea(panelId_, ctrlId_);
        }

    private:
        int             ctrlId_{ -1 };
        double          state_{ 0.0 };
        UINT            group_;
        const NTVERTEX* vertices_;
        int             panelId_;
        MESHHANDLE      meshPanel_{ nullptr };
        double          texOffset_{ 0.0 };

        void OnRedrawPanelEvent()
        {
            assert(meshPanel_ != nullptr);
            auto grp = oapiMeshGroup(meshPanel_, group_);

            float trans = (float)(state_ * texOffset_);
            grp->Vtx[0].tu = vertices_[0].tu + trans;
            grp->Vtx[1].tu = vertices_[1].tu + trans;
            grp->Vtx[2].tu = vertices_[2].tu + trans;
            grp->Vtx[3].tu = vertices_[3].tu + trans;
        }
    };
}