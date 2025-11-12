#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("Hello, World!");
  Serial.print("2 + 3 = ");
  Serial.println(myFunction(2, 3)); 
  delay(1000);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}