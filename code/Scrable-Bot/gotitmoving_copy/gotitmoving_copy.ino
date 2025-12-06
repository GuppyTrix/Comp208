#include<Pixy2.h>
#include<SPI.h>
#include<Pixy2SPI_SS.h>
#include<Pixy2CCC.h>
Pixy2 pixy;
float deadzone = 0.15;
int cont = 0;
int signature , x,y,width,height;
float cx,cy,area;
const int H1_IN1 = 39;
const int H1_IN2 = 41;
const int H1_IN3 = 43;
const int H1_IN4 = 45;
// H-BRIDGE 2
const int H2_IN1 = 38;
const int H2_IN2 = 40;
const int H2_IN3 = 42;
const int H2_IN4 = 44;
// H-BRIDGE 1 (LEFT SIDE)
const int TL_FWD = H1_IN1;  
const int TL_BWD = H1_IN2;

const int BL_FWD = H1_IN3;
const int BL_BWD = H1_IN4;

// H-BRIDGE 2 (RIGHT SIDE)
const int TR_FWD = H2_IN1;
const int TR_BWD = H2_IN2;

const int BR_FWD = H2_IN3;
const int BR_BWD = H2_IN4;

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
    y = pixy.blocks
  }
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
