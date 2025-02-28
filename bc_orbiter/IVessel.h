//	IVessel - bco Orbiter Library
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

#include "Animation.h"
#include "Orbitersdk.h"



/*
    VC Event:
    A VC event can be a redraw event, or a mouse event, or both.  For both the oapiRegister* method
    must be called in the VCOnLoad callback.  For mouse events oapiSetAreaClick* must also be called.
    An area ID can be used for both redraw and mouse click.

    -In the SetClassCaps:
    UINT RegisterForVCRedraw(int vcId, redrawFunc) -- Takes the VC id for the redraw area, returns an ID that can be used to request an element redraw itself.
    UINT RegisterForVCEvent(int vcId, VECTOR location, double radius, eventFunc) -- takes the VC id, location and radius.
    UINT RegisterForPanelRedraw(int panelId, redrawFunc) -- Same as VC call above
    UINT RegisterForPanelEvent(int panelId, RECT hitLocation, eventFunc) -- Panel version of above.
    void RequestVCRedraw(UINT id);
    void RequestPanelRedraw(UINT id);
    void RequestRedraw(UINT id, UINT id);

    With the register calls made, the Vessel will then manage the oapi* calls needed in LoadPanel and LoadVC.  It
    will also call the appropriate eventFunc calls when those events are triggered.
    -In LoadVC:


    So, a control like the toggle switch would require:
    - RegisterFor VC Event
    - Register for panel event
    - Register for panel redraw
    - Register VC animation.: will need mesh group id, rotation axis, range, speed
    - Event handler, which can be shared between VC, Panel
    - Redraw handler for panel: will need mesh group id for panel object.
    - Variable for the panel redraw
    - Variable to track the state of the switch
    * So, we need some way to manage all this as it is overly complex to do all this for one toggle switch.
    *
    * : Register(IVessel&, vcId, panelId, vcLocation, vcAxis1, vcAxis2, panelLocation) -- SetClassCaps
    * : SetEvent(std::function<void()> func) -- SetClassCaps


    Panel Event:
    A panel event can be a redraw event, or a mouse event, or both.  Everything for the panel
    events is handled by the RegisterPanel* method in the PanelOnLoad callback.  The flags
    determine what the area does.

    Animation Controls:
    Animation controls for both VC and panel have no special registration with the vessel.  Generally
    the controls are animated as part of the Step method, where the mesh itself can be directly modified.
    These animations are not registered with the vessel, but they can be.



    - OnLoad call oapiRegisterVC* with a unique id. and oapiSetAreaClick*
    - OnRedraw and OnMouseClick will be called with the unique id used.

*/
namespace bc_orbiter {

    struct IVessel;

    using funcRedraw = std::function<void(IVessel&)>;
    using funcEvent = std::function<void(IVessel&)>;

    struct AnimationGroup;

    struct IVessel
    {
        /* \brief Adds an animation group to the vessel.  This call is good for simple, single
        * mesh animations such as VC switches etc.  This call will generally be made in
        * the SetClassCaps methd.  Call SetAnimation in the Step method to update the state
        * of the animation.  This does not happen automatically.
        * \param meshIndex Index of the mesh
        * \param group AnimationGroup to add
        * \return The id for the animation
        */
        virtual auto AddAnimation(UINT meshIndex, IAnimationGroup& group) -> UINT = 0;

        /* \brief Adds an animation group to the vessel.  This call is good for simple, single
        * mesh animations such as VC switches etc.  This version simplifies and allows passing
        * of just a MGROUP transform.
        * \param meshIndex Index of the mesh
        * \param group AnimationGroup to add
        * \return The id for the animation
        */
        virtual auto AddAnimation(UINT meshIndex, MGROUP_TRANSFORM* transform) -> UINT = 0;

        /* \brief Adds a mesh to the vessel and returns its index.  This can be called
        * multiple times, it will only add once.  Get the mesh index from GetMeshIndex.
        * \param name The name of the mesh file.
        * \return Returns the mesh index.
        */
        virtual auto AddVesselMesh(const char* name) -> UINT = 0;

