#include "sensors.h"
#include "storage.h"
using namespace nustars;

Accelerometer* accelerometer = NULL;
Altimeter* altimeter = NULL;
Storage* storage = NULL;
IntervalTimer timer;

void setup() {
  while(!Serial);
  Serial.println("Test");

  
  accelerometer = new Accelerometer;
  altimeter = new Altimeter;
  storage = new Storage("out.csv");
  Serial.print("Hello");
  delay(1000);
  timer.begin(realLoop, 20000);
}

void loop() {
  return;
}

void realLoop() {
  char* msg = new char[500]{0};
  accelerometer->tick();
  altimeter->tick();
  sprintf(msg, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", millis(), accelerometer->getOrientation(0), accelerometer->getOrientation(1), accelerometer->getOrientation(2),
    accelerometer->getAcceleration(0), accelerometer->getAcceleration(1), accelerometer->getAcceleration(2),
    accelerometer->getGyro(0), accelerometer->getGyro(1), accelerometer->getGyro(2),
    altimeter->getPressure(), altimeter->getAltitude());
  storage->write(msg);
  Serial.println(msg);
  delete msg;
  delay(10);
}
