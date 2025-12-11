#include <Encoder.h>
// const int TR = 5;
// const int TRb = 4;
// const int TL = 3;
// const int TLb = 2;
// const int BR = 6;
// const int BRb = 7;
// const int BL = 8;
// const int BLb = 9;
// const int motor1a = TR;
// const int motor1b = TRb;
// const int motor2a = BR;
// const int motor2b = BRb;
// const int motor3a = TL;
// const int motor3b = TLb;
// const int motor4a = BL;
// const int motor4b = BLb;
//top
const int motor1a = 4;
const int motor1b = 5;
//top 
const int motor2a = 2;
const int motor2b = 3;
//bottom right
const int motor3a = 6;
const int motor3b = 7;
//bottom left
const int motor4a = 8;
const int motor4b = 9;
Encoder FL_motor(20, 21);  // 1 Front Left
Encoder FR_motor(18, 19);  // 2 Front Right
int speed = 255;
void setup()
{
  Serial.begin(9600);

  pinMode(motor1a, OUTPUT);
  pinMode(motor1b, OUTPUT);
  pinMode(motor2a, OUTPUT);
  pinMode(motor2b, OUTPUT);
  pinMode(motor3a, OUTPUT);
  pinMode(motor3b, OUTPUT);
  pinMode(motor4a, OUTPUT);
  pinMode(motor4b, OUTPUT);
}
 void motor1forward()//TOPLEFT doesn't work
 {
  analogWrite(motor1a, speed);
  analogWrite(motor1b, 0);
 }
  void motor2forward()
 {
  analogWrite(motor2a, speed);
  analogWrite(motor2b, 0);
 }
  void motor3forward()// doesn't work top left
 {
  analogWrite(motor3a, speed);
  analogWrite(motor3b, 0);
 }
  void motor4forward() // bottomleft
 {
  analogWrite(motor4a, speed);
  analogWrite(motor4b, 0);
 }
 void forward()
{
  motor1forward();
  motor2forward();
  motor3forward();
  motor4forward();
}
  void motor1backward()
 {
  analogWrite(motor1a, 0);
  analogWrite(motor1b, speed);
 }
  void motor2backward()
 {
  analogWrite(motor2a, 0);
  analogWrite(motor2b, speed);
 }
  void motor3backward()
 {
  analogWrite(motor3a, 0);
  analogWrite(motor3b, speed);
 }
  void motor4backward()
 {
  analogWrite(motor4a, 0);
  analogWrite(motor4b, speed);
 }
 void backward()
{
  motor1backward();
  motor2backward();
  motor3backward();
  motor4backward();
}
void left()
{
  int turn = 100;

  analogWrite(motor1a, turn);
  analogWrite(motor1b, 0);

  analogWrite(motor2a, turn);
  analogWrite(motor2b, 0);

  analogWrite(motor3a, speed);
  analogWrite(motor3b, 0);

  analogWrite(motor4a, speed);
  analogWrite(motor4b, 0);
}

void right()
{
  int turn = 100;

  analogWrite(motor1a, speed);
  analogWrite(motor1b, 0);

  analogWrite(motor2a, speed);
  analogWrite(motor2b, 0);

  analogWrite(motor3a, turn);
  analogWrite(motor3b, 0);

  analogWrite(motor4a, turn);
  analogWrite(motor4b, 0);
}

void stopMotors()
{
  analogWrite(motor1a, 0);
  analogWrite(motor1b, 0);
  analogWrite(motor2a, 0);
  analogWrite(motor2b, 0);
  analogWrite(motor3a, 0);
  analogWrite(motor3b, 0);
  analogWrite(motor4a, 0);
  analogWrite(motor4b, 0);
}
void movement()
{
  forward();
  delay(1000);

  backward();
  delay(1000);

  right();
  delay(1000);

  left();
  delay(1000);
}

void loop()
{
  motor3forward();
}
