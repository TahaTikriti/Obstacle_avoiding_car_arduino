#include "AFMotor.h"
#include <Servo.h>

#define ECHO_PIN A1 // Echo pin
#define TRIG_PIN A0 // Trigger pin

Servo myservo;

AF_DCMotor motor1(1, MOTOR12_64KHZ);
AF_DCMotor motor2(2, MOTOR12_64KHZ);
AF_DCMotor motor3(3, MOTOR12_64KHZ);
AF_DCMotor motor4(4, MOTOR12_64KHZ);

int distanceL, distanceF, distanceR;
const int safetyDistance = 20;

void setup() {
  Serial.begin(9600);
  Serial.println("Robot starting...");

  myservo.attach(10);
  myservo.write(90); // Center servo

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  setMotorSpeed(180); // Set a uniform speed for all motors
}

void loop() {
  distanceF = getDistance();
  Serial.print("Distance Front: ");
  Serial.println(distanceF);

  if (distanceF > safetyDistance && distanceF != -1) { // Check if distance is valid
    moveForward();
  } else if (distanceF != -1) {
    executeManeuver();
  } else {
    Serial.println("Error reading distance");
    stopMotors();
  }
}

void setMotorSpeed(int speed) {
  motor1.setSpeed(speed);
  motor2.setSpeed(speed);
  motor3.setSpeed(speed);
  motor4.setSpeed(speed);
}

void moveForward() {
  Serial.println("Moving Forward");
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30 ms timeout
  if (duration == 0) {
    return -1; // -1 indicates a timeout (no echo received)
  }
  return duration * 0.034 / 2;
}

void executeManeuver() {
  Serial.println("Obstacle Detected - Stopping");
  stopMotors();

  myservo.write(0);
  delay(300);
  distanceR = getDistance();

  myservo.write(170);
  delay(500);
  distanceL = getDistance();

  myservo.write(90);
  delay(300);

  compareDistance();
}

void stopMotors() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void compareDistance() {
  if (distanceL > distanceR) {
    turnLeft();
  } else if (distanceR > distanceL) {
    turnRight();
  } else {
    backupAndTurn();
  }
}

void turnLeft() {
  Serial.println("Turning Left");
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  delay(350);
}

void turnRight() {
  Serial.println("Turning Right");
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
  delay(350);
}

void backupAndTurn() {
  Serial.println("Backing Up");
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
  delay(300);

  turnLeft(); // Assuming default maneuver is to turn left
}
