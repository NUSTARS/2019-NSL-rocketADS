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

#define SERVO_CLOSE 750 //TODO: see if this can be used elsewhere in the code, as there are references to 750 where this is not used
#define SERVO_OPEN 2248

#define CLOSED_SERVO_PIN 0
#define OPEN_SERVO_PIN 1

#define SERVO_STEP_SIZE 50

using namespace nustars;


IntervalTimer timer;
IntervalTimer saveTimer;
IntervalTimer servoTimer;

bool errorFlag = false;
uint8_t testBit = 0;
byte value = 0;
int timerCounter = 0;

bool inFlight = false;
uint32_t flightTimestamp = 0;
bool postBurnout = false;
bool postApogee = false;

//SERVO VARS
Servo myservo; //NOTE: shouldn't need to be volatile as this is not a mutable class
volatile int servoRequestPosition;    //volatility required as per wiki for interrupt pin functions
int actualServoPosition;
byte servoLimits = 0;

void setup() {

    timer.begin(processLoop, 20000);
    servoTimer.begin(servoLoop, 5000); //TODO: change this to an appropriate value; we want it to be slow enough to allow the servo time to update but fast enough that it will move quickly
    saveTimer.begin(saveLoop, 15000000);
    myservo.attach(38);
    servoRequestPosition = (SERVO_OPEN + SERVO_CLOSE) >> 1;
    actualServoPosition = servoRequestPosition;
    myservo.writeMicroseconds(actualServoPosition); //set the servo to its middle position, which should be definitely safe

    //todo: read calibration data in from previous runs for accelerometers

    //assuming the pins are pulled normally high and also I forgot the specifics of the switches so this is general for now...
    pinMode(OPEN_SERVO_PIN, INPUT);
    pinMode(CLOSED_SERVO_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(CLOSED_SERVO_PIN), interruptServoClosed, CHANGE);
    attachInterrupt(digitalPinToInterrupt(OPEN_SERVO_PIN), interruptServoOpen, CHANGE);
}

//todo: check for verticalness of rocket and only start timer at that point.
//todo: dont start save loop until flight start
void loop() {
    //intentionally blank
}

void saveLoop() {
    Serial.println(digitalRead(CLOSED_SERVO_PIN));
}

void processLoop() {

    if (servoRequestPosition > SERVO_OPEN) servoRequestPosition = SERVO_CLOSE;
    servoRequestPosition += 100;
    Serial.println(servoLimits);

}

/**
 * Update the servo position intelligently.
 * Keep track of actual servo position while handling requested position, allowing us to easily correct for over/underextension
 */
void servoLoop() {
    servoRequestPosition -= servoRequestPosition % SERVO_STEP_SIZE;
    if (servoRequestPosition == actualServoPosition) return;
    int dir = servoRequestPosition < actualServoPosition ? -1 : 1;
    dir *= SERVO_STEP_SIZE;
    dir -= SERVO_STEP_SIZE * (dir > 0 && (servoLimits & 1));
    dir += SERVO_STEP_SIZE * (dir < 0 && (servoLimits & 2));
    actualServoPosition += dir;
    if (actualServoPosition > SERVO_OPEN) actualServoPosition = SERVO_OPEN;
    else if (actualServoPosition < SERVO_CLOSE) actualServoPosition = SERVO_CLOSE;
    myservo.writeMicroseconds(actualServoPosition);
}

/**
 * When the overextension limit switch changes, update its limit bit
 */
void interruptServoOpen() {
    servoLimits &= ~1;
    servoLimits |= !digitalRead(OPEN_SERVO_PIN);
}

/**
 * When the underextension limit switch changes, update its limit bit
 */
void interruptServoClosed() {
    servoLimits &= ~2;
    servoLimits |= !digitalRead(CLOSED_SERVO_PIN) << 1;
}
