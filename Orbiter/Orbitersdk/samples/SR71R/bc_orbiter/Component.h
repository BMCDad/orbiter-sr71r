//	Component - bco Orbiter Library
//	Copyright(C) 2015  Blake Christensen
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

namespace bc_orbiter
{
	class BaseVessel;
}

#include <vector>

namespace bc_orbiter
{
	/**
	Base class for modeling the behavior of a vessel's component parts.
	*/
	class Component
	{
	public:
		Component(BaseVessel* baseVessel) : baseVessel_(baseVessel)
		{}

		/**
		Override to initialize things like animations etc. Called from clbkSetClassCaps.
		*/
		virtual void SetClassCaps() {}
		
        /**
        Override to load component settings from the configuration file.
        */
		virtual bool LoadConfiguration(char* key, FILEHANDLE scn, const char* configLine) { return false; }

        /**
        Override to save component settings in the configuration file.
        */
		virtual void SaveConfiguration(FILEHANDLE scn) const { }

        /**
        Override to setup the component Virtual Cockpit objects.
        */
		virtual bool LoadVC(int id) { return false; }

        /**
        Override to handle mouse events.
        */
		virtual bool MouseEvent(int id, int event) { return false; }

        /**
        Override to handle virtual cockpit redraw events.
        */
		virtual bool VCRedrawEvent(int id, int event, SURFHANDLE surf) { return false; }


		BaseVessel*	GetBaseVessel() const { return baseVessel_; }

	private:
		BaseVessel*	baseVessel_;
	};
}