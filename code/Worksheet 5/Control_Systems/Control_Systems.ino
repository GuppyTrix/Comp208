#include <Encoder.h>

// Encode Variables //
int encoderA = 3;
int encoderB = 2;
int motorA = 9;
int motorB = 10;
int motorE = 11;

Encoder myEncoder(encoderA, encoderB);
// General Variables //
float pos = 0;
float setpoint = 0;
float error = 0;
float output = 0;
float measured_value = 0;
float controller_output = 0;
float proportional_gain = 2.0;   
float integral_gain = 0.5;       
float integral = 0;

float ticksPerDegree = (44 * 193) / (360 / 3);

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


void BANGBANG(float setpoint, float measured_value) {
  if (setpoint > measured_value) {
    motorDrive(-255);
  } else if (measured_value > setpoint) {
    motorDrive(255);
  }
}

void Proportional_Control(float setpoint, float measured_value) {
  float Kp = 1;  // Proportional gain
  error = setpoint - pos;  // Proportional term
  output = Kp * error;
  motorDrive(output);
}
void PI(float measured_value,float setpoint)
{
  error = setpoint - measured_value;
  integral += error * integral_gain;
  integral  = constrain(integral,-255,255);
  float proportional_term = error * proportional_gain;
  controller_output = proportional_term + integral;
  controller_output = constrain(controller_output,-255,255);
}
void setup() {
  // set the pin modes of the motor pins and encoder pins
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  pinMode(motorE, OUTPUT);
  pinMode(encoderA, INPUT);
  pinMode(encoderB, INPUT);

  Serial.begin(115200);

  delay(200);
}



void loop() {
  // convert the encoder ticks to degrees
  pos = myEncoder.read() / ticksPerDegree;
  measured_value = pos;

  // update the desired setpoint of the motor from serial
  if (Serial.available() > 1) {
    setpoint = Serial.parseInt();
  }

  //  make the control loop run ten times per second
  delay(100);

  // BANGBANG(setpoint, measured_value);
  Proportional_Control(setpoint, measured_value);

  //  print some debug info, check it out in the serial plotter
  Serial.print("Setpoint:");
  Serial.print(setpoint);
  Serial.print(", ");
  Serial.print("Position:");
  Serial.print(pos);
  Serial.print(", ");
  Serial.print("Error:");
  Serial.print(error);
  Serial.print(", ");
  Serial.print("OutputPower:");
  Serial.print(output);
  Serial.print(", ");
  Serial.println();
}
