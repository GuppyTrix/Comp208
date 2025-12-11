const int Left_Pin = 27; 
const int Right_Pin = 25; 
const int Found_Pin = 23; 
const int Arrived_Pin = 29; 

void setup() {
  Serial.begin(9600);
  pinMode(Left_Pin, INPUT);
  pinMode(Right_Pin, INPUT);
  pinMode(Found_Pin, INPUT);
  pinMode(Arrived_Pin, INPUT);
}

void loop() {
  int Left = digitalRead(Left_Pin);  
  int Right = digitalRead(Right_Pin);  
  int Found = digitalRead(Found_Pin);  
  int Arrived = digitalRead(Arrived_Pin);  

  Serial.print("Left:");
  Serial.print(Left);
  Serial.print(",");

  Serial.print("Right:");
  Serial.print(Right);
  Serial.print(",");

  Serial.print("Found:");
  Serial.print(Found);
  Serial.print(",");

  Serial.print("Arrived:");
  Serial.println(Arrived);
}
