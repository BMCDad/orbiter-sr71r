//	AeroData - SR-71r Orbiter Addon
//	Copyright(C) 2023  Blake Christensen
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

#pragma

#include "bc_orbiter\control.h"
#include "bc_orbiter\signals.h"


namespace bco = bc_orbiter;

class AeroData :
	public bco::vessel_component,
	public bco::post_step {

public:
	enum AvionMode { AvionAtmo, AvionExo };

	AeroData();
	~AeroData() {}

	// post_step
	void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override;

	void SetCourse(double s);
	void SetHeading(double s);

	// Slots:
	//bco::slot<bool>&		EnabledSlot()			{ return enabledSlot_; }
	//bco::slot<bool>&		AvionicsModeSlot()		{ return avionicsModeSlot_;	}
	bco::slot<bool>&		SetCourseIncSlot()		{ return setCourseIncSlot_; }
	bco::slot<bool>&		SetCourseDecSlot()		{ return setCourseDecSlot_; }
	bco::slot<bool>&		SetHeadingIncSlot()		{ return setHeadingIncSlot_; }
	bco::slot<bool>&		SetHeadingDecSlot()		{ return setHeadingDecSlot_; }


	// Signals:
//	bco::signal<AvionMode>& AvionicsModeSignal()	{ return avionicsModeSignal_; }
	bco::signal<double>&	SetCourseSignal()		{ return setCourseSignal_; }
	bco::signal<double>&	SetHeadingSignal()		{ return setHeadingSignal_; }

private:
	// Slots:
	//bco::slot<bool>			enabledSlot_;				// Main avion power switch.
	//bco::slot<bool>			avionicsModeSlot_;

	bco::slot<bool>			setCourseIncSlot_;
	bco::slot<bool>			setCourseDecSlot_;
	bco::slot<bool>			setHeadingIncSlot_;
	bco::slot<bool>			setHeadingDecSlot_;

	// Signals:
//	bco::signal<AvionMode>	avionicsModeSignal_;
	bco::signal<double>		setCourseSignal_;
	bco::signal<double>		setHeadingSignal_;

	void UpdateSetCourse(double i);
	void UpdateSetHeading(double i);
};
