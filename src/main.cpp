#include <Arduino.h>
#include "dht_nonblocking.h"

// put function declarations here:
int myFunction(int, int);

int temperaturePin = 5;
int humidityPin = 5;


void setup() {
  // put your setup code here, to run once:
  //int result = myFunction(2, 3);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}