// This calls itself gcode, but really is just a very basic gcode-like line-by-line encoding.

#define GCODE_BUFFER_SIZE 128

char gcode_buffer[GCODE_BUFFER_SIZE] = { 0 };
int gcode_buffer_pos;

void handle_g(config_t *conf) {
  int num = atoi(&gcode_buffer[1]);
  switch (num) {
    case 98:
      conf->feed_mode = per_minute;
      break;
    case 99:
      conf->feed_mode = per_rev;
      break;
  }
}

void handle_f(config_t *conf) {
  conf->feed = atof(&gcode_buffer[1]);
}

void handle_m(config_t *conf) {
  int num = atoi(&gcode_buffer[1]);
  switch (num) {
    case 100:
      conf->feed_type = half_nut;
      break;
    case 101:
      conf->feed_type = power_feed;
      break;
  }
}

void interpret_gcode(config_t *conf) {
  switch (gcode_buffer[0]) {
    case 'G':
      handle_g(conf);
      break;
    case 'F':
      handle_f(conf);
      break;
    case 'M':
      handle_m(conf);
      break;
    default:
      break;
  }
}

void read_gcode_from_serial(config_t *conf) {
  while (Serial.available()) {
    char in = (char)Serial.read();
    gcode_buffer[gcode_buffer_pos] = in;
    if (in == '\n') {
      gcode_buffer[gcode_buffer_pos] = 0;
      interpret_gcode(conf);
      gcode_buffer_pos = 0;
    } else {
      gcode_buffer_pos = (gcode_buffer_pos + 1) % GCODE_BUFFER_SIZE;
    }
  }
}
