#include <cppQueue.h>
#include <pio_encoder.h>
#include <math.h>
// =============================================================
// Grid Setup
// =============================================================
// Motor pins
#define L_MOTOR_A   11   // PWM
#define L_MOTOR_B   10   // DIR
#define R_MOTOR_A   12   // PWM
#define R_MOTOR_B   13   // DIR

// Encoders
PioEncoder l_encoder(18);
PioEncoder r_encoder(20);

// Odometry
float axisWidth = 120.0;  
float TICKS_TO_MM = 0.5;

// Robot pose
float x = 0;
float y = 0;
float heading = 0;

long lPrev = 0;
long rPrev = 0;

float totaldistance = 0;

// Motor speeds
float Lspeed = 200;
float Rspeed = 200;
void stopRobot()
{
  digitalWrite(L_MOTOR_A, LOW);
  digitalWrite(L_MOTOR_B, LOW);
  digitalWrite(R_MOTOR_A, LOW);
  digitalWrite(R_MOTOR_B, LOW);

  Serial.println("Reached 1 metre, stopping.");
  while (1);
}
void not_loop()
{
  long lNow = l_encoder.getCount();
  long rNow = r_encoder.getCount();

  long lTicks = lNow - lPrev;
  long rTicks = rNow - rPrev;


  float lDist = lTicks * TICKS_TO_MM;
  float rDist = rTicks * TICKS_TO_MM;

  float d = (lDist + rDist) / 2.0;
  float dTheta = (rDist - lDist) / axisWidth;

  float hMid = heading + dTheta / 2.0;

  x += d * cos(hMid);
  y += d * sin(hMid);
  heading += dTheta;

  lPrev = lNow;
  rPrev = rNow;

  totaldistance += fabs(d);
  Serial.println(totaldistance);
  delay(100);
}
const int rows = 5;
const int cols = 5;

int grid[rows][cols] =
{
  {0, 0, 0, 1, 0},
  {0, 1, 0, 1, 0},
  {0, 1, 0, 0, 0},
  {0, 0, 0, 1, 0},
  {0, 1, 0, 0, 0}
};

// =============================================================
// Shared Arrays
// =============================================================
bool visited[rows][cols];
int prevCell[rows][cols][2];
int distCost[rows][cols];   // g-cost
int fCost[rows][cols];      // f = g + h

// =============================================================
// BFS Queue
// =============================================================
struct Node
{
  int row;
  int col;
};

cppQueue bfsQueue(sizeof(Node), 50, FIFO);

// =============================================================
// Priority Queue for Dijkstra + A*
// =============================================================
struct PQItem
{
  int row;
  int col;
  int cost;   // g for Dijkstra, f for A*
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
// BFS
// =============================================================
void runBFS(int startR, int startC, int endR, int endC) {
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
      Serial.println("BFS Path:");
      printDirections(endR, endC);
      return;
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
}

// =============================================================
// Dijkstra
// =============================================================
void runDijkstra(int startR, int startC, int endR, int endC) {
  resetArrays();
  pqSize = 0;

  distCost[startR][startC] = 0;
  pqPush({startR, startC, 0});

  int dr[4] = {0, 1, 0, -1};
  int dc[4] = {1, 0, -1, 0};

  while (!pqEmpty()) {
    PQItem u = pqPopMin();

    if (u.row == endR && u.col == endC) {
      Serial.println("Dijkstra Path:");
      printDirections(endR, endC);
      return;
    }

    for (int i = 0; i < 4; i++) {
      int nr = u.row + dr[i];
      int nc = u.col + dc[i];

      if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
        if (grid[nr][nc] == 0) {

          int gNew = distCost[u.row][u.col] + 1;

          if (gNew < distCost[nr][nc]) {
            distCost[nr][nc] = gNew;

            prevCell[nr][nc][0] = u.row;
            prevCell[nr][nc][1] = u.col;

            pqPush({nr, nc, gNew});
          }
        }
      }
    }
  }

  Serial.println("Dijkstra: No path found");
}

// =============================================================
// A* Algorithm
// =============================================================
void runAStar(int startR, int startC, int endR, int endC) {
  resetArrays();
  pqSize = 0;

  distCost[startR][startC] = 0;
  fCost[startR][startC] = heuristic(startR, startC, endR, endC);

  pqPush({startR, startC, fCost[startR][startC]});

  int dr[4] = {0, 1, 0, -1};
  int dc[4] = {1, 0, -1, 0};

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

            pqPush({nr, nc, fNew});
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
void setup()
{
  delay(500);
  Serial.begin(115200);
  pinMode(L_MOTOR_A, OUTPUT);
  pinMode(L_MOTOR_B, OUTPUT);
  pinMode(R_MOTOR_A, OUTPUT);
  pinMode(R_MOTOR_B, OUTPUT);
  l_encoder.begin();
  r_encoder.begin();
  l_encoder.flip();
  r_encoder.flip();
  lPrev = l_encoder.getCount();
  rPrev = r_encoder.getCount();
  int startR = 0, startC = 0;
  int endR = 4, endC = 4;

  // Choose algorithm:
  // runBFS(startR, startC, endR, endC);
  // runDijkstra(startR, startC, endR, endC);
  runAStar(startR, startC, endR, endC);
}

void loop()
{
  not_loop();
}
