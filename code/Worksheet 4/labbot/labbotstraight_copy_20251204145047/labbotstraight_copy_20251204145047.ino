
#include "pio_encoder.h"
#include <math.h>

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
float Lspeed = 200;   // left wheel slower
float Rspeed = 200;   // right wheel faster

void setup()
{
  Serial.begin(115200);

  pinMode(L_MOTOR_A, OUTPUT);
  pinMode(L_MOTOR_B, OUTPUT);
  pinMode(R_MOTOR_A, OUTPUT);
  pinMode(R_MOTOR_B, OUTPUT);

  l_encoder.begin();
  r_encoder.begin();
  l_encoder.flip();
  r_encoder.flip();

  // Drive forward
  analogWrite(L_MOTOR_A, Lspeed);
  digitalWrite(L_MOTOR_B, LOW);

  analogWrite(R_MOTOR_A, Rspeed);
  digitalWrite(R_MOTOR_B, LOW);

  lPrev = l_encoder.getCount();
  rPrev = r_encoder.getCount();
}

void stopRobot()
{
  digitalWrite(L_MOTOR_A, LOW);
  digitalWrite(L_MOTOR_B, LOW);
  digitalWrite(R_MOTOR_A, LOW);
  digitalWrite(R_MOTOR_B, LOW);

  Serial.println("Reached 1 metre, stopping.");
  while (1);
}

void loop()
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

  if (totaldistance >= 5940.48)
  {
    Serial.println("stop");
    
  }
  //1 rotation == 560 ticks

  delay(100);
}
