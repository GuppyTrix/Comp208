#include <Encoder.h>

// =============================================================
// Motor Variables
// =============================================================
const int motor1a = 38;  
const int motor1b = 40;  

const int motor2a = 41;  
const int motor2b = 39;  

const int motor3a = 43;  
const int motor3b = 45;  

const int motor4a = 42;  
const int motor4b = 44;  

// =============================================================
// Distance calculations
// =============================================================
const int gearRatio = 63;
const int ticksPerRev = 44;
const float wheelDiameter = 120;  // mm
const float wheelCirc = wheelDiameter * M_PI;
const float ticksPerDegree = (gearRatio * ticksPerRev) / 360.0;
const float distancePerDegree = wheelCirc / 360.0;
const float distancePerTick = wheelCirc / (gearRatio * ticksPerRev);

const float WHEELBASE = 200.0;  // Distance between left and right wheels (mm)

// =============================================================
// Sensor Pins (not yet determined)
// =============================================================
const int PIN_CAN_LEFT = 22;     // High when can is to the left
const int PIN_CAN_RIGHT = 23;    // High when can is to the right
const int PIN_CAN_FOUND = 24;    // High when can is detected
const int PIN_CAN_ARRIVED = 25;  // High when robot reached can

// =============================================================
// Movement Variables
// =============================================================
const float DEFAULT_SPEED = 100;
const float HOMING_SPEED = 100;
const float TURN_ADJUSTMENT = 5.0;

// =============================================================
// Encoders
// =============================================================
Encoder FL_motor(50, 52);  // 1 Front Left
Encoder FR_motor(48, 46);  // 2 Front Right
Encoder BL_motor(49, 47);  // 3 Back Left
Encoder BR_motor(53, 51);  // 4 Back Right

