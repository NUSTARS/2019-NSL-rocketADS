//#include <pid.h>
#include "sensors.h"
#include "storage.h" 
#include <string>
#include <Servo.h>



#define STATUS_LED 10
#define ERROR_LED 9
#define BUILT_IN_LED 13
#define MISC_LED 11

#define BURNOUT_TIME 3500
#define APOGEE_TIME  17000

using namespace nustars;

Accelerometer* accelerometer = NULL;
Altimeter* altimeter = NULL;
Storage* storage = NULL;

IntervalTimer timer;
IntervalTimer saveTimer;

bool errorFlag = false;
uint8_t testBit = 0;
byte value = 0;
int timerCounter = 0;

bool inFlight = false;
uint32_t flightTimestamp = 0;
bool postBurnout = false;
bool postApogee = false;



Servo adsServo;
int servoPos = 0;

void setup()
{
    //increase speed of wire bus
    Wire.setClock(400000L);

    //set pinmodes to outoput
    pinMode(MISC_LED, OUTPUT);
    pinMode(STATUS_LED, OUTPUT);
    pinMode(ERROR_LED, OUTPUT);
    pinMode(BUILT_IN_LED, OUTPUT);

    //display that setup is beign enabled
    digitalWrite(ERROR_LED, HIGH);

    //TODO: change to enable serial only if serial monitor detected
    //configure serial monitor
    Serial.begin(115200);

    adsServo.attach(38);

    accelerometer = new Accelerometer;
    altimeter = new Altimeter;
    storage = new Storage("out.csv");
    
    char* msg = new char[500] {0};
    msg = "time, x orientation, y orientation, z orientation, x accel, y accel, z accel, x gyro, y gyro, z gyro, pressure, altitude";
    storage -> write(msg);

    //turn of misc led to show enabling is done



        digitalWrite(ERROR_LED, LOW);
    //enable two interrupts for saving, data collecting and stuff
    timer.begin(processLoop, 20000);
    saveTimer.begin(saveLoop, 15000000);

    //todo: read calibration data in from previous runs for accelerometers
	
}

//todo: check for verticalness of rocket and only start timer at that point. 
//todo: dont start save loop until flight start
void loop()
{
	// Not really anything here
}

void saveLoop() {
  storage->save();
}

void processLoop() {
    using namespace std;
    char* msg = new char[500] {0};
    accelerometer->tick();
    altimeter->tick();
    
    //check for BNO connection
    Wire.beginTransmission(BNO055_ADDRESS_A);
    Wire.send(0x00);
    Wire.endTransmission();
    Wire.requestFrom(BNO055_ADDRESS_A, (byte)1);
    value = Wire.receive();
    

    testBit = value;

        if (millis() - 1000 > timerCounter) {
        timerCounter = millis();
        digitalWrite(MISC_LED, !digitalRead(MISC_LED));
    }



    if (testBit != BNO055_ID) {
        errorFlag = true;
        accelerometer->reconnect();
        storage->write("bno disconnected");
    }

    

    
    Serial.println(testBit);
    
sprintf(msg, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", millis(), accelerometer->getOrientation(0), accelerometer->getOrientation(1), accelerometer->getOrientation(2),
          accelerometer->getAcceleration(0), accelerometer->getAcceleration(1), accelerometer->getAcceleration(2),
          accelerometer->getGyro(0), accelerometer->getGyro(1), accelerometer->getGyro(2),
altimeter->getPressure(), altimeter->getAltitude());
    
    //check for storage issue
    if(!storage->write(msg)) {
        errorFlag = true;
        storage = new Storage("out.csv");
    }
    Serial.println(msg);


    //set LED high if issue
    if (errorFlag) {
        digitalWrite(ERROR_LED, HIGH);
        errorFlag = false;
    }
    else {
        digitalWrite(ERROR_LED, LOW);
    }

    if (accelerometer->getAcceleration(3) < -2) {
        inFlight = true;
        flightTimestamp = millis();
    }
    
    if (!postApogee && inFlight && millis()-flightTimestamp > BURNOUT_TIME) {
        postBurnout = true;
        if (millis()-flightTimestamp > APOGEE_TIME) postApogee = true;
    }

    if (inFlight && postBurnout && !postApogee) {
            servoPos = 180;
    }
    else servoPos = 0;

    Serial.print(accelerometer -> getAcceleration(3));
    Serial.println(servoPos);

    delete msg;

}
