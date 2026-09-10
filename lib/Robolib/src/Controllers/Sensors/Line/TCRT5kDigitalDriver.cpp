#include "TCRT5kDigitalDriver.h"
#include <Arduino.h>

namespace robolib
{

    /**
     * @file TCRT5kDigitalDriver.cpp
     * @brief Implementación del driver digital TCRT5000.
     *
     * @ingroup drivers
     */

    /**
     * @brief Constructor.
     * @param pin Pin GPIO conectado a la salida digital (DO).
     */
    TCRT5kDigitalDriver::TCRT5kDigitalDriver(uint8_t pin) : sensorPin(pin) {}

    /**
     * @brief Inicializa el hardware del sensor.
     *
     * Configura el pin como entrada digital.
     */
    void TCRT5kDigitalDriver::init()
    {
        pinMode(sensorPin, INPUT);
    }

    /**
     * @brief Actualización periódica (vacía - no requiere polling continuo).
     */
    void TCRT5kDigitalDriver::update()
    {
        // No requiere implementación - implementación por defecto vacía en IRobotModule
    }

    /**
     * @brief Configura la lógica de detección.
     * @param value true si 1 = línea, false si 0 = línea.
     */
    void TCRT5kDigitalDriver::set1IsOnLine(bool value)
    {
        is1onLine = value;
    }

    /**
     * @brief Verifica si el sensor está sobre una línea.
     *
     * Compara la lectura digital con la configuración is1onLine.
     *
     * @return true si (lectura == is1onLine), false en caso contrario.
     */
    bool TCRT5kDigitalDriver::getLine()
    {
        return (is1onLine == digitalRead(sensorPin));
    }

} // namespace robolib