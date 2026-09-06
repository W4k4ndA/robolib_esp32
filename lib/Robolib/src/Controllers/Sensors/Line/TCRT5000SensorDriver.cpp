#include "TCRT5000SensorDriver.h"
#include <Arduino.h>

namespace robolib
{
    TCRT5000SensorDriver::TCRT5000SensorDriver(uint8_t pin, bool sensorType) : sensorPin(pin), isDigital(sensorType) {}

    void TCRT5000SensorDriver::init()
    {
        IRobotModule::init();
        pinMode(sensorPin, INPUT);
    }

    void TCRT5000SensorDriver::update()
    {
        IRobotModule::update();
    }

    void TCRT5000SensorDriver::set1IsOnLine(bool value)
    {
        is1onLine = value;
    }

    int TCRT5000SensorDriver::getAnalogLine()
    {
        return analogRead(sensorPin);
    }

    bool TCRT5000SensorDriver::getLine()
    {
        if (is1onLine == digitalRead(sensorPin))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void TCRT5000SensorDriver::setThreshold(int value)
    {
        threshold = value;
    }

    bool TCRT5000SensorDriver::getIsOnLine(){
        return analogRead(sensorPin) >= threshold;
    }
}