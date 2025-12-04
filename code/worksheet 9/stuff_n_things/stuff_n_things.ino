#include <pio_encoder.h>
#include <cppQueue.h>




// =============================================================
// Motor Setup
// ============================================================
#define LEFT_MOTOR_A 11   // PWM
#define LEFT_MOTOR_B 10   // DIR
#define RIGHT_MOTOR_A 12  // PWM
#define RIGHT_MOTOR_B 13  // DIR





// =============================================================
// Grid Setup
// ============================================================
const int rows = 5;
const int cols = 5;

int grid[rows][cols] = {
  { 0, 0, 0, 1, 0 },
  { 0, 1, 0, 1, 0 },
  { 0, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 0 }
};

// =============================================================
// Shared Arrays
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
}

void printDirections(int row, int col) {
  if (prevCell[row][col][0] != -1 || prevCell[row][col][1] != -1) {
    printDirections(prevCell[row][col][0], prevCell[row][col][1]);
  }

  if (row != 0 || col != 0) {
    Serial.print("Move to (");
    Serial.print(row);
    Serial.print(", ");
    Serial.print(col);
    Serial.print(") Direction: ");

    int pr = prevCell[row][col][0];
    int pc = prevCell[row][col][1];

    if (row == pr + 1) Serial.println("Down");
    else if (row == pr - 1) Serial.println("Up");
    else if (col == pc + 1) Serial.println("Right");
    else if (col == pc - 1) Serial.println("Left");
  }
}

// =============================================================
// Manhattan Heuristic for A*
// =============================================================
int heuristic(int r, int c, int goalR, int goalC) {
  return abs(r - goalR) + abs(c - goalC);
}


// =============================================================
// A* Algorithm
// =============================================================
void runAStar(int startR, int startC, int endR, int endC) {
  resetArrays();
  pqSize = 0;

  distCost[startR][startC] = 0;
  fCost[startR][startC] = heuristic(startR, startC, endR, endC);

  pqPush({ startR, startC, fCost[startR][startC] });

  int dr[4] = { 0, 1, 0, -1 };
  int dc[4] = { 1, 0, -1, 0 };

  while (!pqEmpty()) {
    PQItem u = pqPopMin();

    if (u.row == endR && u.col == endC) {
      Serial.println("A* Path:");
      printDirections(endR, endC);
      return;
    }

    for (int i = 0; i < 4; i++) {
      int nr = u.row + dr[i];
      int nc = u.col + dc[i];

      if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
        if (grid[nr][nc] == 0) {

          int gNew = distCost[u.row][u.col] + 1;
          int hNew = heuristic(nr, nc, endR, endC);
          int fNew = gNew + hNew;

          if (fNew < fCost[nr][nc]) {
            distCost[nr][nc] = gNew;
            fCost[nr][nc] = fNew;

            prevCell[nr][nc][0] = u.row;
            prevCell[nr][nc][1] = u.col;

            pqPush({ nr, nc, fNew });
          }
        }
      }
    }
  }

  Serial.println("A*: No path found");
}

// =============================================================
// Arduino Setup
// =============================================================
void setup() {
  Serial.begin(9600);
  delay(500);
  while (!Serial){}

  int startR = 0, startC = 0;
  int endR = 4, endC = 4;

  pinMode(LEFT_MOTOR_A, OUTPUT);
  pinMode(LEFT_MOTOR_B, OUTPUT);
  pinMode(RIGHT_MOTOR_A, OUTPUT);
  pinMode(RIGHT_MOTOR_B, OUTPUT);

  // Choose algorithm:
  // runBFS(startR, startC, endR, endC);
  // runDijkstra(startR, startC, endR, endC);
  runAStar(startR, startC, endR, endC);
}

void loop() {

}


void LEFT_CONTROL(int raw_input) {
  int motor_power = constrain(raw_input, -255, 255);
}
void RIGHT_CONTROL(int raw_input) {
  int motor_power = constrain(raw_input, -255, 255);
}

void FORWARD(int raw_input) {
  digitalWrite(RIGHT_MOTOR_A, HIGH);
  digitalWrite(LEFT_MOTOR_A, HIGH);
}
void STOP(int raw_input) {
  digitalWrite(RIGHT_MOTOR_A, LOW);
  digitalWrite(LEFT_MOTOR_A, LOW);
}
void LEFT(int raw_input, int motor_power) {
  digitalWrite(LEFT_MOTOR_B, LOW);
  analogWrite(LEFT_MOTOR_A, abs(motor_power));
  analogWrite(RIGHT_MOTOR_A, abs(motor_power));
  digitalWrite(RIGHT_MOTOR_B, LOW);
}
void RIGHT(int raw_input, int motor_power) {
  digitalWrite(RIGHT_MOTOR_A, LOW);
  analogWrite(RIGHT_MOTOR_B, abs(motor_power));
  analogWrite(LEFT_MOTOR_B, abs(motor_power));
  digitalWrite(LEFT_MOTOR_A, LOW);
}
void REVERSE(int raw_input) {
  // how Can I reverse dc motor on the back
}
void stuck(int raw_input) {
  // go do certain directions in a certain order.
}