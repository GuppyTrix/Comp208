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
float error_integral = 0;

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


void BANGBANG(float setpoint, float pos) {
  if (setpoint > pos) {
    motorDrive(-255);
  } else if (pos > setpoint) {
    motorDrive(255);
  }
}

void Proportional_Control(float setpoint, float pos) {
  float Kp = 3;
  error = setpoint - pos;
  output = Kp * error;
  motorDrive(-output);
}


void Proportional_Integral_Control(float setpoint, float pos) {
  // Output = Kp * error + Ki * error_integral

  float Kp = 3;
  float Ki = 0.1; 

  error = setpoint - pos;
  output = Kp * error;
  error_integral += error;
  
  float integral = Ki * error_integral;
  output = output + integral;

  output = Kp * (setpoint - pos) + (Ki * error_integral);

  
  // Drive motor (negative because of motor direction)
  motorDrive(-output);
}

void Proportional_Integral_Derivative_Control(float setpoint, float pos){

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

  // update the desired setpoint of the motor from serial
  if (Serial.available() > 1) {
    setpoint = Serial.parseInt();
  }

  //  make the control loop run ten times per second
  delay(100);

  // BANGBANG(setpoint, pos);
  // Proportional_Control(setpoint, pos);
  Proportional_Integral_Control(setpoint, pos);
  // Proportional_Integral_Derivative_Control(setpoint, pos);

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
