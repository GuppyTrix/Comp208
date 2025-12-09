#include <Encoder.h>

const int motor3a = 38;
const int motor3b = 40;

const int motor1a = 41;
const int motor1b = 39;

const int motor2a = 43;
const int motor2b = 45;

const int motor4a = 42;
const int motor4b = 44;


Encoder FL_motor(20, 21);  // 1 Front Left
Encoder FR_motor(18, 19);  // 2 Front Right

void setup() {
  Serial.begin(9600);

  pinMode(motor1a, OUTPUT);
  pinMode(motor1b, OUTPUT);
  pinMode(motor2a, OUTPUT);
  pinMode(motor2b, OUTPUT);
  pinMode(motor3a, OUTPUT);
  pinMode(motor3b, OUTPUT);
  pinMode(motor4a, OUTPUT);
  pinMode(motor4b, OUTPUT);

}

void forward()
{
  digitalWrite(motor1a, HIGH);
  digitalWrite(motor1b, LOW);
  digitalWrite(motor2a, HIGH);
  digitalWrite(motor2b, LOW);
  digitalWrite(motor3a, HIGH);
  digitalWrite(motor3b, LOW);
  digitalWrite(motor4a, HIGH);
  digitalWrite(motor4b, LOW);
}

void backward()
{
  digitalWrite(motor1a, LOW);
  digitalWrite(motor1b, HIGH);
  digitalWrite(motor2a, LOW);
  digitalWrite(motor2b, HIGH);
  digitalWrite(motor3a, LOW);
  digitalWrite(motor3b, HIGH);
  digitalWrite(motor4a, LOW);
  digitalWrite(motor4b, HIGH);
}

void stopMotors()
{
  digitalWrite(motor1a, LOW);
  digitalWrite(motor1b, LOW);
  digitalWrite(motor2a, LOW);
  digitalWrite(motor2b, LOW);
  digitalWrite(motor3a, LOW);
  digitalWrite(motor3b, LOW);
  digitalWrite(motor4a, LOW);
  digitalWrite(motor4b, LOW);
}

void driveMotor(int motora, int motorb){
  digitalWrite(motora, HIGH);
  digitalWrite(motorb, LOW);

}

void loop() {
  // forward();
  driveMotor(motor4a, motor4b);
}
