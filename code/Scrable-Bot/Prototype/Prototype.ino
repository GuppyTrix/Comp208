#include <Encoder.h>

// =============================================================
// Motor Variables
// =============================================================
const int motor3a = 39;
const int motor3b = 41;

const int motor2a = 44;
const int motor2b = 42;

const int motor1a = 40;
const int motor1b = 38;

const int motor4a = 43;
const int motor4b = 45;



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
// Sensor Pins
// =============================================================
const int PIN_CAN_LEFT = 27;     // High when can is to the left
const int PIN_CAN_RIGHT = 25;    // High when can is to the right
const int PIN_CAN_FOUND = 23;    // High when can is detected
const int PIN_CAN_ARRIVED = 29;  // High when robot reached can
const int RESET_PIN = 10;//not confirmed yet

const int buzzerPin = 22;


// =============================================================
// Movement Variables
// =============================================================
const float DEFAULT_SPEED = 100;
const float HOMING_SPEED = 100;
const float TURN_ADJUSTMENT = 5.0;

// =============================================================
// Encoders
// =============================================================
Encoder FL_motor(20, 21);  // 1 Front Left
Encoder FR_motor(18, 19);  // 2 Front Right

// the mega only had 6 interrupt pins so im just using the front 2 encoders for the values otherwise it isnt accurate
Encoder BL_motor(20, 21);  // 3 Back Left
Encoder BR_motor(18, 19);  // 4 Back Right




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
      digitalWrite(motorPinA, LOW);
      digitalWrite(motorPinB, HIGH);
    } else if (speed < 0) {
      digitalWrite(motorPinA, HIGH);
      digitalWrite(motorPinB, LOW);
    }

    // speed = constrain(speed, -255, 255);
    // if (speed > 0) {
    //   analogWrite(motorPinA, speed);
    //   analogWrite(motorPinB, 0);
    // } else if (speed < 0) {
    //   analogWrite(motorPinA, 0);
    //   analogWrite(motorPinB, -speed);
    // }
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
  int buzzer;
  int reset;


  float degreesToTurnDistance(float degrees) {
    // For differential drive: turning distance = (wheelbase * PI * degrees) / 360
    return (WHEELBASE * M_PI * abs(degrees)) / 360.0;
  }

