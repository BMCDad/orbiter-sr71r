//	GenericTank - SR-71r Orbiter Addon
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
#include "vessel.h"

#include <sstream>

namespace bc_orbiter {

    /**
    Base class for a basic tank that reports Level, can be filled via a pump, and can be drawn from.
    Units is determined by the implementer.

    Inputs:
    - slot <- volts Level
    - signal => amps per sec


    */
    class GenericTank
      : public VesselComponent,
        public PostStep,
        public PowerConsumer,
        public ManageState,
        public Consumable
    {
    public:
        GenericTank(PowerProvider& pwr, double capacity, double fillRate) 
          : power_(pwr),
            capacity_(capacity),
            fillRate_(fillRate)
        {
            power_.AttachConsumer(this);
        }

        // Consumable
        double Level() const override { return current_ / capacity_; }

        double Draw(double amount) override {
            auto draw_amount = max(0.0, min(current_, amount));
            SetNewCurrentLevel(current_ - draw_amount);
            return draw_amount;
        }

        // power_consumable
        double AmpDraw() const override {
            return (IsPowered() && isFilling_) ? AMPS_PUMP : 0.0;
        }

        // PostStep
        virtual void HandlePostStep(Vessel& vessel, double simt, double simdt, double mjd) override {
            auto tD = simt - prevTime_;

            if (fabs(tD) > 0.2)
            {
                isExternal_ = vessel.IsStoppedOrDocked();
                if (isExternal_ && IsPowered()) {
                    //sigIsAvailable_.fire(true);
                    UpdateIsAvailable(true);

                    if (isFilling_) {
                        FillTank(fillRate_ * simdt);
                    }
                }
                else {
                    //sigIsAvailable_.fire(false);
                    UpdateIsAvailable(false);
                    //sigIsFilling_.fire(false);
                    UpdateIsFilling(false);
                }
                prevTime_ = simt;
            }
        }

        // ManageState
        bool HandleLoadState(Vessel& vessel, const std::string& line) override {
            // [a b]  :  [current_quantity fillPumpOn]

            std::istringstream in(line);
            double amount;
            in >> amount >> isFilling_;
            current_ = amount * capacity_;
            current_ = fmax(0.0, current_);
            current_ = fmin(capacity_, current_);
            //sigIsFilling_.fire((isFilling_ == 1) ? true : false);
            UpdateIsFilling((isFilling_ == 1) ? true : false);
            UpdateLevel(current_ / capacity_);
            //sigLevel_.fire(current_ / capacity_);

            return true;
        }

        std::string HandleSaveState(Vessel& vessel) override {
            std::ostringstream os;
            os << current_ << " " << isFilling_;
            return os.str();
        }

    protected:
        bool IsPowered() const {
            return
                power_.VoltsAvailable() > VOLTS_MIN;
        }

        void FillTank(double amount) {
            SetNewCurrentLevel(min(current_ + amount, capacity_));

            if (current_ == capacity_) {
                isFilling_ = false;
                //sigIsFilling_.fire(isFilling_);
                UpdateIsFilling(isFilling_);
            }

            //sigLevel_.fire(current_ / capacity_);		// 0 to 1 range
            UpdateLevel(current_ / capacity_);
        }

        void ToggleFilling() {
            if (isFilling_) {
                isFilling_ = false;
            }
            else {
                if (IsPowered() && isExternal_) {
                    isFilling_ = true;
                }
            }
            UpdateIsFilling(isFilling_);
        }

        void SetNewCurrentLevel(double new_current) {
            current_ = new_current;
            UpdateLevel(current_ / capacity_);
        }

        virtual void UpdateLevel(double l) {};
        virtual void UpdateIsFilling(bool b) {};
        virtual void UpdateIsAvailable(bool b) {};

    private:
        PowerProvider&  power_;

        const double    VOLTS_MIN = 24.0;
        const double    AMPS_PUMP = 4.0;

        signal<bool>    sigIsFilling_;

        bool            isFilling_{ false };
        bool            isExternal_{ false };
        double          prevTime_{ 0.0 };

        double          capacity_;
        double          current_{ 0.0 };
        double          fillRate_;
    };
}
