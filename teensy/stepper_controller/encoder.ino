long encoder_pos;

long get_encoder_ticks() {
  long new_pos = enc.read();
  long new_ticks = (new_pos - encoder_pos);
  encoder_pos = new_pos;
  return new_ticks;
}
