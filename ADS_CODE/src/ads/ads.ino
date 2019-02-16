#include <pid.h>
#include "Peripherals/sensors.h"
#include "Peripherals/storage.h" 
#include <string>


#define STATUS_LED 9
#define ERROR_LED 10
#define MISC_LED 11
#define BUILT_IN_LED 13

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
    digitalWrite(MISC_LED, HIGH);

    //TODO: change to enable serial only if serial monitor detected
    //configure serial monitor
    Serial.begin(115200);


    accelerometer = new Accelerometer;
    altimeter = new Altimeter;
    storage = new Storage("out.csv");

    String msg;
    msg = "time, x orientation, y orientation, z orientation, x accel, y accel, z accel, x gyro, y gyro, z gyro, pressure, altitude";
    storage -> write(msg);

    //turn of misc led to show enabling is done
    digitalWrite(MISC_LED, LOW);

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
    String msg;
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
        digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
    }



    if (testBit != BNO055_ID) {
        errorFlag = true;
        accelerometer->reconnect();
        storage->write("bno disconnected");
    }

    

    
    Serial.println(testBit);
    msg =  String(millis()) + ", ";
    for (const auto & x : accelerometer->getVals()) {
        msg += String(x);
        msg+= ",";
    }
    msg += String(altimeter->getPressure()) +  ",";
    msg += String(altimeter -> getAltitude());
    
    //check for storage issue
    if(!storage->write(msg)) {
        errorFlag = true;
        storage = new Storage("out.csv";)
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

    if (accelerometer.getAcceleration(3) < -8) {
        inFlight = true;
        flightTimestamp = millis();
    }
    
    if (!postApogee && inFlight && millis-flightTimestamp > 3500) {
        postBurnout = true;
        if (avgVerticalVelocity < 0) postApogee = true;
    }

    if (inFlight && postBurnout && !postApogee) {
            //do the thing
    }

}