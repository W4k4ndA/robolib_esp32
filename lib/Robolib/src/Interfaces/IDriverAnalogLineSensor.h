#ifndef I_DRIVER_ANALOG_LINE_SENSOR_H
#define I_DRIVER_ANALOG_LINE_SENSOR_H

/**
 * @file IDriverAnalogLineSensor.h
 * @brief Interfaz para controladores de sensores de línea analógicos.
 *
 * Define el contrato para sensores de línea que proporcionan lectura analógica
 * (valor ADC). Permite posicionamiento más preciso que solo true/false.
 *
 * @ingroup interfaces
 */

#include "IRobotModule.h"

namespace robolib
{

    /**
     * @class IDriverAnalogLineSensor
     * @brief Interfaz base para sensores de línea analógicos (salida ADC).
     *
     * Los sensores analógicos retornan un valor proporcional a la reflectancia,
     * permitiendo detección de posición relativa sobre la línea (centro, bordes).
     * El umbral de detección es configurable por software via setThreshold().
     *
     * @see TCRT5kAnalogDriver, IDriverDigitalLineSensor
     * @ingroup interfaces
     */
    class IDriverAnalogLineSensor : public IRobotModule
    {
    private:
        bool threshold;  ///< Valor umbral para detección de línea (configurado via setThreshold)

    public:
        /**
         * @brief Destructor virtual por defecto.
         */
        virtual ~IDriverAnalogLineSensor() = default;

        /**
         * @brief Obtiene el tipo de módulo.
         * @return ModuleType::AnalogLineSensor
         */
        static constexpr ModuleType staticType() { return ModuleType::AnalogLineSensor; }
        ModuleType getType() const override { return staticType(); }

        /**
         * @brief Obtiene el valor analógico crudo del sensor.
         * @return Lectura ADC (0-4095 en ESP32 12-bit).
         *         Valores bajos = superficie oscura / línea detectada.
         *         Valores altos = superficie clara / fuera de línea.
         */
        virtual int getAnalogLine() = 0;

        /**
         * @brief Establece el umbral de detección de línea.
         * @param value Valor umbral ADC (0-4095). Valores por debajo = línea.
         * @note Típico: 1500-2500 según calibración y superficie.
         */
        virtual void setThreshold(int value) = 0;

        /**
         * @brief Obtiene estado digital basado en umbral.
         * @return true si valor analógico >= threshold (línea detectada).
         */
        virtual bool isOnLine() = 0;
    };

} // namespace robolib

#endif // I_DRIVER_ANALOG_LINE_SENSOR_H