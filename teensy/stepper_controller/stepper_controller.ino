#include <Encoder.h>
#include <IntervalTimer.h>

// Run the inner loop every this number of millis.
#define LOOP_EVERY_MS 1
#define PRINT_EVERY_LOOP 1000

// Encoder ticks per revolution (from the encoder data sheet)
#define ENCODER_TICKS_PER_REV (4 * 360.0)
// Motor steps (or micro steps) per rev
#define STEPPER_TICKS_PER_REV 1000

// Leadscrew pitch, in MM per full revolution
#define LEADSCREW_PITCH 2.0
// Powerfeed gear ratio, as a ratio of the leadscrew pitch
#define POWERFEED_RATIO 0.18

// Pin allocations
const int pin_stepper_step = 21;
const int pin_stepper_dir = 20;

const int pin_encoder_a = 2;
const int pin_encoder_b = 3;
const int pin_encoder_z = 1;

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
  double feed;
  feedrate_mode feed_mode;
  feedrate_type feed_type;
} config_t;

Encoder enc(pin_encoder_a, pin_encoder_b);
IntervalTimer motor_timer;

config_t conf {
  0.25,
  per_rev,
  power_feed,
};

void setup() {
  Serial.begin(9600);
  pinMode(pin_stepper_step, OUTPUT);
  pinMode(pin_stepper_dir, OUTPUT);
  motor_timer.begin(step_motor, 20);
}

void serialEvent() {
  read_gcode_from_serial(&conf);
}

double encoder_pulses_to_leadscrew_pulses(double pulses, config_t *conf) {
  pulses /= ENCODER_TICKS_PER_REV; // Now in spindle revs
  pulses *= conf->feed / LEADSCREW_PITCH; // Now in leadscrew revs
  if (conf->feed_type == power_feed) {
    pulses /= POWERFEED_RATIO; // Correct for powerfeed gearbox
  }
  pulses *= STEPPER_TICKS_PER_REV; // Now in stepper pulses
  return pulses;
}

int loop_ctr = 0;
double total_encoder_ticks = 0;
double total_leadscrew_ticks = 0;

void loop() {
  long ticks = get_encoder_ticks();
  if (ticks != 0) {
    double leadscrew_ticks = encoder_pulses_to_leadscrew_pulses(ticks, &conf);
    if (leadscrew_ticks > 0) {
      set_motor_dir(cw);
    } else {
      set_motor_dir(ccw);
    }
    add_motor_steps(abs(leadscrew_ticks));

    total_encoder_ticks += ticks;
    total_leadscrew_ticks += leadscrew_ticks;
  }

  if ((++loop_ctr % PRINT_EVERY_LOOP) == 0) {
    Serial.print(60 * total_leadscrew_ticks / STEPPER_TICKS_PER_REV);
    Serial.print(" ");
    Serial.print(60 * total_encoder_ticks / ENCODER_TICKS_PER_REV);
    Serial.print(" ");
    Serial.print(conf.feed);
    Serial.print(" ");
    Serial.println(conf.feed_type);
    loop_ctr = 0;
        total_encoder_ticks = 0;
    total_leadscrew_ticks = 0;
  }
  
  delay(LOOP_EVERY_MS);
}
