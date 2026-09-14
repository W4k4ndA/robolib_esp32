#include <Arduino.h>
#include "ButtonSensorDriver.h"

namespace robolib
{

    ButtonSensorDriver::ButtonSensorDriver(uint8_t pin): buttonPin(pin)
    {
    }

    void ButtonSensorDriver::init()
    {
        //se pueden usar resistencias de pullup externas
        pinMode(buttonPin, INPUT_PULLUP);
    }

    bool ButtonSensorDriver::readButton() {
        lastButtonState = digitalRead(buttonPin);
        return lastButtonState;

    }

    bool ButtonSensorDriver::lastStateButton() const{
        return lastButtonState;
    }

}