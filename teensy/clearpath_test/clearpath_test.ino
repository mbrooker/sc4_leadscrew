#include <Encoder.h>

// Run the inner loop every this number of millis.
#define LOOP_EVERY_MS 100

#define RPM_FILTER_BUCKETS 5

#define ANALOG_WRITE_RES 10
// Configured max motor RPM. This is set in the clearpath motor's configuration, done ahead of time via USB
#define CP_MOTOR_MAX_RPM 1000

// Pin allocations
const int pin_cp_enable = 20;
const int pin_cp_dir = 21;
const int pin_cp_speed = 22;

const int pin_encoder_a = 1;
const int pin_encoder_b = 2;
const int pin_encoder_z = 3;
Encoder enc(pin_encoder_a, pin_encoder_b);

enum motor_direction {
  cw,
  ccw
};

enum feedrate_mode {
  per_minute, // G98
  per_rev // G99
};

typedef struct {
  char verb;
  float value;
} gcode_verb;

typedef struct {
  long rpm_filter_accum = 0;
  long enc_bucket_start = 0;
  long pos;
  long buckets[RPM_FILTER_BUCKETS];
  long bucket_pointer = 0;
  double rpm = 0;
} encoder_state;

long last_loop_start = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pin_cp_enable, OUTPUT);
  pinMode(pin_cp_dir, OUTPUT);
  pinMode(pin_cp_speed, OUTPUT);
  analogWriteResolution(10);
  // Magic number from https://www.pjrc.com/teensy/td_pulse.html for 96MHz CPU speed
  analogWriteFrequency(pin_cp_speed, 46875);
  digitalWrite(pin_cp_enable, HIGH);
}

void serialEvent() {
  read_gcode_from_serial();
}

void loop() {
  long now = millis();

  move_clearpath(10, cw);

  long end = millis();

  // Delay for the amount of time it takes to run this inner loop at a constant rate
  long delay_for = constrain(LOOP_EVERY_MS + now - end, 0, LOOP_EVERY_MS);
  Serial.println(delay_for);
  delay(delay_for);
}
