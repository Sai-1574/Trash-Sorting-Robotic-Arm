#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define BASE_CHANNEL 0
#define ELBOW_CHANNEL 2
#define SHOULDER_CHANNEL 1
#define END_EFFECTOR_CHANNEL 3

#define SERVOMIN  100
#define SERVOMAX  500
#define STEP_DELAY 20

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Arduino ready.");

  pwm.begin();
  pwm.setPWMFreq(60);
  delay(10);
}

void loop() {

    if (Serial.available()) {
    char command = Serial.read();
    Serial.print("Received command: ");
    Serial.println(command);

    if (command == '1') {
      executeRecyclableRoutine();
    } else if (command == '2') {
      executeNonRecyclableRoutine();
    } else if (command == '3') {
      executeHazardousRoutine();
    }
  }
}

void executeRecyclableRoutine() {
  moveServoSlowly(BASE_CHANNEL, 90, 90); 
  moveServoSlowly(SHOULDER_CHANNEL, 140, 140);
  moveServo(END_EFFECTOR_CHANNEL,160);
  delay(100);
  moveServoSlowly(SHOULDER_CHANNEL, 140, 60);
  delay(100);
  moveServo(END_EFFECTOR_CHANNEL,0);
  delay(100);
  moveServoSlowly(SHOULDER_CHANNEL, 60, 140);
  delay(100);
  moveServoSlowly(BASE_CHANNEL,90,0);
  delay(100);
  moveServoSlowly(SHOULDER_CHANNEL, 140, 60);
  delay(100);
  moveServo(END_EFFECTOR_CHANNEL,140);
  delay(100);
  moveServoSlowly(SHOULDER_CHANNEL, 60, 140);
  delay(500);
  moveServoSlowly(BASE_CHANNEL,0,90);
  delay(500);
}

void executeNonRecyclableRoutine() {
  moveServoSlowly(BASE_CHANNEL, 90, 90); 
  moveServoSlowly(SHOULDER_CHANNEL, 140, 140);
  moveServo(END_EFFECTOR_CHANNEL,160);
  delay(100);
  moveServoSlowly(SHOULDER_CHANNEL, 140, 60);
  delay(100);
  moveServo(END_EFFECTOR_CHANNEL,0);
  delay(100);
  moveServoSlowly(SHOULDER_CHANNEL, 60, 140);
  delay(100);
  moveServoSlowly(BASE_CHANNEL,90,180);
  delay(100);
  moveServoSlowly(SHOULDER_CHANNEL, 140, 60);
  delay(100);
  moveServo(END_EFFECTOR_CHANNEL,140);
  delay(100);
  moveServoSlowly(SHOULDER_CHANNEL, 60, 140);
  delay(500);
  moveServoSlowly(BASE_CHANNEL,180,90);
  delay(500);
}


void executeHazardousRoutine() {
    moveServoSlowly(BASE_CHANNEL, 90, 90); 
  moveServoSlowly(SHOULDER_CHANNEL, 140, 140);
  moveServo(END_EFFECTOR_CHANNEL,160);
  delay(100);
  moveServoSlowly(SHOULDER_CHANNEL, 140, 60);
  delay(100);
  moveServo(END_EFFECTOR_CHANNEL,0);
  delay(100);
  moveServoSlowly(SHOULDER_CHANNEL, 60, 140);
  delay(100);
  moveServoSlowly(BASE_CHANNEL,90,150);
  delay(100);
  moveServoSlowly(SHOULDER_CHANNEL, 140, 60);
  delay(100);
  moveServo(END_EFFECTOR_CHANNEL,140);
  delay(100);
  moveServoSlowly(SHOULDER_CHANNEL, 60, 140);
  delay(500);
  moveServoSlowly(BASE_CHANNEL,150,90);
  delay(500);
}

void moveServo(uint8_t channel, int position) {
  int pulseLength = map(position, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(channel, 0, pulseLength);
}

void moveServoSlowly(uint8_t channel, int startPos, int endPos) {
  if (startPos < endPos) {
    for (int pos = startPos; pos <= endPos; pos++) {
      int pulseLength = map(pos, 0, 180, SERVOMIN, SERVOMAX);
      pwm.setPWM(channel, 0, pulseLength);
      delay(STEP_DELAY);
    }
  } else {
    for (int pos = startPos; pos >= endPos; pos--) {
      int pulseLength = map(pos, 0, 180, SERVOMIN, SERVOMAX);
      pwm.setPWM(channel, 0, pulseLength);
      delay(STEP_DELAY);
    }
  }
}
