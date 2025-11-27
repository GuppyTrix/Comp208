#include <Ultrasonic.h>

// MOTOR PINS //
#define L_MOTOR_A 11
#define L_MOTOR_B 10

#define R_MOTOR_A 12
#define R_MOTOR_B 13

// ULTRASOUND SENSOR PINS //
#define L_TRIG 1
#define L_ECHO 0

#define M_TRIG 3
#define M_ECHO 2

#define R_TRIG 7
#define R_ECHO 6

const float SOUND_SPEED = 0.0343;
long duration;
float distance;
unsigned long lastMeasurement = 0;
const unsigned long MEASUREMENT_INTERVAL = 200;

// Robot states
enum State { 
  MOVING_FORWARD,
  STOPPING,
  TURNING_LEFT,
  TURNING_RIGHT 
};
State robotState = MOVING_FORWARD;
unsigned long stateStartTime = 0;

// PID variables
float Kp = 2.0;
float previousError = 0;
float integral = 0;

// Parameters
const int BASE_SPEED = 150;
const float STOP_DISTANCE = 15.0;
const unsigned long TURN_TIME = 800;

void setup() {
  Serial.begin(115200);

  // MOTORS//
  pinMode(L_MOTOR_A, OUTPUT);
  pinMode(L_MOTOR_B, OUTPUT);
  pinMode(R_MOTOR_A, OUTPUT);
  pinMode(R_MOTOR_B, OUTPUT);

  // ULTRASOUND SENSOR PINS //
  pinMode(L_TRIG, OUTPUT);
  pinMode(L_ECHO, INPUT);
  pinMode(M_TRIG, OUTPUT);
  pinMode(M_ECHO, INPUT);
  pinMode(R_TRIG, OUTPUT);
  pinMode(R_ECHO, INPUT);

  Serial.println("Robot Initialized");
}

void loop() {
  if (millis() - lastMeasurement >= MEASUREMENT_INTERVAL) {
    float frontDistance = measureDistance(M_TRIG, M_ECHO);
    float leftDistance = measureDistance(M_TRIG, M_ECHO);
    float rightDistance = measureDistance(M_TRIG, M_ECHO);

    runStateMachine(frontDistance);
    runStateMachine(rightDistance);
    runStateMachine(leftDistance);

    lastMeasurement = millis();
  }
}

void LEFTMOTOR(int f, int b) {
  analogWrite(L_MOTOR_A, f);
  analogWrite(L_MOTOR_B, b);
}

void RIGHTMOTOR(int f, int b) {
  analogWrite(R_MOTOR_A, f);
  analogWrite(R_MOTOR_B, b);
}

void moveForward(int speed) {
  LEFTMOTOR(speed, 0);
  RIGHTMOTOR(speed, 0);
}

void stopMotors() {
  LEFTMOTOR(0, 0);
  RIGHTMOTOR(0, 0);
}

void turnLeft(int speed = BASE_SPEED) {
  LEFTMOTOR(0, speed);
  RIGHTMOTOR(speed, 0);
}

void turnRight(int speed = BASE_SPEED) {
  LEFTMOTOR(speed, 0);
  RIGHTMOTOR(0, speed);
}

float measureDistance(int TRIG, int ECHO) {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);

  distance = (duration * SOUND_SPEED) / 2;

  return distance;
}

float pidControl(float currentDistance) {
  float error = currentDistance - STOP_DISTANCE;
  integral += error;
  float derivative = error - previousError;

  float output = Kp * error + 0.05 * integral + 0.8 * derivative;
  previousError = error;

  return output;
}

void runStateMachine(float frontDistance) {
  switch (robotState) {
    case MOVING_FORWARD:
      if (frontDistance > 0 && frontDistance < 30.0) {
        Serial.println("Object detected - Stopping");
        robotState = STOPPING;
        stateStartTime = millis();
        integral = 0;
      } else {
        moveForward(BASE_SPEED);
        Serial.print("Moving Forward - Distance: ");
        Serial.println(frontDistance);
      }
      break;

    case STOPPING:
      if (frontDistance > 0 && frontDistance <= STOP_DISTANCE + 5.0) {
        float pidOutput = pidControl(frontDistance);
        int motorSpeed = BASE_SPEED + pidOutput;

        if (motorSpeed <= 20 || frontDistance <= STOP_DISTANCE) {
          stopMotors();
          Serial.println("Stopped - Preparing to turn");
          delay(500);

          // Choose random direction
          if (random(2) == 0) {
            robotState = TURNING_LEFT;
            Serial.println("Turning Left");
          } else {
            robotState = TURNING_RIGHT;
            Serial.println("Turning Right");
          }
          stateStartTime = millis();
        } else {
          moveForward(constrain(motorSpeed, 0, BASE_SPEED));
        }
      } else {
        robotState = MOVING_FORWARD;
        Serial.println("Object moved - Resuming");
      }
      break;

    case TURNING_LEFT:
      turnLeft(BASE_SPEED - 50);
      if (millis() - stateStartTime >= TURN_TIME) {
        stopMotors();
        robotState = MOVING_FORWARD;
        Serial.println("Turn complete - Moving Forward");
        delay(200);
      }
      break;

    case TURNING_RIGHT:
      turnRight(BASE_SPEED - 50);
      if (millis() - stateStartTime >= TURN_TIME) {
        stopMotors();
        robotState = MOVING_FORWARD;
        Serial.println("Turn complete - Moving Forward");
        delay(200);
      }
      break;
  }
}