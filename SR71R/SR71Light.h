/*
SR71 Light - SR-71r Orbiter Addon
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

SR71 Toggle Switch represents a physical on / off toggle in the SR71
cockpit.  It uses an animation for the VC representation, and a static
texture image for the 2D cockpits.  It maintains a simple state of
on or off.

Use:
Include this file in the component source.

*/
#pragma once

#include <vector>
#include <limits>
#include <functional>

#include "..\bc_orbiter\Vessel.h"
#include "..\bc_orbiter\AnimatedValue.h"
#include "..\bc_orbiter\MeshTools.h"
#include "..\bc_orbiter\Tools.h"
#include "..\bc_orbiter\AnimationGroup.h"
#include "..\bc_orbiter\IUIControl.h"
#include "..\bc_orbiter\PanelTextureTranslate.h"
#include "..\bc_orbiter\VCTextureTranslate.h"

#include "SR71r_mesh.h"
#include "ShipMets.h"

namespace bco = bc_orbiter;

namespace SR71
{
    const int StatusOn      = 0;
    const int StatusOff     = 1;
    const int StatusError   = 2;
    const int StatusWarn    = 3;

    /**
    * \brief Light UI component for the SR-71r.
    * \brief Member declaration layout:
    * \brief VC Group ID: The mesh group ID for the light in the VC.
    * \brief VC NTVERTEX* The vertices array for the light in the VC.
    * \brief Panel Group ID: The mesh group ID for the light in the 2D panel.
    * \brief Panel NTVERTEX*:  The vertices array light in the panel.
    * \brief Panel ID: ID for the panel itself, either main or right.
    */
    class Light : 
        public bco::IUIControl, 
        public bco::PanelTextureTranslate, 
        public bco::VCTextureTranslate
    {
    public:
        /**
        * \brief Constructor for a Light control.
        * \param vcGroupId The mesh group ID for the VC representation of the light.
        * \param vcVerts The vertices for the VC representation of the light.
        * \param pnlGroup The mesh group ID for the 2D panel representation of the light.
        * \param pnlVerts The vertices for the 2D panel representation of the light.
        * \param pnlId The panel ID for the 2D panel representation of the light.
        */
        Light(
            const UINT vcGroupId,
            const NTVERTEX* vcVerts,
            const UINT pnlGroup,
            const NTVERTEX* pnlVerts,
            const int pnlId
        );

        // IUIControl interface
        void Setup(bco::Vessel& vessel) override;
        void LoadVC(int vcId, bco::Vessel& vessel) override;
        void LoadPanel(int panelId, bco::Vessel& vessel, PANELHANDLE handle);

        void SetState(bool state);
        void SetState(int state);

    private:

        bco::Vessel*    vessel_{ nullptr };
        int             ctrlId_{ -1 };
    };

    inline Light::Light(
        const UINT vcGroupId,
        const NTVERTEX* vcVerts,
        const UINT pnlGroup,
        const NTVERTEX* pnlVerts,
        const int pnlId
    ) :
        bco::PanelTextureTranslate(pnlGroup, pnlVerts, pnlId),
        bco::VCTextureTranslate(vcGroupId, vcVerts)
    {}

    inline void Light::Setup(bco::Vessel& vessel)
    {
        // Setup the event handler.
        vessel_ = &vessel;
       
        ctrlId_ = vessel.GetControlId();

        bco::VCTextureTranslate::Setup(vessel, ctrlId_);
        bco::PanelTextureTranslate::Setup(vessel, ctrlId_);
    }

    inline void Light::LoadVC(int vcId, bco::Vessel& vessel)
    {
        bco::VCTextureTranslate::LoadVC(vcId, vessel);
    }

    inline void Light::LoadPanel(int panelId, bco::Vessel& vessel, PANELHANDLE handle)
    {
        bco::PanelTextureTranslate::LoadPanel(vessel, panelId, handle);
    }

    inline void Light::SetState(int state)
    {
        assert(vessel_);
        if (vessel_) {
            bco::PanelTextureTranslate::SetState(*vessel_, state);
            bco::VCTextureTranslate::SetState(*vessel_, state);
        }
    }

    inline void Light::SetState(bool state)
    {
        SetState(state ? 1 : 0);
    }
}
