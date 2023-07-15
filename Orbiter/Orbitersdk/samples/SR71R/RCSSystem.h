//	RCSSystem - SR-71r Orbiter Addon
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

#include "bc_orbiter/control.h"
#include "bc_orbiter/simple_event.h"
#include "bc_orbiter/on_off_display.h"

#include "SR71r_mesh.h"

namespace bco = bc_orbiter;

class VESSEL3;

/** RCSMode
*/

class RCSSystem : 
	public bco::vessel_component
{
public:
	RCSSystem(bco::BaseVessel& vessel);

	// Callback:
	void OnRCSMode(int mode);

	bco::slot<bool>&	IsAeroActiveSlot()	{ return slotIsAeroActive_; }

private:
	bco::BaseVessel&	vessel_;

    void OnChanged(int mode);
	void ActiveChanged(bool isActive);

	bco::slot<bool>		slotIsAeroActive_;

	bco::simple_event<>		btnLinear_{
		bm::vc::vcRCSLin_location,
			0.01,
			bm::pnl::pnlRCSLin_RC
	};

	bco::on_off_display		lightLinear_{
		bm::vc::vcRCSLin_id,
			bm::vc::vcRCSLin_verts,
			bm::pnl::pnlRCSLin_id,
			bm::pnl::pnlRCSLin_verts,
			0.0352
	};

	bco::simple_event<>		btnRotate_{
		bm::vc::vcRCSRot_location,
			0.01,
			bm::pnl::pnlRCSRot_RC
	};

	bco::on_off_display		lightRotate_{
		bm::vc::vcRCSRot_id,
			bm::vc::vcRCSRot_verts,
			bm::pnl::pnlRCSRot_id,
			bm::pnl::pnlRCSRot_verts,
			0.0352
	};
};