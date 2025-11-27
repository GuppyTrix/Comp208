#inlcude <Ultrasonic.h>
#define L_MOTOR_A   11
#define L_MOTOR_B   10
#define R_MOTOR_A   12
#define R_MOTOR_B   13
#define L_TRIG 1
#define L_ECHO 0
#define M_TRIG 3
#define M_ECHO 2
#define R_TRIG 7
#define R_ECHO 6
Ultrasonic leftus(L_TRIG,L_ECHO);
Ultrasonic midus(M_TRIG,M_ECHO);
Ultrasonic rightus(R_TRIG,R_ECHO);
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
void setup()
{
  // put your setup code here, to run once:
  pinMode(L_MOTOR_A, OUTPUT);
  pinMode(L_MOTOR_B, OUTPUT);
  pinMode(R_MOTOR_A, OUTPUT);
  pinMode(R_MOTOR_B, OUTPUT);
  Serial.begin(115200);
}

void loop()
{
  // put your main code here, to run repeatedly:
  int LDISTANCE = leftus.read();
  int MDISTANCE = midus.read();
  int RDISTANCE = rightus.read();
// reset wall detection to what I had in 207. Precision.
  TURN_AROUND();
  delay(500);
}
