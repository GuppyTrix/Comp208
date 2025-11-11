#include <Encoder.h>
//PID DECLARATION
float PIDL();
float PIDR();
void forward();
void stopMotors();
void right();
void left();
//odometry
float odometry();
int axisWidth;
//encoded readings
float rPrev, lPrev,hPrev;
float x,y;
float rad;
//top right wheel
int encoderA = 3;
int encoderB = 2;
int motorA = 8;
int motorB = 9;
//top left wheel
int encoderC = 5;
int encoderD = 6;
int motorC = 10;
int motorD = 11;
//PID CONTROL SYSTEMS
float proportional_gain = 5.0;
float Integral_gain = 0.1;
float Derivative_gain = 0.5;
//variables
unsigned long now;
float errorL = 0;
float lastErrorL = 0;
float integralL = 0;
float derivativeL = 0;
float outputL = 0;

float errorR = 0;
float lastErrorR = 0;
float integralR = 0;
float derivativeR = 0;
float outputR = 0;

unsigned long lastTimeL = 0;
unsigned long lastTimeR = 0;
float setpointL = 0;
float setpointR = 0;
float measured_valueL = 0;
float measured_valueR = 0;
float controller_outputL;
float controller_outputR;
float ticksPerDegree = (44.00 * 193.00) / (360.00 / 3.00); 

int step = 0; // movement phase tracker

//encoders
Encoder myEncoderL(encoderC, encoderD);
Encoder myEncoderR(encoderA, encoderB);

void control_left()
{
  float controlsL = PIDL();
  int pulseL = constrain(controlsL, -255, 255);
  // Motor control logic
  if (pulseL > 0)
  {
    analogWrite(motorC, pulseL);
    analogWrite(motorD, 0);
  }
  else
  {
    analogWrite(motorC, 0);
    analogWrite(motorD, abs(pulseL));
  }
}

void control_right()
{
  float controlsR = PIDR();
  int pulseR = constrain(controlsR, -255, 255);
  if (pulseR > 0)
  {
    analogWrite(motorA, pulseR);
    analogWrite(motorB, 0);
  }
  else
  {
    analogWrite(motorA, 0);
    analogWrite(motorB, abs(pulseR));
  }
}

float PIDL()
{
  now = millis();
  measured_valueL = myEncoderL.read() / ticksPerDegree;
  float error = setpointL - measured_valueL;
  float difference = (now - lastTimeL) / 1000.0;
  if (difference <= 0) difference = 0.0001;

  integralL += error * difference;
  derivativeL = (error - lastErrorL) / difference;
  controller_outputL = (proportional_gain * error) + (Integral_gain * integralL) + (Derivative_gain * derivativeL);

  lastErrorL = error;
  lastTimeL = now;
  return controller_outputL;
}

float PIDR()
{
  now = millis();
  measured_valueR = myEncoderR.read() / ticksPerDegree;
  float error = setpointR - measured_valueR;
  float difference = (now - lastTimeR) / 1000.0;
  if (difference <= 0) difference = 0.0001;

  integralR += error * difference;
  derivativeR = (error - lastErrorR) / difference;
  controller_outputR = (proportional_gain * error) + (Integral_gain * integralR) + (Derivative_gain * derivativeR);

  lastErrorR = error;
  lastTimeR = now;
  return controller_outputR;
}

void forward()
{
  // Move both wheels forward by 90 degrees turns
  myEncoderL.write(0);
  myEncoderR.write(0);
  setpointL = 90;
  setpointR = 90;
  step = 1;
}

void stopMotors()
{
  analogWrite(motorA, 0);
  analogWrite(motorB, 0);
  analogWrite(motorC, 0);
  analogWrite(motorD, 0);
}

void right()
{
  // Spin right: left forward, right backward
  myEncoderL.write(0);
  myEncoderR.write(0);
  setpointL = 90;
  setpointR = -90;
  step = 2;
}

void left()
{
  // Spin left: left backward, right forward
  myEncoderL.write(0);
  myEncoderR.write(0);
  setpointL = -90;
  setpointR = 90;
  step = 3;
}
float odometry()
{
  float l = myEncoderL.read() / ticksPerDegree;
  float r = myEncoderR.read() / ticksPerDegree;
  //I need to define l and r as and right encoder motors but unsure which definition to use.
  float h = hPrev;
  // calculate encoder deltas (differences from this and previous readings)
  float lDelta = l - lPrev;
  float rDelta = r - rPrev;
  // calculate omega
  float hDelta = (rDelta - lDelta) / axisWidth;
  // either approximate if we're going (almost) straight or calculate an arc
  if (abs(lDelta - rDelta) < 1e-5)
  { // 1e-5 = 1 × 10^5 and is equal to 100000
    x += lDelta * cos(h);
    y += rDelta * sin(h);
  } else 
  {
    // calculate the radius of ICC (Instantaneous Centre of Curvature – the point around which the robot is turning)
    // The ICC is marked in fig.3 by the rotation point of angle = delta theta (Δθ).
    rad = (axisWidth / 2) * (rDelta + lDelta) / (rDelta - lDelta);
    // calculate the robot position by finding a point that is rotated
    // around ICC by heading delta
    x += rad * sin(hDelta + h) - rad * sin(h);
    y += - rad * cos(hDelta + h) + rad * cos(h);
  }
  lPrev = l;
  rPrev = r;
  hPrev = h + hDelta;
  
  // Print values for x and y
  Serial.print("x: ");
  Serial.print(x);
  Serial.print(" y: ");
  Serial.println(y);
}
void setup()
{
  Serial.begin(9600);
  lastTimeL = millis();
  lastTimeR = millis();
  rPrev,lPrev,hPrev,x,y = 0;
  axisWidth = 20;
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  pinMode(motorC, OUTPUT);
  pinMode(motorD, OUTPUT);

  // start by moving forward
  forward();
}

void loop()
{
  control_left();
  control_right();

  // Debug
  Serial.print("L Setpoint: "); Serial.print(setpointL);
  Serial.print(", L Pos: "); Serial.print(measured_valueL);
  Serial.print(", R Setpoint: "); Serial.print(setpointR);
  Serial.print(", R Pos: "); Serial.print(measured_valueR);
  Serial.print(", L Out: "); Serial.print(controller_outputL);
  Serial.print(", R Out: "); Serial.println(controller_outputR);

  // Check if target reached
  if (abs(setpointL - measured_valueL) < 2 && abs(setpointR - measured_valueR) < 2)
  {
    stopMotors();
    delay(1000);

    if (step == 1) right();     // after forward, turn right
    else if (step == 2) forward(); // after right turn, move forward again
    else if (step == 3) stopMotors(); // after left turn, stop
  }

  //moving forward = true
  //if movingforward{forward function}else{stop function}
  delay(100);
}
