#include <Encoder.h>

Encoder FL_motor(2, 4);   // 1 Front Left
Encoder FR_motor(3, 5);   // 2 Front Right
Encoder BL_motor(21, 6);  // 3 Back Left
Encoder BR_motor(21, 7);  // 4 Back Right

void setup() {
  Serial.begin(9600);
}

void loop() {
  int amount = FL_motor.read();
  
  Serial.print("FL:");
  Serial.println(amount);
  Serial.print(",");

  Serial.print("FR:");
  Serial.print(FL_motor.read());
  Serial.print(",");

  Serial.print("BL:");
  Serial.print(FL_motor.read());
  Serial.print(",");

  Serial.print("BR:");
  Serial.print(FL_motor.read());
  Serial.println();

}
