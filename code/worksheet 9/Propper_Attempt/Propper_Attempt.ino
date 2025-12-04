#include <cppQueue.h>
#include <pio_encoder.h>

bool hasRun = false;

// Motor Variables //
#define L_MOTOR_A 11  // PWM
#define L_MOTOR_B 10  // DIR

#define R_MOTOR_A 12  // PWM
#define R_MOTOR_B 13  // DIR

// Encoders
PioEncoder L_encoder(18);
PioEncoder R_encoder(20);

float ticksPerDegree = (63 * 46) / 360;
float distancePerDegree = 0.68;  // mm

// =============================================================
// Grid Setup
// =============================================================
const int rows = 5;
const int cols = 5;

int grid[rows][cols] = {
  {0, 0, 0, 1, 0},
  {0, 1, 0, 1, 0},
  {0, 1, 0, 0, 0},
  {0, 0, 0, 1, 0},
  {0, 1, 0, 0, 0}
};

// =============================================================
// Movement Instructions for Robot
// =============================================================
enum Movement {
  TURN_RIGHT,
  TURN_LEFT,
  GO_STRAIGHT,
  TURN_AROUND,
  STOP
};

struct Instruction {
  Movement action;
  int steps;  // For GO_STRAIGHT: number of cells to move
};

// Maximum possible path length
const int MAX_INSTRUCTIONS = rows * cols * 2;
Instruction robotInstructions[MAX_INSTRUCTIONS];
int instructionCount = 0;

// Current direction robot is facing (0=up, 1=right, 2=down, 3=left)
int currentDirection = 1;  // Start facing right

// =============================================================
// Shared Arrays for Pathfinding
// =============================================================
bool visited[rows][cols];
int prevCell[rows][cols][2];
int distCost[rows][cols];  // g-cost
int fCost[rows][cols];     // f = g + h

// =============================================================
// BFS Queue
// =============================================================
struct Node {
  int row;
  int col;
};

cppQueue bfsQueue(sizeof(Node), 50, FIFO);

// =============================================================
// Priority Queue for Dijkstra + A*
// =============================================================
struct PQItem {
  int row;
  int col;
  int cost;  // g for Dijkstra, f for A*
};

PQItem pq[100];
int pqSize = 0;

void pqPush(PQItem item) {
  pq[pqSize++] = item;
}

PQItem pqPopMin() {
  int best = 0;
  for (int i = 1; i < pqSize; i++) {
    if (pq[i].cost < pq[best].cost)
      best = i;
  }
  PQItem out = pq[best];
  pqSize--;
  pq[best] = pq[pqSize];
  return out;
}

bool pqEmpty() {
  return pqSize == 0;
}

// =============================================================
// Utilities
// =============================================================
void resetArrays() {
  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      visited[r][c] = false;
      prevCell[r][c][0] = -1;
      prevCell[r][c][1] = -1;
      distCost[r][c] = 9999;
      fCost[r][c] = 9999;
    }
  }
  // Clear the robot instructions
  instructionCount = 0;
  currentDirection = 1;  // Reset to facing right
}

// =============================================================
// Calculate direction from two points
// =============================================================
int calculateDirection(int dr, int dc) {
  if (dr == -1 && dc == 0) return 0;   // Up
  if (dr == 0 && dc == 1) return 1;    // Right
  if (dr == 1 && dc == 0) return 2;    // Down
  if (dr == 0 && dc == -1) return 3;   // Left
  return 1;  // Default to right
}

// =============================================================
// Calculate turn needed based on current and target direction
// =============================================================
Movement calculateTurn(int fromDir, int toDir) {
  if (fromDir == toDir) return GO_STRAIGHT;
  
  int diff = (toDir - fromDir + 4) % 4;
  
  if (diff == 1) return TURN_RIGHT;
  if (diff == 3) return TURN_LEFT;
  if (diff == 2) return TURN_AROUND;
  
  return GO_STRAIGHT;
}

