// Ticks per revolution (from the encoder data sheet)
#define TICKS_PER_REV (4 * 360.0)

// Simple windowed-sinc FIR filter, aiming for a cutoff frequency of 0.2pi
double filter_coeffs[RPM_FILTER_BUCKETS] = { 0.094, 0.14, 0.173, 0.185, 0.173, 0.14, 0.094 };

void rpm_filter_fir(encoder_state *state, long new_ticks) {
  // Add the latest tick count to the bucket
  state->buckets[state->bucket_pointer] = new_ticks;

  // Apply the FIR filter to find the new RPM
  double ticks = 0;
  for (int i = 0; i < RPM_FILTER_BUCKETS; i++) {
    ticks += state->buckets[(state->bucket_pointer + i) % RPM_FILTER_BUCKETS] * filter_coeffs[i];
  }
  double ticks_per_minute = ticks * 60000 / LOOP_EVERY_MS;
  state->rpm = ticks_per_minute / TICKS_PER_REV;

  state->bucket_pointer -= 1;
  if (state->bucket_pointer < 0) {
    state->bucket_pointer = RPM_FILTER_BUCKETS - 1;
  }
}

void encoder_read_loop(long now, encoder_state *state) {
  long new_pos = enc.read();
  long new_ticks = (new_pos - state->pos);
  state->pos = new_pos;

  rpm_filter_fir(state, new_ticks);
}
