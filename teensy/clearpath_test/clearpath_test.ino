#define ANALOG_WRITE_RES 10
#define CW 0
#define CCW 1

// Configured max motor RPM. This is set in the clearpath motor's configuration, done ahead of time via USB
#define CP_MOTOR_MAX_RPM 1000

typedef struct {
  char verb;
  float value;
} gcode_verb;

const int cp_enable = 1;
const int cp_a = 2;
const int cp_b = 3;

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

void serialEvent() {
  read_gcode_from_serial();
}

void loop() {
  move_clearpath(10, CW);
}
