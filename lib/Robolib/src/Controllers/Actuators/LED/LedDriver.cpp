#include <Arduino.h>
#include "LedDriver.h"

namespace robolib
{

    void LedDriver::init()
    {
        pinMode(ledPin, OUTPUT);
    }

    LedDriver::LedDriver(uint8_t pin) : ledPin(pin)
    {
        pinMode(ledPin, OUTPUT);
    }

    void LedDriver::setLedState(bool state)
    {
        digitalWrite(ledPin, state);
    }

    void LedDriver::setLedIntensity(uint8_t intensity)
    {
        analogWrite(ledPin, intensity);
    }

    bool LedDriver::readLedState()
    {
        return digitalRead(ledPin);
    }

}