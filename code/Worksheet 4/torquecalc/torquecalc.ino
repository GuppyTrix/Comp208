#include <Encoder.h>

Encoder motor(2, 3);  // Encoder pins

// H-Bridge pins
const int IN1 = 9;
const int IN2 = 10;
const int ENA = 11;

// Motor/encoder constants
const float gearratio = 63.0;
const float encodeticks = 44.0;
const float ticksPerRevolution = encodeticks * gearratio;

long pos = 0;
long lastPos = 0;
unsigned long lastRPMTime = 0;
float rpm = 0;
bool israising = true;


void setup() {
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
}

void loop() {
  unsigned long currentTime = millis();
  pos = motor.read();

  if (currentTime - lastRPMTime >= 1000) {
    long deltaPos = pos - lastPos;
    float deltaTime = (currentTime - lastRPMTime) / 1000.0; // seconds
    float revolutions = deltaPos / ticksPerRevolution;
    rpm = (revolutions / deltaTime) * 60.0;
    Serial.print("Ticks: ");
    Serial.println(pos);
    Serial.print("Motor RPM: ");
    Serial.println(rpm, 2);
    Serial.println();
    lastPos = pos;
    lastRPMTime = currentTime;
  }
  // Example motion pattern:
  // Forward for 3s, stop for 1s, reverse for 3s, repeat
  unsigned long cycleTime = currentTime % 7000;  // 7 second loop

  // digitalWrite(IN1, HIGH);
  // digitalWrite(IN2, LOW);
  // digitalWrite(ENA, HIGH);
  //- 14903
  if(pos <= 11000 && israising)
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 1023);
  }
  else
  {
    israising = false;
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(ENA, HIGH);
    if (pos <= 0){
      israising = true;
    }
  }
  // if (pos <= encodeticks * gearratio)
  // {
  //   digitalWrite(IN1, HIGH);
  //   digitalWrite(IN2, LOW);
  //   digitalWrite(ENA, HIGH);
  // }
  // else
  // {
  //   digitalWrite(IN1, LOW);
  //   digitalWrite(IN2, LOW);
  //   digitalWrite(ENA, HIGH);
  // }
  // if (cycleTime < 3000) {
  //   // Forward
  //   digitalWrite(IN1, HIGH);
  //   digitalWrite(IN2, LOW);
  //   digitalWrite(ENA, HIGH);
  // }
  // else if (cycleTime < 4000) {
  //   // Stop
  //   digitalWrite(IN1, LOW);
  //   digitalWrite(IN2, LOW);
  //   digitalWrite(ENA, LOW);
  // }
  // else {
  //   // Reverse
  //   digitalWrite(IN1, LOW);
  //   digitalWrite(IN2, HIGH);
  //   digitalWrite(ENA, HIGH);
  // }
}
