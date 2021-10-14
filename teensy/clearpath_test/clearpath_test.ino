#include <Encoder.h>

// Run the inner loop every this number of millis.
#define LOOP_EVERY_MS 10
#define PRINT_EVERY_LOOP 100
#define ANALOG_WRITE_RES 10
#define RPM_FILTER_BUCKETS 4

// Configured max motor RPM. This is set in the clearpath motor's configuration, done ahead of time via USB
#define CP_MOTOR_MAX_RPM 1000
// Leadscrew pitch, in MM per full revolution
#define LEADSCREW_PITCH 2.0
// Powerfeed gear ratio, as a ratio of the leadscrew pitch
#define POWERFEED_RATIO 0.18

// Pin allocations
const int pin_cp_enable = 20;
const int pin_cp_dir = 21;
const int pin_cp_speed = 22;

const int pin_encoder_a = 1;
const int pin_encoder_b = 2;
const int pin_encoder_z = 3;

enum motor_direction {
  cw,
  ccw
};

enum feedrate_mode {
  per_minute, // G98
  per_rev, // G99
};

// There are two ways to make the carriage move with the lead screw: the half-nut (for threading), and power feed (for turning)
enum feedrate_type {
  half_nut,
  power_feed,
};

typedef struct {
  char verb;
  float value;
} gcode_verb;

typedef struct {
  long pos;
  long buckets[RPM_FILTER_BUCKETS];
  long bucket_pointer = 0;
  double rpm = 0;
} encoder_state;

typedef struct {
  double feed;
  feedrate_mode feed_mode;
  feedrate_type feed_type;
} config_t;

long last_loop_start = 0;
encoder_state enc_state;
Encoder enc(pin_encoder_a, pin_encoder_b);

config_t conf {
  0.25,
  per_rev,
  power_feed,
};

void setup() {
  Serial.begin(9600);
  pinMode(pin_cp_enable, OUTPUT);
  pinMode(pin_cp_dir, OUTPUT);
  analogWriteResolution(ANALOG_WRITE_RES);
  // Magic number from https://www.pjrc.com/teensy/td_pulse.html for 96MHz CPU speed
  analogWriteFrequency(pin_cp_speed, 35156.25);
  digitalWrite(pin_cp_enable, HIGH);
}

void serialEvent() {
  read_gcode_from_serial(&conf);
}

double calculate_leadscrew_rpm(double spindle_rpm, config_t *conf) {
  spindle_rpm = abs(spindle_rpm);
  if (conf->feed_type == half_nut) {
    return spindle_rpm * conf->feed / LEADSCREW_PITCH;
  } else if (conf->feed_type == power_feed) {
    // Power cross-feed mode
    return spindle_rpm * conf->feed / (LEADSCREW_PITCH * POWERFEED_RATIO);
  } else {
    return 0;
  }
}

motor_direction rpm_to_leadscrew_dir(double spindle_rpm) {
  if (spindle_rpm > 0) {
    return cw;
  } else {
    return ccw;
  }
}

int loop_ctr = 0;

void loop() {
  long now = millis();
  encoder_read_loop(now, &enc_state);
  double leadscrew_rpm = calculate_leadscrew_rpm(enc_state.rpm, &conf);
  motor_direction leadscrew_dir = rpm_to_leadscrew_dir(enc_state.rpm);
  move_clearpath(leadscrew_rpm, leadscrew_dir);
  if ((loop_ctr++ % PRINT_EVERY_LOOP) == 0) {
    Serial.print(enc_state.rpm);
    Serial.print(" ");
    Serial.print(leadscrew_rpm);
    Serial.print(" ");
    Serial.print(leadscrew_dir);
    Serial.print(" ");
    Serial.print(conf.feed);
    Serial.print(" ");
    Serial.println(conf.feed_type);
  }
  long end = millis();

  // Delay for the amount of time it takes to run this inner loop at a constant rate
  long delay_for = constrain(LOOP_EVERY_MS + now - end, 0, LOOP_EVERY_MS);
  delay(delay_for);
}