// =============================================================
// Odometry Class
// =============================================================
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
      analogWrite(motorPinA, speed);
      digitalWrite(motorPinB, LOW);
    } else if (speed < 0) {
      digitalWrite(motorPinA, LOW);
      analogWrite(motorPinB, -speed);
    } else {
      stopMotor();
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
// Differential Drive Robot Controller
// =============================================================
class Robot {
private:
  Odometry* leftFront;
  Odometry* rightFront;
  Odometry* leftBack;
  Odometry* rightBack;

  float degreesToTurnDistance(float degrees) {
    // For differential drive: turning distance = (wheelbase * PI * degrees) / 360
    return (WHEELBASE * M_PI * abs(degrees)) / 360.0;
  }

public:
  Robot(
    Odometry* lf, Odometry* rf,
    Odometry* lb, Odometry* rb) {
    leftFront = lf;
    rightFront = rf;
    leftBack = lb;
    rightBack = rb;
  }

  // Update all motors
  void update() {
    if (leftFront) leftFront->update();
    if (rightFront) rightFront->update();
    if (leftBack) leftBack->update();
    if (rightBack) rightBack->update();
  }

  // =============================================================
  // Movement Functions
  // =============================================================
  void moveForward(float distanceMM, int speed = DEFAULT_SPEED) {
    // All wheels forward
    if (leftFront) leftFront->startMoveForward(distanceMM, speed);
    if (rightFront) rightFront->startMoveForward(distanceMM, speed);
    if (leftBack) leftBack->startMoveForward(distanceMM, speed);
    if (rightBack) rightBack->startMoveForward(distanceMM, speed);
  }

  void moveBackward(float distanceMM, int speed = DEFAULT_SPEED) {
    // All wheels backward
    if (leftFront) leftFront->startMoveBackward(distanceMM, speed);
    if (rightFront) rightFront->startMoveBackward(distanceMM, speed);
    if (leftBack) leftBack->startMoveBackward(distanceMM, speed);
    if (rightBack) rightBack->startMoveBackward(distanceMM, speed);
  }

    // Left wheels backward, right wheels forward
  void turnLeft(float degrees, int speed = DEFAULT_SPEED) {
    // needs converting to distance as move forward uses distacce not degrees
    float turnDistance = degreesToTurnDistance(degrees);

    if (leftFront) leftFront->startMoveBackward(turnDistance, speed);
    if (leftBack) leftBack->startMoveBackward(turnDistance, speed);
    if (rightFront) rightFront->startMoveForward(turnDistance, speed);
    if (rightBack) rightBack->startMoveForward(turnDistance, speed);
  }

    // Left wheels forward, right wheels backward
  void turnRight(float degrees, int speed = DEFAULT_SPEED) {
    // needs converting to distance as move forward uses distacce not degrees
    float turnDistance = degreesToTurnDistance(degrees);

    if (leftFront) leftFront->startMoveForward(turnDistance, speed);
    if (leftBack) leftBack->startMoveForward(turnDistance, speed);
    if (rightFront) rightFront->startMoveBackward(turnDistance, speed);
    if (rightBack) rightBack->startMoveBackward(turnDistance, speed);
  }

  void stopAll() {
    if (leftFront) leftFront->stopMovement();
    if (rightFront) rightFront->stopMovement();
    if (leftBack) leftBack->stopMovement();
    if (rightBack) rightBack->stopMovement();
  }

  // =============================================================
  // Homing Functions
  // =============================================================
  void adjustLeft(int speed = HOMING_SPEED) {
    // Small left adjustment
    turnLeft(TURN_ADJUSTMENT, speed);
  }

  void adjustRight(int speed = HOMING_SPEED) {
    // Small right adjustment
    turnRight(TURN_ADJUSTMENT, speed);
  }

  void moveForwardHoming(float distanceMM = 200.0, int speed = HOMING_SPEED) {
    // Move forward slowly for homing
    moveForward(distanceMM, speed);
  }

  // =============================================================
  // Sensor Reading
  // =============================================================
  bool isCanDetected() {
    return digitalRead(PIN_CAN_FOUND) == HIGH;
  }

  bool isCanArrived() {
    return digitalRead(PIN_CAN_ARRIVED) == HIGH;
  }

  bool isCanLeft() {
    return digitalRead(PIN_CAN_LEFT) == HIGH;
  }

  bool isCanRight() {
    return digitalRead(PIN_CAN_RIGHT) == HIGH;
  }

  // =============================================================
  // Status Methods
  // =============================================================
  bool isBusy() {
    bool busy = false;
    if (leftFront) busy = busy || leftFront->isBusy();
    if (rightFront) busy = busy || rightFront->isBusy();
    if (leftBack) busy = busy || leftBack->isBusy();
    if (rightBack) busy = busy || rightBack->isBusy();
    return busy;
  }

  bool allMovementsComplete() {
    bool complete = true;
    if (leftFront) complete = complete && !leftFront->isBusy();
    if (rightFront) complete = complete && !rightFront->isBusy();
    if (leftBack) complete = complete && !leftBack->isBusy();
    if (rightBack) complete = complete && !rightBack->isBusy();
    return complete;
  }
};

// =============================================================
// Finite State Machine
// =============================================================
enum RobotState {
  STATE_SEARCHING,  // Looking for can
  STATE_HOMING,     // Can detected, adjusting approach
  STATE_ARRIVED     // Can reached, stop all motors
};

class RobotFSM {
private:
  RobotState currentState;
  Robot* robot;
  unsigned long stateStartTime;
  unsigned long lastSensorCheck;

public:
  RobotFSM(Robot* dr) {
    robot = dr;
    currentState = STATE_SEARCHING;
    stateStartTime = millis();
    lastSensorCheck = 0;

    // Setup sensor pins
    pinMode(PIN_CAN_LEFT, INPUT);
    pinMode(PIN_CAN_RIGHT, INPUT);
    pinMode(PIN_CAN_FOUND, INPUT);
    pinMode(PIN_CAN_ARRIVED, INPUT);
  }

  void update() {
    // Update robot controller first
    if (robot) {
      robot->update();
    }

    // Check every 100ms if a can has been detected
    unsigned long currentTime = millis();
    if (currentTime - lastSensorCheck > 100) {
      checkSensors();
      lastSensorCheck = currentTime;
    }

    // Run state machine
    switch (currentState) {
      case STATE_SEARCHING:
        updateSearching();
        break;

      case STATE_HOMING:
        updateHoming();
        break;

      case STATE_ARRIVED:
        updateArrived();
        break;
    }
  }

  void checkSensors() {
    // Only check for can detection in SEARCHING state
    if (currentState == STATE_SEARCHING && robot->isCanDetected()) {
      Serial.println("FSM: Can detected! Switching to HOMING");
      currentState = STATE_HOMING;
      stateStartTime = millis();
      robot->stopAll();  // Stop current movements
    }

    // Check for arrival in HOMING state
    if (currentState == STATE_HOMING && robot->isCanArrived()) {
      Serial.println("FSM: Can reached! Switching to ARRIVED");
      currentState = STATE_ARRIVED;
      stateStartTime = millis();
      robot->stopAll();
    }
  }

  void updateSearching() {
    // This is where i need to  implement the search pattern

    // check if robot is idle and start new search movements:
    // if (!robot->isBusy()) {
    //   // Start the search movement here
    //   robot->moveForward(1000);  // Move 1 meter forward
    //   // or robot->turnLeft(45);  // Turn 45 degrees
    // }
  }

  void updateHoming() {
    // Check if we lost the can signal
    if (!robot->isCanDetected()) {
      Serial.println("FSM: Lost the can Returning to SEARCHING");
      currentState = STATE_SEARCHING;
      stateStartTime = millis();
      return;
    }

    // Check if we've arrived at the can
    if (robot->isCanArrived()) {
      Serial.println("FSM: Can reached! Switching to ARRIVED");
      currentState = STATE_ARRIVED;
      stateStartTime = millis();
      robot->stopAll();
      return;
    }

    // Only adjust if robot is not currently moving
    if (!robot->isBusy()) {
      bool canLeft = robot->isCanLeft();
      bool canRight = robot->isCanRight();

      if (canLeft && !canRight) {
        // Can is to the left - turn left while left pin is high
        Serial.println("HOMING: Can is LEFT - turning left");
        robot->turnLeft(TURN_ADJUSTMENT, HOMING_SPEED);
      } else if (canRight && !canLeft) {
        // Can is to the right - turn right while right pin is high
        Serial.println("HOMING: Can is RIGHT - turning right");
        robot->turnRight(TURN_ADJUSTMENT, HOMING_SPEED);
      } else if (!canLeft && !canRight) {
        // Can is centered - move forward
        Serial.println("HOMING: Can is CENTERED - moving forward");
        robot->moveForwardHoming();
      }
    }
  }

  void updateArrived() {
    // In ARRIVED state, all motors are stopped
    Serial.println("ARRIVED: At can location.");
  }

  // =============================================================
  // Public Control Methods
  // =============================================================
  void setState(RobotState newState) {
    currentState = newState;
    stateStartTime = millis();
    robot->stopAll();
  }

  RobotState getState() {
    return currentState;
  }

  String getStateString() {
    switch (currentState) {
      case STATE_SEARCHING: return "SEARCHING";
      case STATE_HOMING: return "HOMING";
      case STATE_ARRIVED: return "ARRIVED";
    }
  }

  // =============================================================
  // Methods to test robots movement
  // =============================================================
  void manualMoveForward(float distance) {
    robot->moveForward(distance);
  }

  void manualTurnLeft(float degrees) {
    robot->turnLeft(degrees);
  }

  void manualTurnRight(float degrees) {
    robot->turnRight(degrees);
  }

  void manualStop() {
    robot->stopAll();
  }
};

// =============================================================
// Global class definitions
// =============================================================
Odometry* odomFL = nullptr;
Odometry* odomFR = nullptr;
Odometry* odomBL = nullptr;
Odometry* odomBR = nullptr;
Robot* robot = nullptr;
RobotFSM* fsm = nullptr;

// =============================================================
// Setup and Loop
// =============================================================
void setup() {
  Serial.begin(9600);
  Serial.println("=== Differential Drive Robot Initialization ===");

  // Initialize individual odometry modules
  Odometry* odomFL = new Odometry(motor1a, motor1b, &FL_motor);
  Odometry* odomFR = new Odometry(motor2a, motor2b, &FR_motor);
  Odometry* odomBL = new Odometry(motor3a, motor3b, &BL_motor);
  Odometry* odomBR = new Odometry(motor4a, motor4b, &BR_motor);

  // Initialize differential drive robot controller
  Robot* robot = new Robot(odomFL, odomFR, odomBL, odomBR);

  // Initialize finite state machine
  RobotFSM* fsm = new RobotFSM(robot);

  Serial.println("Robot initialized.");
  Serial.println("Starting in SEARCHING state.");
  Serial.println("==============================================");
}

void loop() {
  // Update the finite state machine (handles everything)
  if (fsm) {
    // when fsm->update() is called it updates all the logic in robot and motors
    // fsm->update() -> robot->update(); -> if (leftFront) leftFront->update(); and all the other motors
    fsm->update();  
  }

  delay(10);
}