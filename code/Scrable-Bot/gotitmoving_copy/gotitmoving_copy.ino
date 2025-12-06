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
  pinMode(TL_FWD, OUTPUT);
  pinMode(TL_BWD, OUTPUT);
  pinMode(BL_FWD, OUTPUT);
  pinMode(BL_BWD, OUTPUT);
  pinMode(TR_FWD, OUTPUT);
  pinMode(TR_BWD, OUTPUT);
  pinMode(BR_FWD, OUTPUT);
  pinMode(BR_BWD, OUTPUT);
}

void forward()
{
digitalWrite(TL_FWD, HIGH);
digitalWrite(TL_BWD, LOW);

digitalWrite(BL_FWD, HIGH);
digitalWrite(BL_BWD, LOW);

digitalWrite(TR_FWD, LOW);
digitalWrite(TR_BWD, HIGH);
  
digitalWrite(BR_FWD, LOW);
digitalWrite(BR_BWD, HIGH);
}

void backward()
{
  digitalWrite(TL_FWD, LOW);
  digitalWrite(TL_BWD, HIGH);

  digitalWrite(BL_FWD, LOW);
  digitalWrite(BL_BWD, HIGH);

  digitalWrite(TR_FWD, LOW);
  digitalWrite(TR_BWD, HIGH);

  digitalWrite(BR_FWD, LOW);
  digitalWrite(BR_BWD, HIGH);
}

void stopMotors() {
  digitalWrite(TL_FWD, LOW);
  digitalWrite(TL_BWD, LOW);
  digitalWrite(BL_FWD, LOW);
  digitalWrite(BL_BWD, LOW);
  digitalWrite(TR_FWD, LOW);
  digitalWrite(TR_BWD, LOW);
  digitalWrite(BR_FWD, LOW);
  digitalWrite(BR_BWD, LOW);
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
