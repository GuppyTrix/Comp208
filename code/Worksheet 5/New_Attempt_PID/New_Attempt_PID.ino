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

float ticksPerDegree = (63 * 46) / 360;
float distancePerDegree = 0.651;  //mm
//diameter == 70mm

// function for driving a motor from just one value between -255 and 255
void motorDrive(int power, int FW, int BW) {
  // check the input value is in the right range
  power = constrain(power, -100, 100);

  if (power > 0) {
    analogWrite(FW, power);
    analogWrite(BW, 0);
  } else if (power < 0) {
    analogWrite(FW, 0);
    analogWrite(BW, -power);
  }
}

class PIDController {
private:
  float kp, ki, kd;
  float integral = 0.0;
  float previousError = 0.0;
  long previousTime = 0;
  
public:
  PIDController(float p, float i, float d) : kp(p), ki(i), kd(d) {}
  
  float compute(float setpoint, float input) {
    unsigned long currentTime = millis();
    unsigned long dt = (currentTime - previousTime); // time differenve
    float error = setpoint - input; // Proportional term
    integral += error * dt;  // Integral term
    float derivative = (error - previousError) / dt;  // Derivative term
    float output = (kp * error) + (ki * integral) + (kd * derivative);

    previousError = error;
    previousTime = currentTime;

    Serial.print("error:");
    Serial.print(error);
    Serial.print(",");

    Serial.print("integral:");
    Serial.print(integral);
    Serial.print(",");

    Serial.print("derivative:");
    Serial.print(derivative);
    Serial.print(",");

    Serial.print("output:");
    Serial.println(output);
    
    return output;
  }
  
  void reset() {
    integral = 0.0;
    previousError = 0.0;
    previousTime = 0;
  }
};

PIDController leftPID(0.03, 0.01, 0.01);
// PIDController rightPID(0.5, 0.2, 0.2);
void setup() {
  Serial.begin(115200);

  // set the pin modes of the motor pins and encoder pins
  pinMode(L_MOTOR_A, OUTPUT);
  pinMode(L_MOTOR_B, OUTPUT);
  pinMode(R_MOTOR_A, OUTPUT);
  pinMode(R_MOTOR_B, OUTPUT);

  R_encoder.begin();
  R_encoder.flip();

  L_encoder.begin();
  L_encoder.flip();


  delay(200);
}

void loop() {
  // convert the encoder ticks to degrees
  long L_pos = L_encoder.getCount()/ticksPerDegree/distancePerDegree;
  long R_pos = R_encoder.getCount()/ticksPerDegree/distancePerDegree;

  L_setpoint = 1000;
  R_setpoint = 1000;

  float L_output = leftPID.compute(L_setpoint, L_pos);
  // float R_output = rightPID.compute(R_setpoint, R_pos);
  motorDrive(L_output, L_MOTOR_A, L_MOTOR_B);
  // motorDrive(R_output, R_MOTOR_A, R_MOTOR_B);
  
  delay(10);
}