#include <Arduino.h>
#include "TCRT5kAnalogDriver.h"

namespace robolib
{

    TCRT5kAnalogDriver::TCRT5kAnalogDriver(uint8_t pin) : sensorPin(pin) {}


    void TCRT5kAnalogDriver::init()
    {
        pinMode(sensorPin, INPUT);
    }

    int TCRT5kAnalogDriver::getAnalogLine()
    {
        return analogRead(sensorPin);
    }


    void TCRT5kAnalogDriver::setThreshold(int value)
    {
        threshold = value;
    }

    bool TCRT5kAnalogDriver::isOnLine()
    {
        return analogRead(sensorPin) >= threshold;
    }


}
