#include <Ultrasonic.h>
#include <QMC5883LCompass.h>
#include <Encoder.h>
const int BACK_LEFT_MOTOR = 3;
const int BACK_RIGHT_MOTOR = 4;
const int LEDFAULT = 5;

#define LEFT_MOTOR_A 11
#define LEFT_MOTOR_B 10
#define RIGHT_MOTOR_A 12
#define RIGHT_MOTOR_B 13
void LEFT_CONTROL(int raw_input)
{
  int motor_power = constrain(raw_input, -255, 255);
}
void RIGHT_CONTROL(int raw_input)
{
int motor_power = constrain(raw_input, -255,255);
}
void FORWARD(int raw_input)
{
  digitalWrite(RIGHT_MOTOR_A,HIGH);
  digitalWrite(LEFT_MOTOR_A,HIGH);
  digitalWrite(BACK_LEFT_MOTOR,HIGH);
  digitalWrite(BACK_RIGHT_MOTOR,HIGH);
}
void STOP(int raw_input)
{
  digitalWrite(RIGHT_MOTOR_A,LOW);
  digitalWrite(LEFT_MOTOR_A,LOW);
  digitalWrite(BACK_LEFT_MOTOR,LOW);
  digitalWrite(BACK_RIGHT_MOTOR,LOW);
}
void LEFT(int raw_input,int motor_power)
{
digitalWrite(LEFT_MOTOR_B,LOW);
analogWrite(LEFT_MOTOR_A,abs(motor_power));
analogWrite(RIGHT_MOTOR_A,abs(motor_power));
digitalWrite(RIGHT_MOTOR_B,LOW);
digitalWrite(BACK_LEFT_MOTOR,HIGH);
digitalWrite(BACK_RIGHT_MOTOR,HIGH);
}
void RIGHT(int raw_input,int motor_power)
{
digitalWrite(RIGHT_MOTOR_A,LOW);
analogWrite(RIGHT_MOTOR_B,abs(motor_power));
analogWrite(LEFT_MOTOR_B),abs(motor_power));
digitalWrite(LEFT_MOTOR_A,LOW);
digitalWrite(BACK_LEFT)
}
void REVERSE(int raw_input)
{
// how Can I reverse dc motor on the back
}
void stuck(int raw_input)
{
  // go do certain directions in a certain order.
}
void setup()
{
  // put your setup code here, to run once:
  pinMode(BACK_LEFT_MOTOR,OUTPUT);
  pinMode(BACK_RIGHT_MOTOR,OUTPUT);
  pinMode(LEFT_MOTOR_A,OUTPUT);
  pinMode(LEFT_MOTOR_B,OUTPUT);
  pinMode(RIGHT_MOTOR_A,OUTPUT);
  pinMode(RIGHT_MOTOR_B,OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:

}
