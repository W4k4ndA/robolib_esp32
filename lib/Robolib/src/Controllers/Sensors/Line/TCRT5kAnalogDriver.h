#ifndef TCRT5k_DRIVER_ANALOG_H
#define TCRT5k_DRIVER_ANALOG_H

#include "../../../Interfaces/IDriverAnalogLineSensor.h"
#include <cstdint>

namespace robolib
{

    class TCRT5kAnalogDriver : public IDriverAnalogLineSensor
    {
    private:
        uint8_t sensorPin;  // Pin GPIO/ADC del sensor
        int threshold;  // Umbral ADC para detección de línea (modo analógico)
    public:
        ~TCRT5kAnalogDriver() override = default;

        /**
         * @brief Constructor.
         *
         * @param pin Pin GPIO/ADC conectado al sensor (ej: 32 en ESP32).
         */
        TCRT5kAnalogDriver(uint8_t pin);

        void init() override;

        /**
         * @brief Obtiene el valor analógico crudo del sensor.
         *
         * En modo analógico: retorna lectura ADC (0-4095 en ESP32 12-bit).
         * En modo digital: retorna 0 (línea detectada) o 4095 (sin línea) para compatibilidad.
         *
         * @return Valor de reflectancia:
         *         - Valores bajos (~0-1000) = superficie oscura / línea detectada
         *         - Valores altos (~3000-4095) = superficie clara / sin línea
         *         - En modo digital: 0 o 4095
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
         * @note Solo afecta al modo analógico (isDigital=false).
         *       En modo digital, el umbral lo fija el potenciómetro del módulo TCRT5000.
         *
         * @see getAnalogValue() para calibrar observando valores en línea/fuera de línea.
         */
        void setThreshold(int value) override;

        /**
         * @brief Obtiene el estado digital del sensor de línea analogico.
         *
         * @return true si el valor del sensor esta por encima del umbral (threshold) y false si no
         */
        bool isOnLine() override;
    };

}

#endif // TCRT5k_DRIVER_ANALOG_H