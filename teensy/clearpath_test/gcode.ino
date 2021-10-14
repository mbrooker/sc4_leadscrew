#define GCODE_BUFFER_SIZE 128

char gcode_buffer[GCODE_BUFFER_SIZE] = { 0 };
int gcode_buffer_pos;

char *gcode_error;

const char *gcode_no_g_verb = "No such G verb";
const char *gcode_no_m_verb = "No such M verb";
const char *gcode_malformed = "Malformed G code";



int parse_verb(char *text, int len, gcode_verb *verb) {
  return 0;
}

int g_verb(int verb, char *remainder) {
  return 0;
}

int m_verb(int verb, char *remainder) {
  return 0;
}

void interpret_gcode() {
  Serial.print("Got gcode: '");
  Serial.print(gcode_buffer);
  Serial.println("'");
}

void read_gcode_from_serial() {
  while (Serial.available()) {
    char in = (char)Serial.read();
    gcode_buffer[gcode_buffer_pos] = in;
    if (in == '\n') {
      gcode_buffer[gcode_buffer_pos] = 0;
      interpret_gcode();
      gcode_buffer_pos = 0;
    } else {
      gcode_buffer_pos = (gcode_buffer_pos + 1) % GCODE_BUFFER_SIZE;
    }
  }
}
