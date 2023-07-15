//	electrical_component - SR-71r Orbiter Addon
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

#pragma once

#include "BaseVessel.h"
#include "control.h"

namespace bc_orbiter {

	/**	electrical_component
		Models a component that uses the DC vessel power system.
	*/
	class electrical_component :
		public vessel_component,
		public post_step
	{
	public:
		electrical_component(const double min_volts = 24.0, const double amp_draw = 5.0) 
			:
			min_volts_(min_volts),
			amp_draw_(amp_draw)
		{}

		// post_step
		void handle_post_step(BaseVessel& vessel, double simt, double simdt, double mjd) override {
			if (IsPowered()) {
				AmpDraw(simdt);
			}
		}

		// Inputs:
		slot<double>&	VoltsInputSlot()	{ return slotVoltsInput_; }			// Power supply volts.
		signal<double>&	AmpDrawSignal()		{ return signalAmpDraw_; }			// Reports amp draw adjusted for step.

	protected:

		virtual bool IsPowered() { return slotVoltsInput_.value() > min_volts_; }
		virtual void AmpDraw(double simdt) { signalAmpDraw_.fire(amp_draw_ * simdt); }
	private:

		slot<double>		slotVoltsInput_;
		signal<double>		signalAmpDraw_;

		double min_volts_{ 0.0 };
		double amp_draw_{ 0.0 };
	};
}