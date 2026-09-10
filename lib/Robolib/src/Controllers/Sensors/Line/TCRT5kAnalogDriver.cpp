#include <Arduino.h>
#include "TCRT5kAnalogDriver.h"

namespace robolib
{

    /**
     * @file TCRT5kAnalogDriver.cpp
     * @brief Implementación del driver analógico TCRT5000.
     *
     * @ingroup drivers
     */

    /**
     * @brief Constructor.
     * @param pin Pin GPIO/ADC conectado al sensor.
     */
    TCRT5kAnalogDriver::TCRT5kAnalogDriver(uint8_t pin) : sensorPin(pin) {}

    /**
     * @brief Inicializa el hardware del sensor.
     *
     * Configura el pin como entrada ADC.
     */
    void TCRT5kAnalogDriver::init()
    {
        pinMode(sensorPin, INPUT);
    }

    /**
     * @brief Obtiene el valor analógico crudo del sensor.
     * @return Lectura ADC (0-4095).
     */
    int TCRT5kAnalogDriver::getAnalogLine()
    {
        return analogRead(sensorPin);
    }

    /**
     * @brief Establece el umbral de detección.
     * @param value Nuevo valor umbral (0-4095).
     */
    void TCRT5kAnalogDriver::setThreshold(int value)
    {
        threshold = value;
    }

    /**
     * @brief Verifica si está sobre la línea según umbral.
     * @return true si lectura ADC >= threshold.
     */
    bool TCRT5kAnalogDriver::isOnLine()
    {
        return analogRead(sensorPin) >= threshold;
    }

} // namespace robolib