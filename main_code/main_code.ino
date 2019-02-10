#include "sensors.h"
#include "storage.h"
#include <string>

#define POWER_LED 8
#define STATUS_LED 9
#define ERROR_LED 10
#define BUILT_IN_LED 13

using namespace nustars;

Accelerometer* accelerometer = NULL;
Altimeter* altimeter = NULL;
Storage* storage = NULL;
int timerCounter = 0;
bool errorFlag = false;

  uint8_t testBit = 0;

  byte value = 0;
//IntervalTimer timer;
IntervalTimer saveTimer;

void setup() {
  Wire.setClock(400000L);
  pinMode(POWER_LED, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);
  pinMode(BUILT_IN_LED, OUTPUT);


  digitalWrite(POWER_LED, HIGH);
  digitalWrite(ERROR_LED, HIGH);
  Serial.begin(115200);
  Serial.println("Test");


  accelerometer = new Accelerometer;
  altimeter = new Altimeter;
  storage = new Storage("out.csv");
  Serial.print("Hello");
  delay(1000);
  std::string msg;
  
    msg = "time, x orientation, y orientation, z orientation, x accel, y accel, z accel, x gyro, y gyro, z gyro, pressure, altitude";
    storage -> write(msg);
    digitalWrite(ERROR_LED, LOW);
  //timer.begin(realLoop, 20000);
  saveTimer.begin(saveLoop, 1000000);
}

/*void loop() {




}*/

void saveLoop() {
  digitalWrite(13, HIGH);
  storage->save();
}

void loop() {
  using namespace std;
  std::string msg;
  accelerometer->tick();
  altimeter->tick();
  
  Wire.beginTransmission(BNO055_ADDRESS_A);

  Wire.send(0x00);

  Wire.endTransmission();
  Wire.requestFrom(BNO055_ADDRESS_A, (byte)1);

  value = Wire.receive();
 

  testBit = value;

    if (millis() - 1000 > timerCounter) {
    timerCounter = millis();
    digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
  }



  if (testBit != BNO055_ID) {
    errorFlag = true;
    accelerometer->reconnect();
    storage->write("bno disconnected");
  }

  

  
  Serial.println(testBit);
  msg =  to_string(millis()) + ", ";
  for (const auto & x : accelerometer->getVals()) {
    msg += to_string(x);
    msg+= ",";
  }
  msg += to_string(alitimter->getPressure()) +  ",";
  msg += to_string(altimiter -> getAltitude());
  
  // sprintf(msg, "%lu,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", millis(), accelerometer->getOrientation(0), accelerometer->getOrientation(1), accelerometer->getOrientation(2),
  //         accelerometer->getAcceleration(0), accelerometer->getAcceleration(1), accelerometer->getAcceleration(2),
  //         accelerometer->getGyro(0), accelerometer->getGyro(1), accelerometer->getGyro(2),
  //         altimeter->getPressure(), altimeter->getAltitude());
  if(!storage->write(msg)) {
    errorFlag = true;
  }
  Serial.println(msg);

  if (errorFlag) {
    digitalWrite(ERROR_LED, HIGH);
    errorFlag = false;
  }
  else {
    digitalWrite(ERROR_LED, LOW);
  }

}
