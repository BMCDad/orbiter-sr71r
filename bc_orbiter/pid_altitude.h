#pragma once

namespace bc_orbiter {

    class pid_altitude {
    public:
        pid_altitude() {}

        void reset(double target, double kp, double ki, double kd) {
            target_ = target;
            kp = kp;
            ki = ki;
            kd = kd;
        }
        double compute(double current_altitude, double target_altitude) {
            // Calculate the error
            double error = target_altitude - current_altitude;

            //// Calculate the integral term
            //integral_ += error;
            //integral_ = max(-1.0, min(1.0, integral_)); // clamp

            //// Calculate the derivative term
            //double derivative = (error - prev_error_);

            //// Compute the control output
            //double control_output = kp_ * error + ki_ * integral_ + kd_ * derivative;

            //// Update previous error for the next iteration
            //prev_error_ = error;

            //// Apply the control output to adjust the vertical speed
            //double adjusted_vertical_speed = current_speed + control_output;

            //return adjusted_vertical_speed;

            double output = kp_ * error;
            return output;
        }

    private:
        double kp_          { 0.5 };        // Proportional gain
        double ki_          { 0.1 };        // Integral gain
        double kd_          { 0.2 };        // Derivative gain
        double target_      { 0.0 };
        double prev_error_  { 0.0 };
        double integral_    { 0.0 };
    };
};