        /* \brief Updates the state of the animation.  Call this method in the Step method after
        * determining where the animation should be based on the simdt parameter.
        * \param animationId The animation id returned from AddAnimation.
        * \param value The new setting for the animation.
        */
        virtual auto SetAnimationState(UINT animationId, double state) -> void = 0;

        /* \brief Creates the animation with the base vessel and assigns it an id.
        * \param animation The animation object to create.
        */
        virtual auto CreateVesselAnimation(Animation& animation) -> UINT = 0;

        /* \brief Adds an Animation group to an existing Animation.  See CreateVesselAnimation.
        * \param animId The Animation id returned from CreateVesselAnimation.
        * \param meshIdx The mesh index to animate.
        * \param trans The mesh transformation.
        * \param parent The parent group if any.
        * \return Returns the animation component handle.
        */
        virtual auto AddVesselAnimationComponent(UINT animId, UINT meshIdx, IAnimationGroup& transform, ANIMATIONCOMPONENT_HANDLE parent = nullptr) -> ANIMATIONCOMPONENT_HANDLE = 0;

        /* \brief Returns the mesh handle given the mesh name.
        * \param name The name of the mesh.
        * \return Returns the mesh handle.
        */
        virtual auto GetMeshHandle(const char* name) -> MESHHANDLE = 0;

        /* Old method, should be cleaned up.
        */
        virtual auto GetpanelMeshHandle(int id) -> MESHHANDLE = 0;

        /* \brief Returns the device mesh for a mesh id.
        */
        virtual auto GetDeviceMesh(int id) -> DEVMESHHANDLE = 0;

        /* \brief Returns the mesh index for the given mesh.
        * \param name The name of the mesh.
        * \return The mesh index.
        */
        virtual auto GetMeshIndex(const char* name) -> UINT = 0;

        /* \brief Registers an event for the VC indicted in vcId.  The return value can
        * then be passed to RequestVCRedraw to trigger that redraw event.
        * This method should be called in SetClassCaps.  Vessel will handle registering this
        * event as part of the VC Load callback.
        * \param vcId The id for the VC where the redraw should happen.
        * \param redrawFunc The function that will be called in response to the redraw request.
        * \return The area ID that can be passed to RequestVCRedraw to trigger the redraw.
        */
        virtual auto RegisterForVCRedraw(int vcId, funcRedraw redrawFunc) -> UINT = 0;

        /* \brief Request an area to be redrawn.
        * \param vcId The VC where the redraw area belongs.
        * \param id The area id for the redraw.
        */
        virtual auto RequestVCRedraw(int vcId, UINT id) -> void = 0;

        /* \brief Register an area id for redraw.  This should be called in SetClassCaps.  The returned
        * area id can then be used to request the redraw.
        * \param panelId The panel the redraw belongs to.
        * \param redrawFunc The funcRedraw that will be called in response.
        */
        virtual auto RegisterForPanelRedraw(int panelId, funcRedraw redrawFunc) -> UINT = 0;

        /* \brief Request a panel area be redrawn.
        * \param panelId The panel the redraw area belongs to.
        * \param id The area id to redraw.
        */
        virtual auto RequestPanelRedraw(int panelId, UINT id) -> void = 0;

        /* \brief Register a location for a VC mouse event.  When the location, radius recieves a mouse click
        * the eventFunc will be called.  Returns the event id, which may be useful, but is not required.
        * \param vcId VC where the event applies.
        * \param location A VECTOR 3 location for the mouse event.
        * \param radius The radius around location for the mouse event.
        * \param eventFunc The funcEvent to call in response to the mouse click.
        */
        virtual auto RegisterForVCEvent(int vcId, const VECTOR3& location, double radius, funcEvent eventFunc) -> UINT = 0;

        /* \brief Register a panel event.  The mouse event will fire when a click occurs in the hitLocation.
        * \param panelId The panel the event belongs to.
        * \param hitLocation The location of the mouse click.
        * \param eventFunc The event func to call in response.
        * \return Returns the event area id.
        */
        virtual auto RegisterForPanelEvent(int panelId, RECT hitLocation, funcEvent eventFunc) -> UINT = 0;
    };
}