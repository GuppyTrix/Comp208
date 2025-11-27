#inlcude <Ultrasonic.h>

// MOTOR PINS //
#define L_MOTOR_A   11
#define L_MOTOR_B   10

#define R_MOTOR_A   12
#define R_MOTOR_B   13


// ULTRASOUND SENSOR PINS //
#define L_TRIG 1
#define L_ECHO 0

#define M_TRIG 3
#define M_ECHO 2

#define R_TRIG 7
#define R_ECHO 6


const float SOUND_SPEED = 0.0343;
long duration;
float distance;
unsigned long lastMeasurement = 0;
const unsigned long MEASUREMENT_INTERVAL = 200;
void LEFTMOTOR(int f,int b)
{
  analogWrite(L_MOTOR_A,f);
  analogWrite(L_MOTOR_B,b);
}
void RIGHTMOTOR(int f,int b)
{
  digitalWrite(R_MOTOR_A,f > 0 ? HIGH : LOW);
  digitalWrite(L_MOTOR_B,b > 0 ? HIGH : LOW);
}
void STOP()
{
  LEFTMOTOR(0,0);
  RIGHTMOTOR(0,0);
}
void FORWARD()
{
  LEFTMOTOR(150,0);
  RIGHTMOTOR(1,0);
}
void BACKWARDS()
{
  LEFTMOTOR(0,150);
  RIGHTMOTOR(0,1);
}
void TURNLEFT()
{
  LEFTMOTOR(0,150);
  RIGHTMOTOR(1,0);
}
void TURNRIGHT()
{
  LEFTMOTOR(150,0);
  RIGHTMOTOR(0,1);
}
void TURN_AROUND()
{
  TURNLEFT();
  delay(500);
  TURNLEFT();
  STOP();
}
float measureDistance(int TRIG, int ECHO)
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  
  // Set trigPin HIGH for 10μs
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  // Read echoPin
  duration = pulseIn(ECHO, HIGH);
  
  // Calculate distance (cm)
  distance = (duration * SOUND_SPEED) / 2;
  
  return distance;
}
int measurement()
{
  uint currentmillis = millis();
  if (currentmillis - lastMeasurement >= MEASUREMENT_INTERVAL)
  {
    distance = measureDistance();
    lastMeasurement = millis();
  }
  return distance;
}
void setup()
{
  Serial.begin(115200);

  // MOTORS//
  pinMode(L_MOTOR_A, OUTPUT);
  pinMode(L_MOTOR_B, OUTPUT);
  pinMode(R_MOTOR_A, OUTPUT);
  pinMode(R_MOTOR_B, OUTPUT);


  // ULTRASOUND SENSOR PINS //
  pinMode(L_TRIG,OUTPUT);
  pinMode(L_ECHO,OUTPUT);

  pinMode(M_TRIG,OUTPUT);
  pinMode(M_ECHO,OUTPUT);
  
  pinMode(R_TRIG,OUTPUT);
  pinMode(R_ECHO,OUTPUT);

  Serial.println("Robot Initialized");
}

void loop()
{
  const int WALLDISTANCE 5;
  bool wall = distance < WALLDISTANCE;
  if(wall)
  {
    TURNRIGHT();
    return;
  }
  FORWARD();
  delay(500);
}
