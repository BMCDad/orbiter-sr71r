/*
Tools - Orbiter Addon
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

#include "Orbitersdk.h"

namespace bc_orbiter
{
    /**
    Returns distance and direction from point one on a globe to point two given the longitude and
    latitude of each point.

    From Orbiter DeltaGlider code.

    @param lng1 Longitude for point 1.
    @param lat1 Latitude for point 1.
    @parma lng2 Longitude for point 2.
    @param lat2 Latitude for point 2.
    @param dist Returns the dist between the points.
    @param dir Returns the direction in radians.
    */
    inline void Orthodome(double lng1, double lat1, double lng2, double lat2, double& dist, double& dir)
    {
        double A = lng2 - lng1;
        double sinA = sin(A), cosA = cos(A);
        double slat1 = sin(lat1), clat1 = cos(lat1);
        double slat2 = sin(lat2), clat2 = cos(lat2);
        double cosa = slat2 * slat1 + clat2 * clat1 * cosA;
        dist = acos(cosa);
        dir = asin(clat2 * sinA / sin(dist));
        if (lat2 < lat1) dir = PI - dir; // point 2 further south than point 1
        if (dir < 0.0) dir += PI2;     // quadrant 4
    }

    /**
    \brief Call to load an event only VC event during Load VC.
    \param id Event id.
    \param loc Event click location.
    \param radius Hit radius.
    */
    inline void LoadVCSimpleEvent(int id, const VECTOR3& loc, double radius)
    {
        oapiVCRegisterArea(id, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
        oapiVCSetAreaClickmode_Spherical(id, loc, radius);
    }

    inline void LoadVCRedrawEvent(int id, const RECT& textureLoc, const VECTOR3& meshLoc, double radius, SURFHANDLE surface)
    {
        oapiVCRegisterArea(
            id, 
            textureLoc,
            PANEL_REDRAW_USER,
            PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
            PANEL_MAP_BACKGROUND,
            surface);

        oapiVCSetAreaClickmode_Spherical(id, meshLoc, radius);
    }

    /**
    \brief Call to load an event only panel event.
    \param vessel Calling vessel.
    \param eventId Event id.
    \param handle Panel handle
    \param loc Event mouse location.
    */
    inline void LoadPanelSimpleEvent(VESSEL3& vessel, int eventId, PANELHANDLE handle, const RECT& loc)
    {
        vessel.RegisterPanelArea(handle, eventId, loc, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
    }

    inline void LoadPanelRedrawEvent(VESSEL3& vessel, int eventId, PANELHANDLE handle, const RECT& rc, SURFHANDLE surface = nullptr)
    {
        vessel.RegisterPanelArea(
            handle,
            eventId,
            rc,
            PANEL_REDRAW_USER,
            PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_ONREPLAY,
            surface);
    }
}