// =============================================================
// Generate Movement Instructions
// =============================================================
bool generateMovementInstructions(int startR, int startC, int endR, int endC) {
  instructionCount = 0;
  
  // First, reconstruct the path from end to start
  int path[MAX_INSTRUCTIONS][2];
  int pathLength = 0;
  
  int currentR = endR;
  int currentC = endC;
  
  // Backtrack to build path in reverse
  while (currentR != -1 && currentC != -1) {
    path[pathLength][0] = currentR;
    path[pathLength][1] = currentC;
    pathLength++;
    
    if (prevCell[currentR][currentC][0] == -1 && 
        prevCell[currentR][currentC][1] == -1) {
      break;
    }
    
    int nextR = prevCell[currentR][currentC][0];
    int nextC = prevCell[currentR][currentC][1];
    currentR = nextR;
    currentC = nextC;
  }
  
  // Reverse path to get start-to-end order
  for (int i = 0; i < pathLength / 2; i++) {
    int tempR = path[i][0];
    int tempC = path[i][1];
    path[i][0] = path[pathLength - 1 - i][0];
    path[i][1] = path[pathLength - 1 - i][1];
    path[pathLength - 1 - i][0] = tempR;
    path[pathLength - 1 - i][1] = tempC;
  }
  
  // Now generate movement instructions
  int currentDir = currentDirection;  // Start with initial facing direction
  
  for (int i = 0; i < pathLength - 1; i++) {
    int dr = path[i + 1][0] - path[i][0];
    int dc = path[i + 1][1] - path[i][1];
    
    int targetDir = calculateDirection(dr, dc);
    
    // Add turn instruction if needed
    Movement turn = calculateTurn(currentDir, targetDir);
    if (turn != GO_STRAIGHT) {
      if (instructionCount < MAX_INSTRUCTIONS) {
        robotInstructions[instructionCount].action = turn;
        robotInstructions[instructionCount].steps = 0;
        instructionCount++;
      }
    }
    
    // Add forward movement (always move 1 cell forward)
    if (instructionCount < MAX_INSTRUCTIONS) {
      robotInstructions[instructionCount].action = GO_STRAIGHT;
      robotInstructions[instructionCount].steps = 1;
      instructionCount++;
    }
    
    // Update current direction
    currentDir = targetDir;
  }
  
  // Add stop instruction at the end
  if (instructionCount < MAX_INSTRUCTIONS) {
    robotInstructions[instructionCount].action = STOP;
    robotInstructions[instructionCount].steps = 0;
    instructionCount++;
  }
  
  return (instructionCount > 0);
}

// =============================================================
// Print Movement Instructions
// =============================================================
void printMovementInstructions() {
  Serial.println("=== Robot Movement Instructions ===");
  Serial.print("Instruction count: ");
  Serial.println(instructionCount);
  
  for (int i = 0; i < instructionCount; i++) {
    Serial.print("Step ");
    Serial.print(i + 1);
    Serial.print(": ");
    
    switch (robotInstructions[i].action) {
      case TURN_RIGHT:
        Serial.print("TURN RIGHT");
        break;
      case TURN_LEFT:
        Serial.print("TURN LEFT");
        break;
      case GO_STRAIGHT:
        Serial.print("GO STRAIGHT ");
        Serial.print(robotInstructions[i].steps);
        Serial.print(" cell(s)");
        break;
      case TURN_AROUND:
        Serial.print("TURN AROUND (180°)");
        break;
      case STOP:
        Serial.print("STOP - Destination reached");
        break;
    }
    Serial.println();
  }
  Serial.println("===================================");
}

// =============================================================
// BFS with Movement Instruction Generation
// =============================================================
bool runBFS(int startR, int startC, int endR, int endC) {
  resetArrays();

  Node start = {startR, startC};
  bfsQueue.push(&start);
  visited[startR][startC] = true;

  int dr[4] = {0, 1, 0, -1};
  int dc[4] = {1, 0, -1, 0};

  while (!bfsQueue.isEmpty()) {
    Node u;
    bfsQueue.pop(&u);

    if (u.row == endR && u.col == endC) {
      Serial.println("BFS Path Found!");
      
      // Generate movement instructions
      if (generateMovementInstructions(startR, startC, endR, endC)) {
        printMovementInstructions();
        return true;
      }
      return false;
    }

    for (int i = 0; i < 4; i++) {
      int nr = u.row + dr[i];
      int nc = u.col + dc[i];

      if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
        if (!visited[nr][nc] && grid[nr][nc] == 0) {
          visited[nr][nc] = true;

          prevCell[nr][nc][0] = u.row;
          prevCell[nr][nc][1] = u.col;

          Node nxt = {nr, nc};
          bfsQueue.push(&nxt);
        }
      }
    }
  }

  Serial.println("BFS: No path found");
  return false;
}

// =============================================================
// Execute Robot Instructions in a for loop
// =============================================================
void executeRobotInstructions() {
  if (instructionCount == 0) {
    Serial.println("No instructions to execute!");
    return;
  }

  Serial.println("Executing robot instructions...");
  
  for (int i = 0; i < instructionCount; i++) {
    Serial.print("Executing instruction ");
    Serial.print(i + 1);
    Serial.print(": ");
    
    switch (robotInstructions[i].action) {
      case TURN_RIGHT:
        Serial.println("Turning right 90 degrees");
        turnRight();
        break;
        
      case TURN_LEFT:
        Serial.println("Turning left 90 degrees");
        turnLeft();
        break;
        
      case GO_STRAIGHT:
        for (int j = 0; i < robotInstructions[i].steps; j++) {
          Serial.println("Going forward 1 step");

          moveStraight();
        }
        break;
        
      case TURN_AROUND:
        Serial.println("Turning around 180 degrees");
        turnLeft();
        turnLeft();
        break;
        
      case STOP:
       stopMotors();
        break;
    }
    
    // Update current direction based on action
    if (robotInstructions[i].action == TURN_RIGHT) {
      currentDirection = (currentDirection + 1) % 4;
    } else if (robotInstructions[i].action == TURN_LEFT) {
      currentDirection = (currentDirection + 3) % 4;
    } else if (robotInstructions[i].action == TURN_AROUND) {
      currentDirection = (currentDirection + 2) % 4;
    }
    
    delay(1000);
  }
  
  Serial.println("Instruction execution complete!");
}

