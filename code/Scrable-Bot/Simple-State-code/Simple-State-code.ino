

// --- ENUMS ---
enum Movement_Enum {
  STOP,
  FORWARD,
  BACKWARDS,
  TURNING_RIGHT,
  TURNING_LEFT,
};

enum State_enum {
  // The robot is Not doing anything
  IDLE,
  // The robot is in the search algorythm state where it is looking for a red can
  SEARCHING,
  // The robot has spotted a red object
  FOUND,
  // The Robot is driving towards the 
  HOMING,
  // The robot has arrived at what it has spotted
  ARRIVED
};

State_enum Current_Behaviour;
Movement_Enum Current_Movement;

void setup() {
  Serial.begin(9600);
  Current_State = IDLE;
  Serial.println("Robot Initialised");
}


//TO-DO//
/*
the robot starts of idle until everythin is initialised
the robot starts its search algorithm
if the robot finds anything it starts hominh towards it
once the robot reaches the object  it beeps to confirm it met its destination

*/
void loop() {

}

void StateMachine(){
  switch (Current_State) {
    // A state for the robot to do nothing mainly for debuging or for waiting for button input to start the state machine.
    case IDLE:
    break;

    //the robot should implement a search algorithm that explores an X*Y area and attempts to find a coke can
    case SEARCHING:
    break;

    // should enter this State once the robot has found a Red object/ Coke can
    case FOUND:
    break:

    // once the can has been found the robot will start moving towards the object until it has arrived at its destination
    case HOMING:
    break:
    
    // Once arrived it should give some sort of feedback to the user to confirm it thinks it has arrived
    case ARRIVED:
    break:    
  }
}


