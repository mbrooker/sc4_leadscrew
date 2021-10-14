
// Set the speed of a Clearpath motor in "Follow Digital Velocity Command (Unipolar)" mode.
// `speed` is in the range [0, 1]
// `dir` is one of `CW` and `CCW` for clockwise and counter-clockwise
void move_clearpath(float speed, int dir) {
  if (dir == CW) {
    digitalWrite(cp_a, HIGH);
  } else {
    digitalWrite(cp_a, LOW);
  }
  float speed_0_to_1 = constrain(speed / CP_MOTOR_MAX_RPM, 0, 1);
  analogWrite(cp_b, float_to_pwm_unipolar(speed_0_to_1));
}
