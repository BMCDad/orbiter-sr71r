//	generic_tank - SR-71r Orbiter Addon
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

#include "signals.h"
#include "control.h"

#include <sstream>

namespace bc_orbiter {

	/**
	Base class for a basic tank that reports level, can be filled via a pump, and can be drawn from.
	capacity is kg, fillrate kgs per second
	*/
	class generic_tank :
		public vessel_component,
		public post_step,
		public consumable,
		public manage_state,
		public power_consumer
	{
	public:
		generic_tank(double capacity, double fillRate) :
			capacity_(capacity),
			fillRate_(fillRate),
			level_(0.0),
			isFilling_(false),
			prevTime_(0.0),
			slotToggleFill_([&](bool v) { ToggleFilling(); }),
			slotExternalAvail_([&](bool v) {}) {
		}

		// post_step
		void handle_post_step(BaseVessel& vessel, double simt, double simdt, double mjd) override {
			if (fabs(simt - prevTime_) > 0.2)
			{
				if (slotExternalAvail_.value() && IsPowered()) {
					sigIsAvailable_.fire(true);

					if (sigIsFilling_.current()) {
						FillTank(fillRate_ * simdt);
					}
				}
				else {
					sigIsAvailable_.fire(false);
					sigIsFilling_.fire(false);
				}
				prevTime_ = simt;
			}
		}

		// consumable
		double draw(double amount) override {
			auto drawnAmount = min(level_, amount);
			level_ = level_ - drawnAmount;
			sigLevel_.fire(level_ / capacity_);		// Adjust to 0-1 range
			return drawnAmount;
		}

		// manage_state
		bool handle_load_state(const std::string& line) override {
			// [a b]  :  [level fillPumpOn]

			double level = 0.0;
			int isPumpOn = 1;

			std::istringstream in(line);

			if (in >> level >> isPumpOn) {
				auto lv = fmax(0.0, fmin(1.0, level));
				level_ = lv * capacity_;

				sigIsFilling_.fire((isPumpOn == 1) ? true : false);
				return true;
			}
			else {
				return false;
			}
		}

		std::string handle_save_state() override {
			std::ostringstream os;
			os << (level_ / capacity_) << " " << sigIsFilling_.current() ? 1 : 0;
			return os.str();
		}

		// power_consumer
		double amp_load() override { return sigIsFilling_.current() ? AMPS_PUMP : 0.0; }

		// Inputs:
		slot<double>&	VoltsInputSlot()		{ return voltsInputSlot_; }			// Volts input from power
		slot<bool>&		ToggleFillSlot()		{ return slotToggleFill_; }			// Button  Fill tank pump
		slot<bool>&		ExternAvailableSlot()	{ return slotExternalAvail_; }		// Is external connection.

		// Outputs
		signal<double>& LevelSignal()			{ return sigLevel_; }				// 0 - 1 : does adjustment based on capacity
		signal<bool>&	IsFillingSignal()		{ return sigIsFilling_; }			// Is fill pump running.
		signal<bool>&	IsAvailableSignal()		{ return sigIsAvailable_; }			// Is consumable available. (not sure if needed)

	protected:
		void FillTank(double amount) {
			level_ = min(level_ + amount, capacity_);

			if (level_ == capacity_) {
				isFilling_ = false;
				sigIsFilling_.fire(isFilling_);
			}

			sigLevel_.fire(level_);
		}

		void ToggleFilling() {
			if (isFilling_) {
				isFilling_ = false;
			}
			else {
				if (IsPowered() && slotExternalAvail_.value()) {
					isFilling_ = true;
				}
			}
			sigIsFilling_.fire(isFilling_);
		}

		bool IsPowered() { return (voltsInputSlot_.value() > VOLTS_MIN); }

		double Level() { return level_; }
	private:
		const double			VOLTS_MIN = 24.0;
		const double			AMPS_PUMP = 4.0;

		signal<double>			sigLevel_;
		signal<bool>			sigIsFilling_;
		signal<bool>			sigIsAvailable_;

		slot<bool>				slotToggleFill_;
		slot<bool>				slotExternalAvail_;
		slot<double>			voltsInputSlot_;			// Power input.

		bool					isFilling_;
		double					prevTime_;

		double					capacity_;
		double					level_;
		double					fillRate_;
	};
}
