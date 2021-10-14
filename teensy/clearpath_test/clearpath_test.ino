#define ANALOG_WRITE_RES 10
#define GCODE_BUFFER_SIZE 128

char gcode_buffer[GCODE_BUFFER_SIZE] = { 0 };
int gcode_buffer_pos;

char *gcode_error;

const char *gcode_no_g_verb = "No such G verb";
const char *gcode_no_m_verb = "No such M verb";
const char *gcode_malformed = "Malformed G code";

typedef struct {
  char verb;
  float value;
} gcode_verb;

const int cp_enable = 1;
const int cp_a = 2;
const int cp_b = 3;

int float_to_pwm(float x) {
  return (1 << (ANALOG_WRITE_RES - 1)) * (x + 1);
}

void setup() {
  Serial.begin(9600);
  pinMode(cp_enable, OUTPUT);
  pinMode(cp_a, OUTPUT);
  pinMode(cp_b, OUTPUT);
  analogWriteResolution(10);
  // Magic number from https://www.pjrc.com/teensy/td_pulse.html
  analogWriteFrequency(cp_b, 46875);
  digitalWrite(cp_enable, HIGH);
}

int parse_verb(char *verb, int len, gcode_verb *verb) {
  
}

int g_verb(int verb, char *remainder) {
  
}

int m_verb(int verb, char *remainder) {
  
}

void interpret_gcode() {
  Serial.print("Got gcode: '");
  Serial.print(gcode_buffer);
  Serial.println("'");
}

void serialEvent() {
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

void loop() {
  analogWrite(cp_b, float_to_pwm(0.1));
  digitalWrite(cp_a, LOW);
  delay(5000);
  digitalWrite(cp_a, HIGH);
  delay(5000);
  analogWrite(cp_b, float_to_pwm(-0.1));
  digitalWrite(cp_a, LOW);
  delay(5000);
  digitalWrite(cp_a, HIGH);
}