public:
  Robot(
    Odometry* lf, Odometry* rf,
    Odometry* lb, Odometry* rb,
    int buz) 
    {
    leftFront = lf;
    rightFront = rf;
    leftBack = lb;
    rightBack = rb;
    buzzer = buz;
    pinMode(buzzer, OUTPUT);
  }

  typedef enum {
    MOVING_FORWARD,
    TURNING_LEFT,
    TURNING_RIGHT,
    IDLE
  } MoveState;
  MoveState moveState;


  // Update all motors
  void update() {
    leftFront->update();
    rightFront->update();
    leftBack->update();
    rightBack->update();
  }

  // =============================================================
  // Movement Functions
  // =============================================================
  void moveForward(float distanceMM, int speed = DEFAULT_SPEED) {
    // All wheels forward
    leftFront->startMoveForward(distanceMM, speed);
    rightFront->startMoveForward(distanceMM, speed);
    leftBack->startMoveForward(distanceMM, speed);
    rightBack->startMoveForward(distanceMM, speed);

    moveState = MOVING_FORWARD;
  }

  void moveBackward(float distanceMM, int speed = DEFAULT_SPEED) {
    // All wheels backward
    leftFront->startMoveBackward(distanceMM, speed);
    rightFront->startMoveBackward(distanceMM, speed);
    leftBack->startMoveBackward(distanceMM, speed);
    rightBack->startMoveBackward(distanceMM, speed);
  }

  // Left wheels backward, right wheels forward
  void turnLeft(float degrees, int speed = DEFAULT_SPEED) {
    // needs converting to distance as move forward uses distacce not degrees
    float turnDistance = degreesToTurnDistance(degrees);

    leftFront->startMoveBackward(turnDistance, speed);
    leftBack->startMoveBackward(turnDistance, speed);
    rightFront->startMoveForward(turnDistance, speed);
    rightBack->startMoveForward(turnDistance, speed);
    moveState = TURNING_LEFT;
  }

  // Left wheels forward, right wheels backward
  void turnRight(float degrees, int speed = DEFAULT_SPEED) {
    // needs converting to distance as move forward uses distacce not degrees
    float turnDistance = degreesToTurnDistance(degrees);

    leftFront->startMoveForward(turnDistance, speed);
    leftBack->startMoveForward(turnDistance, speed);
    rightFront->startMoveBackward(turnDistance, speed);
    rightBack->startMoveBackward(turnDistance, speed);
    moveState = TURNING_RIGHT;
  }

  void stopAll() {
    leftFront->stopMovement();
    rightFront->stopMovement();
    leftBack->stopMovement();
    rightBack->stopMovement();
    moveState = IDLE;
  }

  // =============================================================
  // Homing Functions
  // =============================================================
  void adjustLeft(int speed = HOMING_SPEED) {

    leftFront->driveMotor(-100);
    leftBack->driveMotor(-100);
    rightFront->driveMotor(100);
    rightBack->driveMotor(100);
    moveState = TURNING_LEFT;
  }

  void adjustRight(int speed = HOMING_SPEED) {
    leftFront->driveMotor(100);
    leftBack->driveMotor(100);
    rightFront->driveMotor(-100);
    rightBack->driveMotor(-100);
    moveState = TURNING_RIGHT;
  }

  void adjustForward(int speed = HOMING_SPEED) {
    leftFront->driveMotor(100);
    leftBack->driveMotor(100);
    rightFront->driveMotor(100);
    rightBack->driveMotor(100);
    moveState = TURNING_RIGHT;
  }

  void moveForwardHoming(float distanceMM = 200.0, int speed = HOMING_SPEED) {
    // Move forward slowly for homing
    moveForward(distanceMM, speed);
    moveState = MOVING_FORWARD;
  }

  void searchingRight(int speed = HOMING_SPEED) {
    leftFront->driveMotor(100);
    leftBack->driveMotor(100);
    rightBack->driveMotor(-100);

    moveState = TURNING_RIGHT;
  }

  void searchingLeft(int speed = HOMING_SPEED) {
    rightFront->driveMotor(100);
    rightBack->driveMotor(100);
    leftBack->driveMotor(-100);

    moveState = TURNING_RIGHT;
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

  bool isResetPin(){
    return digitalRead(RESET_PIN) == HIGH;
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

  MoveState getMoveState() {
    return moveState;
  }

  void startBuzzer(){
    digitalWrite(buzzer, HIGH);
  }

  void stopBuzzer(){
    digitalWrite(buzzer, LOW);
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

class FSM {
private:
  RobotState currentState;
  Robot* robot;
  unsigned long stateStartTime;
  unsigned long lastSensorCheck;

public:
  FSM(Robot* dr) {
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
    robot->update();

    checkSensors();


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
    // if (robot->isResetPin()){
    //   currentState = STATE_SEARCHING;
    //   robot->stopAll();
    //   delay(1000);S
    // }

    // Only check for can detection in SEARCHING state
    if (currentState == STATE_SEARCHING && robot->isCanDetected()) {
      Serial.println("FSM: Can detected! Switching to HOMING");
      // currentState = STATE_SEARCHING;
      currentState = STATE_HOMING;
      stateStartTime = millis();
      robot->stopAll();  // Stop current movements
    }

    // Check for arrival in HOMING state
    if (currentState == STATE_HOMING && robot->isCanArrived() && robot->isCanDetected()) {
      Serial.println("FSM: Can reached! Switching to ARRIVED");
      currentState = STATE_ARRIVED;
      stateStartTime = millis();
      robot->stopAll();
    }
  }

  void updateSearching() {
      // This is where i need to implement the search pattern
      static unsigned long lastActionTime = 0;
      static int searchStep = 0;
      static bool isScanning = false;
      static unsigned long scanStartTime = 0;
      
      unsigned long currentTime = millis();
      
      // Only proceed if robot is not currently moving
      if (robot->isBusy()) {
          return;
      }
      
      switch (searchStep) {
          case 0: // Step 1: Move forward for 2 seconds
              if (!isScanning) {
                  Serial.println("SEARCHING: Moving forward");
                  robot->adjustForward(HOMING_SPEED);
                  isScanning = true;
                  lastActionTime = currentTime;
              } else if (currentTime - lastActionTime >= 2000) { // Adjust time as needed
                  robot->stopAll();
                  isScanning = false;
                  searchStep = 1;
                  lastActionTime = currentTime;
              }
              break;
              
          case 1: // Step 2: Scan left
              if (!isScanning) {
                  Serial.println("SEARCHING: Scanning left");
                  robot->searchingLeft(HOMING_SPEED);
                  isScanning = true;
                  scanStartTime = currentTime;
              } else if (currentTime - scanStartTime >= 4000) { // Scan for 0.5 seconds
                  robot->stopAll();
                  isScanning = false;
                  searchStep = 2;
                  lastActionTime = currentTime;
              }
              break;
              
          case 2: // Step 3: Scan right
              if (!isScanning) {
                  Serial.println("SEARCHING: Scanning right");
                  robot->searchingRight(HOMING_SPEED);
                  isScanning = true;
                  scanStartTime = currentTime;
              } else if (currentTime - scanStartTime >= 8000) { // Scan right for 1 second (longer to return to center)
                  robot->stopAll();
                  isScanning = false;
                  searchStep = 0; // Reset to step 0 to move forward again
                  lastActionTime = currentTime;
              }
              break;
      }
      
      // Always check for can while searching
      if (robot->isCanDetected()) {
          Serial.println("SEARCHING: Can detected during search!");
          robot->stopAll();
          // The FSM will transition to HOMING state in checkSensors()
      }
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
    // if (robot->isCanArrived()) {
    //   Serial.println("FSM: Can reached! Switching to ARRIVED");
    //   currentState = STATE_ARRIVED;
    //   stateStartTime = millis();
    //   robot->stopAll();
    //   return;
    // }

    // Only adjust if robot is not currently moving
    bool canLeft = robot->isCanLeft();
    bool canRight = robot->isCanRight();
    if (!robot->isBusy()) {

      if (canLeft && !canRight) {
        // Can is to the left - turn left while left pin is high
        Serial.println("HOMING: Can is LEFT - turning left");
        robot->adjustLeft(HOMING_SPEED);
      } else if (!canLeft && canRight) {
        // Can is to the right - turn right while right pin is high
        Serial.println("HOMING: Can is RIGHT - turning right");
        robot->adjustRight(HOMING_SPEED);
      } else if (canLeft == false && canRight == false) {
        // Can is centered - move forward
        Serial.println("HOMING: Can is CENTERED - moving forward");
        robot->adjustForward(100);
      }
    } else if ((robot->getMoveState() == Robot::TURNING_LEFT && !canLeft)) {
      robot->stopAll();
    } else if ((robot->getMoveState() == Robot::TURNING_RIGHT && !canRight)) {
      robot->stopAll();
    } else if ((robot->getMoveState() == Robot::MOVING_FORWARD && (canLeft || canRight))) {
      robot->stopAll();
    }
  }

  void updateArrived() {
    // In ARRIVED state, all motors are stopped
    Serial.println("ARRIVED: At can location.");
    robot->startBuzzer();
    robot->stopAll();
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
// Initialize individual odometry modules
Odometry odomFL = Odometry(motor3a, motor3b, &FL_motor);
Odometry odomFR = Odometry(motor4a, motor4b, &FR_motor);
Odometry odomBR = Odometry(motor1a, motor1b, &BR_motor);
Odometry odomBL = Odometry(motor2a, motor2b, &BL_motor);

// Initialize differential drive robot controller
Robot robot = Robot(&odomFL, &odomFR, &odomBL, &odomBR, buzzerPin);

// Initialize finite state machine
FSM fsm = FSM(&robot);

// =============================================================
// Setup and Loop
// =============================================================
void setup() {
  Serial.begin(9600);
  Serial.println("=== Robot Initialization ===");


  Serial.println("Robot initialized.");
  Serial.println("Starting in SEARCHING state.");
  Serial.println("=============================");
}

void loop() {

  // when fsm->update() is called it updates all the logic in robot and motors
  // fsm->update() -> robot->update(); -> if (leftFront) leftFront->update(); and all the other motors
  fsm.update();
  // fsm.manualMoveForward(100);
  // fsm.manualTurnLeft(10);
  // fsm.manualTurnRight(10);

  delay(100);
}