#include "TCRT5kDigitalDriver.h"
#include <Arduino.h>

namespace robolib
{
    TCRT5kDigitalDriver::TCRT5kDigitalDriver(uint8_t pin) : sensorPin(pin) {}

    void TCRT5kDigitalDriver::init()
    {
        IRobotModule::init();
        pinMode(sensorPin, INPUT);
    }

    void TCRT5kDigitalDriver::update()
    {
        IRobotModule::update();
    }

    void TCRT5kDigitalDriver::set1IsOnLine(bool value)
    {
        is1onLine = value;
    }

    bool TCRT5kDigitalDriver::getLine(){
        return digitalRead(sensorPin) == is1onLine;
    }

}