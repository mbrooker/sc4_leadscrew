#define RPM_FILTER_BUCKETS 5
#define RPM_FILTER_MS 1000
#define RPM_FILTER_A0 0.9
// Filter gain for the bucket filter, see gain.py for how its calculated from RPM_FILTER_BUCKETS and RPM_FILTER_A0
#define RPM_FILTER_GAIN 4.68

#define TICKS_PER_REV (4 * 360.0)

void update_rpm_filter(encoder_state *state) {
  state->buckets[state->bucket_pointer] = state->rpm_filter_accum;
  state->rpm_filter_accum = 0;
  double ticks = 0;
  double gain = 1.0;
  for (int i = 0; i < RPM_FILTER_BUCKETS; i++) {
    ticks += state->buckets[(state->bucket_pointer + i) % RPM_FILTER_BUCKETS] * gain;
    gain *= RPM_FILTER_A0;
  }

  // Move the bucket pointer over by one
  state->bucket_pointer -= 1;
  if (state->bucket_pointer < 0) {
    state->bucket_pointer = RPM_FILTER_BUCKETS - 1;
  }
  // Now convert from ticks-per-period to RPM
  ticks /= RPM_FILTER_GAIN;
  double ticks_per_minute = ticks * 60000 / RPM_FILTER_MS;
  state->rpm = ticks_per_minute / TICKS_PER_REV;
}

void encoder_read_loop(long now, encoder_state *state) {
    long new_pos = enc.read();

   if (new_pos != state->pos) {
    state->rpm_filter_accum += (new_pos - state->pos);
    state->pos = new_pos;
  }
  if ((now - state->enc_bucket_start) > RPM_FILTER_MS) {
    update_rpm_filter(state);
  }
}
