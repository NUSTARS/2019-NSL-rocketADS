#include "sensors.h"

namespace nustars {

    /**********ACCELEROMETER
     ***********************
     **********************/

    /**
     * Initialize the BNO
     */
    Accelerometer::Accelerometer() {
        bno = Adafruit_BNO055(55); //I2C address, probably.
        orientation = new float[3];
        gyro = new float[3];
        acc = new float[3];
        if (!bno.begin()) {
            Serial.print(
                    "Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!"); //TODO: Learn to throw an exception
            while (1);
        }
        bno.setExtCrystalUse(true);
    }

    /**
     * Update the orientation information
     */
    void Accelerometer::tick() {
        imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
        orientation[0] = euler.x();
        orientation[1] = euler.y();
        orientation[2] = euler.z();
        euler = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
        gyro[0] = euler.x();
        gyro[1] = euler.y();
        gyro[2] = euler.z();
        euler = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
        acc[0] = euler.x();
        acc[1] = euler.y();
        acc[2] = euler.z();
    }

    void Accelerometer::reconnect() {
      bno.begin();
    }

    /**
     * Get the current orientation
     * @param axis From Sensor class, integer representing axis to retrieve
     * @return The value of the orientation on requested axis
     */
    float Accelerometer::getOrientation(int axis) {
        return orientation[axis];
    }

    float Accelerometer::getGyro(int axis) {
        return gyro[axis];
    }
    std::vector<float> Accelerometer::getVals() {
        std::vector<float> res;
        for (int i = 0; i < 2; i++) {
            res.push_back(getOrientation(i));
        }
        for (int i = 0; i < 2; i++) {
            res.push_back(getAcceleration(i));
        }
        for (int i = 0; i < 2; i++) {
            res.push_back(getGyro(i));
        }
        return res;
    }

    /**
     * Get the current acceleration
     * @param axis From Sensor class, integer representing axis to retrieve
     * @return The value of the acceleration on requested axis
     */
    float Accelerometer::getAcceleration(int axis) {
        return acc[axis];
    }


    /***********ALTIMETER
     ********************
     *******************/

    /**
     * Initialize the BME and perform starting tasks
     */
    Altimeter::Altimeter() {
        if (!bme.begin()) {
            Serial.println("Could not find a valid BMP280 sensor, check wiring!");
            while (1);
        }
        setBaseAlt();
    }

    /**
     * Take several samples of the current altitude and record the average
     */
    void Altimeter::setBaseAlt() {
        const int NUM_BASE_SAMPLES = 10;
        //Set altitude at ground
        baseAlt = 0;
        for (int i = 0; i < NUM_BASE_SAMPLES; i++) {
            baseAlt += bme.readAltitude(1000);
        }
        baseAlt = baseAlt / NUM_BASE_SAMPLES; //average readings
    }

    /**
     * Update the current altitude, pressure, and temperature
     */
    void Altimeter::tick() {
        bme.performReading();
        temp = bme.temperature;
        pressure = bme.pressure;
        alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
    }

    //Altimeter getters

    //get temperature
    float Altimeter::getTemp() {
        return temp;
    }

    //get altitude
    float Altimeter::getAltitude() {
        return alt;
    }

    //get pressure
    float Altimeter::getPressure() {
        return pressure;
    }
} //END NAMESPACE

