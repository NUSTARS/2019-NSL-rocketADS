#include "sensors.h"
#include "storage.h"
using namespace nustars;

Accelerometer* accelerometer = NULL;
Altimeter* altimeter = NULL;
Storage* storage = NULL;

void setup() {
  accelerometer = new Accelerometer;
  altimeter = new Altimeter;
  storage = new Storage("out");
  Serial.print("Hello");
  delay(1000);
}

void loop() {
  storage->write("hello world");
  delay(300);
}
