#ifndef TCRT5000_SENSOR_DRIVER_H
#define TCRT5000_SENSOR_DRIVER_H

/**
 * @file TCRT5kDigitalDriver.h
 * @brief Driver para sensor de línea TCRT5000 en modo digital (GPIO).
 *
 * Implementa IDriverDigitalLineSensor para lectura digital (0/1) del sensor TCRT5000.
 * Usa la salida DO del módulo con comparador interno y potenciómetro de umbral.
 *
 * @ingroup drivers
 */

#include "../../../Interfaces/IDriverDigitalLineSensor.h"
#include <cstdint>

namespace robolib
{

    /**
     * @class TCRT5kDigitalDriver
     * @brief Driver para sensor TCRT5000 en modo digital.
     *
     * Lee la salida digital (DO) del módulo TCRT5000.
     * El módulo incluye un comparador interno con potenciómetro para ajuste de umbral.
     *
     * Conexión típica:
     * - VCC -> 3.3V / 5V
     * - GND -> GND
     * - DO (digital) -> Pin GPIO (ej: GPIO 2)
     *
     * @note El umbral se ajusta via potenciómetro físico en el módulo.
     * @see IDriverDigitalLineSensor, TCRT5kAnalogDriver
     * @ingroup drivers
     */
    class TCRT5kDigitalDriver : public IDriverDigitalLineSensor
    {
    private:
        uint8_t sensorPin;      ///< Pin GPIO conectado a la salida digital (DO) del sensor
        bool is1onLine = false; ///< true si 1 = línea, false si 0 = línea

    public:
        /**
         * @brief Constructor.
         * @param pin Pin GPIO conectado a la salida digital (DO) del sensor.
         * @note No configura hardware; llamar a init() después.
         */
        TCRT5kDigitalDriver(uint8_t pin);

        /**
         * @brief Destructor por defecto.
         */
        ~TCRT5kDigitalDriver() override = default;

        /**
         * @brief Inicializa el hardware del sensor.
         *
         * Configura el pin como entrada digital.
         * Debe llamarse una vez en setup() antes de usar el sensor.
         */
        void init() override;

        /**
         * @brief Actualización periódica (opcional, implementación vacía).
         */
        void update() override;

        /**
         * @brief Verifica si el sensor está actualmente sobre una línea.
         *
         * En modo digital: lee estado GPIO directo (el módulo TCRT5000 tiene
         * comparador interno + potenciómetro para umbral).
         *
         * @return true si detecta línea (según configuración is1onLine),
         *         false si no detecta línea / sensor no existe.
         */
        bool getLine() override;

        /**
         * @brief Configura la lógica de detección.
         *
         * @param value true si el sensor retorna 1 cuando está sobre la línea,
         *              false si retorna 0 cuando está sobre la línea.
         *
         * @note Llamar una vez tras init() para calibrar según el sensor.
         */
        void set1IsOnLine(bool value = true) override;
    };

} // namespace robolib

#endif // TCRT5000_SENSOR_DRIVER_H