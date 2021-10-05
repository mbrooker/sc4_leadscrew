#include <Encoder.h>
#define RPM_FILTER_BUCKETS 5
#define RPM_FILTER_MS 1000
#define RPM_FILTER_A0 0.9
// Filter gain for the bucket filter, see gain.py for how its calculated from RPM_FILTER_BUCKETS and RPM_FILTER_A0
#define RPM_FILTER_GAIN 4.68

#define TICKS_PER_REV (4 * 360.0)

Encoder myEnc(1, 2);

long pos = -999;
long buckets[RPM_FILTER_BUCKETS];
long bucket_pointer = 0;
long rpm_filter_accum = 0;
long enc_bucket_start = 0;
double rpm = 0;

void setup() {
  enc_bucket_start = millis();
  // put your setup code here, to run once:

}

void update_rpm_filter() {
  buckets[bucket_pointer] = rpm_filter_accum;
  rpm_filter_accum = 0;
  double ticks = 0;
  double gain = 1.0;
  for (int i = 0; i < RPM_FILTER_BUCKETS; i++) {
   // Serial.print(buckets[(bucket_pointer + i) % RPM_FILTER_BUCKETS]);
  //  Serial.print(" ");
    ticks += buckets[(bucket_pointer + i) % RPM_FILTER_BUCKETS] * gain;
    gain *= RPM_FILTER_A0;
  }
 // Serial.println("");
  // Move the bucket pointer over by one
  bucket_pointer -= 1;
  if (bucket_pointer < 0) {
    bucket_pointer = RPM_FILTER_BUCKETS - 1;
  }
  // Now convert from ticks-per-period to RPM
  ticks /= RPM_FILTER_GAIN;
  double ticks_per_minute = ticks * 60000 / RPM_FILTER_MS;
  rpm = ticks_per_minute / TICKS_PER_REV;

  Serial.println(rpm);
 
}

void loop() {
  delay(100);
  long new_pos = myEnc.read();
  long now_millis = millis();

  if (new_pos != pos) {
    rpm_filter_accum += (new_pos - pos);
    pos = new_pos;
  }
  if ((now_millis - enc_bucket_start) > RPM_FILTER_MS) {
    update_rpm_filter();
  }
  // put your main code here, to run repeatedly:

}
