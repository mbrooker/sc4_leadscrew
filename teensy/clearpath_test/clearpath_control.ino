
// Set the speed of a Clearpath motor in "Follow Digital Velocity Command (Unipolar)" mode.
// `speed` is in the range [0, 1]
// `dir` is one of `CW` and `CCW` for clockwise and counter-clockwise
void move_clearpath(float speed, motor_direction dir) {
  if (dir == cw) {
    digitalWrite(pin_cp_dir, HIGH);
  } else {
    digitalWrite(pin_cp_dir, LOW);
  }
  float speed_0_to_1 = constrain(speed / CP_MOTOR_MAX_RPM, 0, 1);
  analogWrite(pin_cp_speed, float_to_pwm_unipolar(speed_0_to_1));
}
