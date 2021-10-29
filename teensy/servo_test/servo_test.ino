int pin_stepper_step = 21;
int pin_stepper_dir = 20;

void setup() {

  // put your setup code here, to run once:
  pinMode(pin_stepper_step, OUTPUT);
  pinMode(pin_stepper_dir, OUTPUT);

}

void step1k() {
  for (int i = 0; i < 1000; i++) {
    digitalWrite(pin_stepper_step, HIGH);
    delay(1);
    digitalWrite(pin_stepper_step, LOW);
    delay(1);
  }
}

void loop() {
  digitalWrite(pin_stepper_dir, LOW);
  step1k();
  digitalWrite(pin_stepper_dir, HIGH);
  step1k();

}
