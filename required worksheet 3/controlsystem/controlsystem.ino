#include <Encoder.h>
enum DIRECTION_ENUM
{
  STOP,
  FORWARD,
  BACKWARD,
  RIGHT,
  LEFT,
  WAITING
};
enum SYSTEM_ENUM
{
  BANGBANG,
  P,
  PI,
  PID
};
int encoderA = 3;
int encoderB = 2;
int motorA = 6;
int motorB = 5;
int motorE = 11;

Encoder myEncoder(encoderA, encoderB);

float pos = 0;
float setpoint = 0;
float error = 0;
float output = 0;
float measured_value = 0;
float ticksPerDegree = 44 * 193 / 360 / 3;

// function for driving a motor from just one value between -255 and 255
void motorDrive(int power) {

  // check the input value is in the right range
  if (power > 255) {
    power = 255;
  }
  if (power < -255) {
    power = -255;
  }

  // set the h-bridge signals depending on the sign of the input value
  if (power == 0) {
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, LOW);
  }
  if (power > 0) {
    digitalWrite(motorA, HIGH);
    digitalWrite(motorB, LOW);
  }
  if (power < 0) {
    digitalWrite(motorA, LOW);
    digitalWrite(motorB, HIGH);
  }

  // write the motor power to the enable pin
  analogWrite(motorE, abs(power));
}
 void BANGBANG(float setpoint, float measured_value)
 {
  measured_value = analog_read(pos);
  if(setpoint > measured_value)
  {
    motorDrive(255);
  }
  else if(measured_value > setpoint)
  {
    motorDrive(-255);
  }
 }
void setup() {
  // set the pin modes of the motor pins and encoder pins
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  pinMode(motorE, OUTPUT);
  pinMode(encoderA, INPUT);
  pinMode(encoderB, INPUT);

  Serial.begin(115200);

  // quick motor test to make sure the 'positive' direction of the motor matches the encoder
  // to change the direction of either the motor or encoder, simply swap the two signal wires
  motorDrive(255);
  delay(200);
  motorDrive(0);
  delay(100);
}

void loop()
{
  // convert the encoder ticks to degrees
  pos = myEncoder.read() / ticksPerDegree;
measured_value = pos
  // update the desired setpoint of the motor from serial
  if (Serial.available() > 1)
  {
    setpoint = Serial.parseInt();
  }
  // ----------WRITE CONTROLLER HERE--------------------------------
 // You will need to write your Bang Bang and PID controller code here
 // Remember you will need to create variables for Kp, Ki and Kd (Your gain values to tune)
  motorDrive(output);

  // ----------------------------------------------------------------

  //  make the control loop run ten times per second
  delay(100);
BANGBANG(float setpoint,float measured_value);
  //  print some debug info, check it out in the serial plotter
  Serial.print("Setpoint:"); Serial.print(setpoint); Serial.print(", ");
  Serial.print("Position:"); Serial.print(pos); Serial.print(", ");
  Serial.print("Error:"); Serial.print(error); Serial.print(", ");
  Serial.print("OutputPower:"); Serial.print(output); Serial.print(", ");
  Serial.println();
}
}