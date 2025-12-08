#include <Encoder.h>

// =============================================================
// Motor Variables
// =============================================================
const int motor1a = 38;
const int motor1b = 40;

const int motor2a = 41;
const int motor2b = 39;

const int motor3a = 45;
const int motor3b = 43;

const int motor4a = 42;
const int motor4b = 44;

Encoder FL_motor(2, 4);   // 1 Front Left
Encoder FR_motor(3, 5);   // 2 Front Right
Encoder BL_motor(20, 6);  // 3 Back Left
Encoder BR_motor(21, 7);  // 4 Back Right


// =============================================================
// Distance calculations
// =============================================================
const int gearRatio = 63;
const int ticksPerRev = 44;
const float wheelDiameter = 100;  // mm
const float wheelCirc = wheelDiameter * M_PI;
const float ticksPerDegree = (gearRatio * ticksPerRev) / 360.0;
const float distancePerDegree = wheelCirc / 360.0;
const float distancePerTick = wheelCirc / (gearRatio * ticksPerRev);

const float WHEELBASE = 142.0;  // Distance between left and right wheels (mm)

// =============================================================
// Movement Variables
// =============================================================
const float DEFAULT_SPEED = 100;
const float HOMING_SPEED = 100;
const float TURN_ADJUSTMENT = 5.0;


class Odometry {
private:
  int motorPinA;
  int motorPinB;
  Encoder* encoder;

  // Movement state variables
  typedef enum {
    STATE_IDLE,
    STATE_MOVING_FORWARD,
    STATE_MOVING_BACKWARD,
    STATE_TURNING
  } MoveState;

  MoveState moveState;  // What the motor is doing
  float targetValue;    // Target distance or angle
  float startEncoder;   // Encoder value at movement start
  float currentValue;   // Current progress
  int motorSpeed;
  bool movementComplete;

  // For turn calculations
  bool turningRight;

public:
  // Constructor
  Odometry(int motorA, int motorB, Encoder* enc) {
    motorPinA = motorA;
    motorPinB = motorB;
    encoder = enc;
    moveState = STATE_IDLE;
    movementComplete = true;
    motorSpeed = DEFAULT_SPEED;

    pinMode(motorPinA, OUTPUT);
    pinMode(motorPinB, OUTPUT);
  }

  // =============================================================
  // Motor Control Methods
  // =============================================================
  void driveMotor(int speed) {
    speed = constrain(speed, -255, 255);
    if (speed > 0) {
      digitalWrite(motorPinA, HIGH);
      digitalWrite(motorPinB, LOW);
    } else if (speed < 0) {
      digitalWrite(motorPinA, LOW);
      digitalWrite(motorPinB, HIGH);
    }
  }

  void stopMotor() {
    digitalWrite(motorPinA, LOW);
    digitalWrite(motorPinB, LOW);
  }

  // =============================================================
  // Movement Functions
  // =============================================================
  void startMoveForward(float distanceMM, int speed = DEFAULT_SPEED) {
    if (moveState != STATE_IDLE) return;

    targetValue = distanceMM;
    startEncoder = getDistanceMM();
    motorSpeed = speed;
    moveState = STATE_MOVING_FORWARD;
    movementComplete = false;

    driveMotor(speed);
  }

  void startMoveBackward(float distanceMM, int speed = DEFAULT_SPEED) {
    if (moveState != STATE_IDLE) return;

    targetValue = distanceMM;
    startEncoder = getDistanceMM();
    motorSpeed = speed;
    moveState = STATE_MOVING_BACKWARD;
    movementComplete = false;

    driveMotor(-speed);
  }

  void startTurn(float degrees, int speed = DEFAULT_SPEED) {
    if (moveState != STATE_IDLE) return;

    targetValue = abs(degrees);
    startEncoder = encoder->read();
    motorSpeed = speed;
    moveState = STATE_TURNING;
    movementComplete = false;
    turningRight = (degrees > 0);

    driveMotor(turningRight ? speed : -speed);
  }

  // =============================================================
  // Update Method
  // =============================================================
  void update() {
    if (moveState == STATE_IDLE) return;

    switch (moveState) {
      case STATE_MOVING_FORWARD:
      case STATE_MOVING_BACKWARD:
        updateDistance();
        break;

      case STATE_TURNING:
        updateAngle();
        break;
    }
  }

  void updateDistance() {
    currentValue = abs(getDistanceMM() - startEncoder);

    if (currentValue >= targetValue) {
      stopMovement();
    }
  }

  void updateAngle() {
    long currentTicks = encoder->read();
    currentValue = abs(currentTicks - startEncoder) / ticksPerDegree;

    if (currentValue >= targetValue) {
      stopMovement();
    }
  }

  void stopMovement() {
    stopMotor();
    moveState = STATE_IDLE;
    movementComplete = true;
    currentValue = 0;
    targetValue = 0;
  }

  // =============================================================
  // Status Methods
  // =============================================================
  bool isBusy() {
    return (moveState != STATE_IDLE);
  }

  bool isMovementComplete() {
    return movementComplete;
  }

  float getProgress() {
    if (targetValue == 0) return 0;
    return (currentValue / targetValue) * 100.0;
  }

  float getRemaining() {
    return targetValue - currentValue;
  }

  long readEncoder() {
    return encoder->read();
  }

  float getDistanceMM() {
    return readEncoder() * distancePerTick;
  }

  void resetEncoder() {
    encoder->write(0);
  }

  MoveState getMoveState() {
    return moveState;
  }
};

// =============================================================
// Global class definitions
// =============================================================
// Initialize individual odometry modules
Odometry odomFL = Odometry(motor1a, motor1b, &FL_motor);
Odometry odomFR = Odometry(motor2a, motor2b, &FR_motor);
Odometry odomBL = Odometry(motor3a, motor3b, &BL_motor);
Odometry odomBR = Odometry(motor4a, motor4b, &BR_motor);

void setup() {
  Serial.begin(9600);
  Serial.println("=== Robot Initialization ===");

  pinMode(motor1a, OUTPUT);
  pinMode(motor1b, OUTPUT);

  pinMode(motor2a, OUTPUT);
  pinMode(motor2b, OUTPUT);


  pinMode(motor3a, OUTPUT);
  pinMode(motor3b, OUTPUT);


  pinMode(motor4a, OUTPUT);
  pinMode(motor4b, OUTPUT);

  Serial.println("Robot initialized.");
  Serial.println("Starting in SEARCHING state.");
  Serial.println("==============================================");
}

void loop() {
  // Test motors one at a time
  Serial.println("Testing FL motor...");
  odomFL.driveMotor(100);
  delay(2000);
  odomFL.stopMotor();
  delay(1000);
  
  Serial.println("Testing FR motor...");
  odomFR.driveMotor(100);
  delay(2000);
  odomFR.stopMotor();
  delay(1000);
  
  Serial.println("Testing BL motor...");
  odomBL.driveMotor(100);
  delay(2000);
  odomBL.stopMotor();
  delay(1000);
  
  Serial.println("Testing BR motor...");
  odomBR.driveMotor(100);
  delay(2000);
  odomBR.stopMotor();
  delay(2000);
}
