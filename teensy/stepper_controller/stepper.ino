#include <assert.h>

volatile int outstanding_steps;
volatile motor_direction desired_dir;

double outstanding_steps_delta;
int motor_pin;
motor_direction current_dir;

// step_motor needs to be called at most every 5us (because the CL86T needs pulses at least 5us long), but as often as possible up to that point.
//  Calling it more often will increase accuracy, and top speed. For example, at 1000 PPR, calling every 60us will limit the motor to 500 RPM.
void step_motor() {
    // The program has asked the motor to change direction. Wait until we've fed all the pulses we've got outstanding, then flip the direction pin.
    if ((outstanding_steps == 0) && (current_dir != desired_dir)) {
        current_dir = desired_dir;
        if (current_dir == cw) {
          digitalWrite(pin_stepper_dir, LOW);
        } else {
          digitalWrite(pin_stepper_dir, HIGH);
        }
    }
    // Form a pulse. If the pin is currently high, pull it low.
    if (motor_pin == HIGH) {
      motor_pin = LOW;
      digitalWrite(pin_stepper_step, LOW);
    } else if (outstanding_steps > 0) {
      // If it's currently low, and there are outstanding steps to do, pull it high.
      outstanding_steps -= 1;
      motor_pin = HIGH;
      digitalWrite(pin_stepper_step, HIGH);
    }
}

// Request `steps` steps be executed. If `steps` is <1, we accumulate them until there's a need to make a complete step.
// step_motor() is run as an interrupt, and therefore we need to make sure it doesn't run while we do this math.
void add_motor_steps(double steps) {
  assert(steps > 0);
  outstanding_steps_delta += steps;
  if (outstanding_steps_delta > 1) {
    int whole_steps = (int)outstanding_steps_delta;
    noInterrupts();
    outstanding_steps += whole_steps;
    interrupts();
    outstanding_steps_delta -= whole_steps;
    assert(outstanding_steps_delta > 0);
  }
}

// Set the motor direction to `dir`
void set_motor_dir(motor_direction dir) {
  if (desired_dir != dir) {
    noInterrupts();
    desired_dir = dir;
    interrupts();
  }
}
