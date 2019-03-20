#include "sensors.h"
#include "storage.h"
using namespace nustars;

Accelerometer* accelerometer = NULL;
Altimeter* altimeter = NULL;
Storage* storage = NULL;

void setup() {
  accelerometer = new Accelerometer;
  altimeter = new Altimeter;
  storage = new Storage("out.csv");
  Serial.print("Hello");
  delay(1000);
}

void loop() {
  char* msg = new char[500]{0};
  sprintf(msg, "%d,%d,%d", accelerometer->getOrientation(0), accelerometer->getOrientation(1), accelerometer->getOrientation(2));
  storage->write(msg);
  Serial.println(msg);
  delay(10);
}
