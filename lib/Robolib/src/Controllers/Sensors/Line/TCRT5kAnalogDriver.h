#ifndef TCRT5k_DRIVER_ANALOG_H
#define TCRT5k_DRIVER_ANALOG_H

/**
 * @file TCRT5kAnalogDriver.h
 * @brief Driver para sensor de línea TCRT5000 en modo analógico (ADC).
 *
 * Implementa IDriverAnalogLineSensor para lectura de reflectancia via ADC.
 * Permite detección precisa de posición sobre la línea.
 *
 * @ingroup drivers
 */

#include "../../../Interfaces/IDriverAnalogLineSensor.h"
#include <cstdint>

namespace robolib
{

    /**
     * @class TCRT5kAnalogDriver
     * @brief Driver para sensor TCRT5000 en modo analógico.
     *
     * Lee la salida analógica (OUT) del sensor TCRT5000 via ADC del ESP32.
     * Proporciona valor proporcional a la reflectancia, permitiendo:
     * - Detección de línea (umbral configurable)
     * - Posicionamiento relativo (centro, borde izquierdo/derecho)
     *
     * Conexión típica:
     * - VCC -> 3.3V / 5V
     * - GND -> GND
     * - OUT (analógico) -> Pin ADC (ej: GPIO 32 en ESP32)
     *
     * @note Requiere pin capaz de ADC (en ESP32-C3: GPIO 1-10 recomendados).
     * @see IDriverAnalogLineSensor, TCRT5kDigitalDriver
     * @ingroup drivers
     */
    class TCRT5kAnalogDriver : public IDriverAnalogLineSensor
    {
    private:
        uint8_t sensorPin;  ///< Pin GPIO/ADC conectado a la salida analógica del sensor
        int threshold;      ///< Umbral ADC para detección de línea (0-4095)

    public:
        /**
         * @brief Destructor por defecto.
         */
        ~TCRT5kAnalogDriver() override = default;

        /**
         * @brief Constructor.
         * @param pin Pin GPIO/ADC conectado a la salida analógica del sensor.
         * @note No configura hardware; llamar a init() después.
         */
        TCRT5kAnalogDriver(uint8_t pin);

        /**
         * @brief Inicializa el hardware del sensor.
         *
         * Configura el pin como entrada ADC.
         * Debe llamarse una vez en setup() antes de usar el sensor.
         */
        void init() override;

        /**
         * @brief Obtiene el valor analógico crudo del sensor.
         *
         * @return Lectura ADC (0-4095 en ESP32 12-bit).
         *         - Valores bajos (~0-1000) = superficie oscura / línea detectada
         *         - Valores altos (~3000-4095) = superficie clara / sin línea
         *
         * @note Función no bloqueante, lectura inmediata.
         */
        int getAnalogLine() override;

        /**
         * @brief Establece el umbral de detección para modo analógico.
         *
         * @param value Umbral ADC (0-4095). Valores por debajo = línea detectada.
         *              Típico: 1500-2500 según calibración y superficie.
         *
         * @note Solo afecta al modo analógico.
         *       En modo digital, el umbral lo fija el potenciómetro del módulo.
         * @see isOnLine() para usar el umbral en detección digital.
         */
        void setThreshold(int value) override;

        /**
         * @brief Verifica si el sensor está sobre la línea (basado en umbral).
         *
         * @return true si valor analógico >= threshold (línea detectada).
         */
        bool isOnLine() override;
    };

} // namespace robolib

#endif // TCRT5k_DRIVER_ANALOG_H