#define ANALOG_WRITE_RES 10

const int cp_enable = 1;
const int cp_a = 2;
const int cp_b = 3;

int float_to_pwm(float x) {
  return (1 << (ANALOG_WRITE_RES - 1))*(x + 1);
}

void setup() {
  pinMode(cp_enable, OUTPUT);
  pinMode(cp_a, OUTPUT);
  pinMode(cp_b, OUTPUT);
  analogWriteResolution(10);
  // Magic number from https://www.pjrc.com/teensy/td_pulse.html
  analogWriteFrequency(cp_b, 58593.75);
  digitalWrite(cp_enable, HIGH);
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
