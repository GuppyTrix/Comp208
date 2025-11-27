#include <Ultrasonic.h>

#define echoPin 2
#define trigPin 3

// Calibration variables
const float SOUND_SPEED = 0.0343; // cm/μs at 20°C

// Variables for measurement
long duration;
float distance;
unsigned long lastMeasurement = 0;
const unsigned long MEASUREMENT_INTERVAL = 200; // ms

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
}


void loop() {
  uint currentmillis = millis();
  if (currentmillis - lastMeasurement >= MEASUREMENT_INTERVAL) {
    distance = measureDistance();
    
    Serial.print("Duration: ");
    Serial.print(duration);
    Serial.print(" μs, Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    lastMeasurement = millis();
  }
}

float measureDistance() {
  // Clear trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Set trigPin HIGH for 10μs
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read echoPin
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance (cm)
  distance = (duration * SOUND_SPEED) / 2;
  
  return distance;
}
