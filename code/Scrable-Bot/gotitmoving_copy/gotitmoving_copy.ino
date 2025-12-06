#include<Pixy2.h>
#include<SPI.h>
#include<Pixy2SPI_SS.h>
#include<Pixy2CCC.h>
Pixy2 pixy;
float deadzone = 0.15;
int cont = 0;
int signature , x,y,width,height;
float cx,cy,area;
const int motor2a = 41;
const int motor2b = 39;
const int motor3a = 43;
const int motor3b = 45;
const int motor1a = 38;
const int motor1b = 40;
const int motor4a = 42;
const int motor4b = 44;
void setup() {
  Serial.begin(9600);
  pixy.init();
  pinMode(motor1a, OUTPUT); // Motor 2and going backwards
  pinMode(motor1b, OUTPUT); //
  pinMode(motor2a, OUTPUT); //
  pinMode(motor2b, OUTPUT); //
  pinMode(motor3a, OUTPUT); // 
  pinMode(motor3b, OUTPUT); //
  pinMode(motor4a, OUTPUT); //
  pinMode(motor4b, OUTPUT); // 
}

void forward()
{
digitalWrite(motor1a, LOW);
digitalWrite(motor1b, HIGH);
digitalWrite(motor2a, HIGH);
digitalWrite(motor2b, LOW);
digitalWrite(motor3a, HIGH);
digitalWrite(motor3b, LOW);
digitalWrite(motor4a, HIGH);
digitalWrite(motor4b, LOW);
}

void backward()
{
digitalWrite(motor1a, HIGH);
digitalWrite(motor1b, LOW);
digitalWrite(motor2a, LOW);
digitalWrite(motor2b, HIGH);
digitalWrite(motor3a, LOW);
digitalWrite(motor3b, HIGH);
digitalWrite(motor4a, LOW);
digitalWrite(motor4b, HIGH);
}

void stopMotors()
{
digitalWrite(motor1a, LOW);
digitalWrite(motor1b, LOW);
digitalWrite(motor2a, LOW);
digitalWrite(motor2b, LOW);
digitalWrite(motor3a, LOW);
digitalWrite(motor3b, LOW);
digitalWrite(motor4a, LOW);
digitalWrite(motor4b, LOW);
}
void move()
{
  if(leftspeed >= 0)
{

}
}
float mapfloat(long x,long in_min,long in_max,long out_min,long out_max)
{
  return(float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}
float pixyCheck()
{
  static int i = 0;
  int j;
  uint16_t blocks;
  char buf[32];
  blocks = pixy.getBlocks();
  if(blocks)
  {
    signature = pixy.blocks[0].signature;
    height = pixy.blocks[0].height;
    width = pixy.blocks[0].width;
    x = pixy.blocks[0].x;
    y = pixy.blocks[0].y;
    cx = (x + (width / 2));
    cy = (y + (height/2));
    cx = mapfloat(cx,0,320,-1,1);
    cy = mapfloat(cy,0,200,1,-1);
    area = width * height;
  }
  else
  {
    cont += 1;
    if(cont == 100)
    {
      cont = 0;
      cx = 0;
    }
  }
  return cx;
}
void pixyloop()
{
  float turn = pixyCheck();
  if(turn > -deadzone && turn < deadzone)
  {
    turn = 0;
  }
  if(turn < 0)
  {
    forward();
  }
  else if(turn > 0)
  {
    backward();
  }
  delay(5);
}
void loop() {
  forward();
  delay(2000);

  // backward();
  // delay(1000);

  // stopMotors();
  // delay(2000);
}
