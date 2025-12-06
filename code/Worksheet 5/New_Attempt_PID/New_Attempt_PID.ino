#include <pio_encoder.h>

// Encode Variables //

#define L_MOTOR_A 11  // PWM
#define L_MOTOR_B 10  // DIR

#define R_MOTOR_A 12  // PWM
#define R_MOTOR_B 13  // DIR

// Encoders
PioEncoder L_encoder(18);
PioEncoder R_encoder(20);

// General Variables //

float L_setpoint = 0;
float R_setpoint = 0;
float error = 0;
float output = 0;
float integral = 0;
float deltaTime = 0;
float lastTime = 0;
float previousError = 0;
float derivative = 0;


float ticksPerDegree = (63 * 46) / 360 ;
float distancePerDegree = 0.68; //mm
//diameter == 70mm

// function for driving a motor from just one value between -255 and 255
void motorDrive(int power, int FW, int BW) {

  // check the input value is in the right range
  power = constrain(power, -255, 255);

  analogWrite(FW, -power);
  analogWrite(BW, power);
}

void Proportional_Integral_Derivative_Control(float setpoint, float pos, int motor_A, int motor_B ) {

  float KP = 1;
  float KI = 1.1;
  float KD = 4;

  unsigned long currentTime = millis(); // get system time 
  deltaTime = (currentTime - lastTime) / 100.0; // time differenve
  error = setpoint - pos; // Proportional term
  integral += error * deltaTime;  // Integral term
  derivative = (error - previousError) / deltaTime;  // Derivative term
  output = (KP * error) + (KI * integral) + (KD * derivative);
  lastTime = currentTime;
  previousError = error;


  motorDrive(-output, motor_A, motor_B);
}

void setup() {
  Serial.begin(115200);

  // set the pin modes of the motor pins and encoder pins
  pinMode(L_MOTOR_A, OUTPUT);
  pinMode(L_MOTOR_B, OUTPUT);
  pinMode(L_MOTOR_A, OUTPUT);
  pinMode(L_MOTOR_B, OUTPUT);

  R_encoder.begin();
  R_encoder.flip();
  
  L_encoder.begin();
  L_encoder.flip();

  delay(200);
}

float distancetoticks(float distanceMM){
  return distanceMM/distancePerDegree;
}

void loop() {
  // convert the encoder ticks to degrees
  long L_pos = L_encoder.getCount();
  long R_pos = R_encoder.getCount();


  
  int input;
  // update the desired setpoint of the motor from serial
  if (Serial.available() > 1) {
    input = Serial.parseInt();
    L_setpoint = input;
  }

  // input = distancetoticks(1000);

  //  make the control loop run ten times per second
  delay(100);

  Proportional_Integral_Derivative_Control(L_setpoint, L_pos, L_MOTOR_A, L_MOTOR_B);
  // Proportional_Integral_Derivative_Control(L_setpoint, R_pos, R_MOTOR_A, R_MOTOR_B);



  //  print some debug info, check it out in the serial plotter
  Serial.print("Setpoint:");
  Serial.print(L_setpoint);
  Serial.print(", ");
  Serial.print("Position:");
  Serial.print(L_pos);
  Serial.print(", ");
  Serial.print("Error:");
  Serial.print(error);
  Serial.print(", ");
  Serial.print("OutputPower:");
  Serial.print(output);
  Serial.print(", ");
  Serial.println();
}
