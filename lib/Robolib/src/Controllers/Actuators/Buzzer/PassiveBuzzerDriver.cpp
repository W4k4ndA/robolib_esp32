#include "PassiveBuzzerDriver.h"
#include <Arduino.h>

namespace robolib
{

    PassiveBuzzerDriver::PassiveBuzzerDriver(uint8_t pin) : buzzerPin(pin)
    {
    }
    void PassiveBuzzerDriver::init()
    {
        pinMode(buzzerPin, OUTPUT);
    }

    void PassiveBuzzerDriver::sound(uint16_t frequency)
    {
        tone(buzzerPin, frequency);
    }


}