// =============================================================
// Motor Control Functions
// =============================================================


void moveStraight() {
  int distance = 200;
  long L_pos = L_encoder.getCount() / ticksPerDegree * distancePerDegree;
  long R_pos = R_encoder.getCount() / ticksPerDegree * distancePerDegree;

  int L_adjDistance = distance += L_pos;
  int R_adjDistance = distance += R_pos;

  while (L_pos < L_adjDistance || R_pos < R_adjDistance){
    L_pos = L_encoder.getCount() / ticksPerDegree * distancePerDegree;
    R_pos = R_encoder.getCount() / ticksPerDegree * distancePerDegree;

    Serial.print("L_pos stright: ");
    Serial.println(L_pos);

    Serial.print("L_adjDistance stright: ");
    Serial.println(L_adjDistance);

    analogWrite(L_MOTOR_A, 100);
    analogWrite(L_MOTOR_B, 0);

    analogWrite(R_MOTOR_A, 100);
    analogWrite(R_MOTOR_B, 0);
  }
  stopMotors();


}

void stopMotors() {
    analogWrite(L_MOTOR_A, 0);
    analogWrite(L_MOTOR_B, 0);

    analogWrite(R_MOTOR_A, 0);
    analogWrite(R_MOTOR_B, 0);
}

void turnLeft(){
  int distance = 90;
  long L_pos = L_encoder.getCount() / ticksPerDegree * distancePerDegree;
  long R_pos = R_encoder.getCount() / ticksPerDegree * distancePerDegree;

  int L_adjDistance = L_pos += distance;
  int R_adjDistance = R_pos -= distance;

  while (L_pos < L_adjDistance || R_pos > R_adjDistance){
    L_pos = L_encoder.getCount() / ticksPerDegree * distancePerDegree;
    R_pos = R_encoder.getCount() / ticksPerDegree * distancePerDegree;

    Serial.print("L_pos left: ");
    Serial.println(L_pos);

    Serial.print("R_pos left: ");
    Serial.println(R_pos);

    analogWrite(L_MOTOR_A, 100);
    analogWrite(L_MOTOR_B, 0);

    analogWrite(R_MOTOR_A, 0);
    analogWrite(R_MOTOR_B, 100);
  }
  stopMotors();


}

void turnRight(){
  int distance = 90;
  long L_pos = L_encoder.getCount() / ticksPerDegree * distancePerDegree;
  long R_pos = R_encoder.getCount() / ticksPerDegree * distancePerDegree;

  int L_adjDistance = L_pos -= distance;
  int R_adjDistance = R_pos += distance;

  while (L_pos > L_adjDistance || R_pos < R_adjDistance){
    L_pos = L_encoder.getCount() / ticksPerDegree * distancePerDegree;
    R_pos = R_encoder.getCount() / ticksPerDegree * distancePerDegree;

    Serial.print("L_pos right: ");
    Serial.println(L_pos);

    Serial.print("R_pos right: ");
    Serial.println(R_pos);

    analogWrite(L_MOTOR_A, 0);
    analogWrite(L_MOTOR_B, 100);

    analogWrite(R_MOTOR_A, 100);
    analogWrite(R_MOTOR_B, 0);
  }
  stopMotors();


}

// =============================================================
// Arduino Setup
// =============================================================
void setup() {
  Serial.begin(9600);
  delay(500);

  pinMode(L_MOTOR_A, OUTPUT);
  pinMode(L_MOTOR_B, OUTPUT);
  pinMode(L_MOTOR_A, OUTPUT);
  pinMode(L_MOTOR_B, OUTPUT);

  R_encoder.begin();
  R_encoder.flip();
  
  L_encoder.begin();
  L_encoder.flip();

  while (!Serial);
  Serial.println("Running Pathfinder with Movement Instructions");
  
  // Run BFS and generate movement instructions
  bool pathFound = runBFS(0, 0, 4, 4);
  
  if (pathFound) {
    Serial.println("\nMovement instructions are ready!");
    
    executeRobotInstructions();
  }
}

void loop() {
  // Your existing encoder reading code
  long L_pos = L_encoder.getCount() / ticksPerDegree * distancePerDegree;
  long R_pos = R_encoder.getCount() / ticksPerDegree * distancePerDegree;
  
  
  delay(100);
}