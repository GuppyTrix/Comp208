#include <Pixy2.h>
#include <Pixy2CCC.h>

Pixy2 pixy;

// Motor pins
const int motor3a = 38;
const int motor3b = 40;

const int motor1a = 41;
const int motor1b = 39;

const int motor2a = 45;
const int motor2b = 43;

const int motor4a = 42;
const int motor4b = 44;

// Tracking variables from pixy
float deadzone = 0.15;
int signature, x, y, width, height;
float cx, cy, area;

void setup()
{
  Serial.begin(115200);
  pixy.init();
  pixy.changeProg("color_connected_components");

  // Motor outputs
  pinMode(motor1a, OUTPUT);
  pinMode(motor1b, OUTPUT);
  pinMode(motor2a, OUTPUT);
  pinMode(motor2b, OUTPUT);
  pinMode(motor3a, OUTPUT);
  pinMode(motor3b, OUTPUT);
  pinMode(motor4a, OUTPUT);
  pinMode(motor4b, OUTPUT);
}

void forward()
{
  digitalWrite(motor1a, HIGH);
  digitalWrite(motor1b, LOW);
  digitalWrite(motor2a, HIGH);
  digitalWrite(motor2b, LOW);
  digitalWrite(motor3a, HIGH);
  digitalWrite(motor3b, LOW);
  digitalWrite(motor4a, HIGH);
  digitalWrite(motor4b, LOW);
}

void backward()
{
  digitalWrite(motor1a, LOW);
  digitalWrite(motor1b, HIGH);
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

float mapfloat(long x,long in_min,long in_max,long out_min,long out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

float pixyCheck()
{
  uint16_t blocks = pixy.ccc.getBlocks();

  // Print block count every time
  Serial.print("Blocks: ");
  Serial.println(blocks);

  if (blocks)
  {
    signature = pixy.ccc.blocks[0].m_signature;
    height = pixy.ccc.blocks[0].m_height;
    width  = pixy.ccc.blocks[0].m_width;
    x      = pixy.ccc.blocks[0].m_x;
    y      = pixy.ccc.blocks[0].m_y;

    // Print block values for important information
    Serial.print("X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.print(y);
    Serial.print(" W=");
    Serial.print(width);
    Serial.print(" H=");
    Serial.println(height);

    cx = x + (width / 2);
    cy = y + (height / 2);

    cx = mapfloat(cx, 0, 316, -1, 1);
    cy = mapfloat(cy, 0, 208, 1, -1);

    return cx;
  }

  return NAN;
}

void pixyloop()
{
  float turn = pixyCheck();

  if (isnan(turn))   // no blocks detected
  {
    stopMotors();
    return;
  }

  // Object centered? Go forward
  if(turn > -deadzone && turn < deadzone)
  {
    forward();
    return;
  }
  if(turn < 0)
  {
    forward();
  }
  else if(turn > 0)
  {
    backward();
  }

  delay(50);
}

void loop()
{
  pixyloop();
}
