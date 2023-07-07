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
	public bco::post_step,
	public bco::manage_state,
	public bco::power_consumer {

public:
	enum AvionMode { AvionAtmo, AvionExo };

	AeroData();
	~AeroData() {}

	// post_step
	void handle_post_step(bco::BaseVessel& vessel, double simt, double simdt, double mjd) override;

	// manage_state
	bool handle_load_state(const std::string& line) override;
	std::string handle_save_state() override;

	// power_consumer
	double amp_load() override { return isAeroDataActive_.current() ? AMPS_USED : 0.0; }

	void SetCourse(double s);
	void SetHeading(double s);

	// Slots:
	bco::slot<bool>&		EnabledSlot()			{ return enabledSlot_; }			// Switch:  Main avionics is enabled
	bco::slot<bool>&		AvionicsModeSlot()		{ return avionicsModeSlot_;	}		// Switch:  Avionics mode switch, ON = ATMOSPHERE, OFF = EXO
	bco::slot<double>&		VoltsInputSlot()		{ return voltsInputSlot_; }			// Volts input from power
	bco::signal<bool>&		IsAeroActiveSignal()	{ return isAeroDataActive_; }		// Is aero available (switch is on, power is adequate)

	bco::slot<bool>&		SetCourseIncSlot()		{ return setCourseIncSlot_; }
	bco::slot<bool>&		SetCourseDecSlot()		{ return setCourseDecSlot_; }
	bco::slot<bool>&		SetHeadingIncSlot()		{ return setHeadingIncSlot_; }
	bco::slot<bool>&		SetHeadingDecSlot()		{ return setHeadingDecSlot_; }

	// Signals:
//	bco::signal<AvionMode>& AvionicsModeSignal()	{ return avionicsModeSignal_; }
	bco::signal<double>&	SetCourseSignal()		{ return setCourseSignal_; }
	bco::signal<double>&	SetHeadingSignal()		{ return setHeadingSignal_; }

	bco::signal<double>&	GForceSignal()			{ return gforceSignal_; }
	bco::signal<double>&	TrimSignal()			{ return trimSignal_; }
	bco::signal<double>&	AOASignal()				{ return aoaSignal_; }
	bco::signal<double>&	VSINeedleSignal()		{ return vsiNeedleSignal_; }
	bco::signal<double>&	BankSignal()			{ return signalBank_; }
	bco::signal<double>&	PitchSignal()			{ return signalPitch_; }

private:
	const double			MIN_VOLTS = 25.0;			// No areo data if voltage input drops below.
	const double			AMPS_USED = 5.0;


	bco::slot<bool>			enabledSlot_;				// Main avion power switch input.
	bco::slot<bool>			avionicsModeSlot_;			// Avionics mode switch input.
	bco::slot<double>		voltsInputSlot_;			// Power input.

	bco::slot<bool>			setCourseIncSlot_;
	bco::slot<bool>			setCourseDecSlot_;
	bco::slot<bool>			setHeadingIncSlot_;
	bco::slot<bool>			setHeadingDecSlot_;

	// Signals:
//	bco::signal<AvionMode>	avionicsModeSignal_;
	bco::signal<bool>		isAeroDataActive_;
	bco::signal<double>		setCourseSignal_;
	bco::signal<double>		setHeadingSignal_;

	bco::signal<double>		gforceSignal_;
	bco::signal<double>		trimSignal_;
	bco::signal<double>		aoaSignal_;

	bco::signal<double >	vsiNeedleSignal_;
	bco::signal<double>		signalBank_;
	bco::signal<double>		signalPitch_;


	void UpdateSetCourse(double i);
	void UpdateSetHeading(double i);
};
