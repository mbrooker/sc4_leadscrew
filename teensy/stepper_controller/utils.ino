// Convert a float in (-1, 1) to a PWM value, with 0 corresponding to half of the PWM range
int float_to_pwm_bipolar(float x) {
  return (1 << (ANALOG_WRITE_RES - 1)) * constrain(x + 1, 0, 2);
}

// Convert a float in [0, 1) to a PWM value, with 0 corresponding to 0
int float_to_pwm_unipolar(float x) {
  return (1 << ANALOG_WRITE_RES) * constrain(x, 0, 1);
}
