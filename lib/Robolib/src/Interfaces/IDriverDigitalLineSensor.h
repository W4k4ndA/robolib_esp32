#ifndef I_DRIVER_DIGITAL_LINE_SENSOR_H
#define I_DRIVER_DIGITAL_LINE_SENSOR_H

/**
 * @file IDriverDigitalLineSensor.h
 * @brief Interfaz para controladores de sensores de línea digitales.
 *
 * Define el contrato para sensores de línea que proporcionan salida digital (0/1).
 * Típicamente módulos TCRT5000 en modo digital (salida DO con comparador interno).
 *
 * @ingroup interfaces
 */

#include "IRobotModule.h"

namespace robolib
{

    /**
     * @class IDriverDigitalLineSensor
     * @brief Interfaz base para sensores de línea digitales (salida 0/1).
     *
     * Los sensores digitales detectan presencia/ausencia de línea retornando
     * un valor booleano. El umbral de detección se configura via hardware
     * (potenciómetro en el módulo TCRT5000).
     *
     * @see TCRT5kDigitalDriver, IDriverAnalogLineSensor
     * @ingroup interfaces
     */
    class IDriverDigitalLineSensor : public IRobotModule
    {
    private:
        bool is1onLine;  ///< Estado interno: true si 1 = línea detectada

    public:
        /**
         * @brief Destructor virtual por defecto.
         */
        virtual ~IDriverDigitalLineSensor() = default;

        /**
         * @brief Obtiene el tipo de módulo.
         * @return ModuleType::DigitalLineSensor
         */
        static constexpr ModuleType staticType() { return ModuleType::DigitalLineSensor; }
        ModuleType getType() const override { return staticType(); }

        /**
         * @brief Obtiene el estado del sensor de línea.
         * @return true si detecta línea, false en caso contrario.
         * @note La interpretación (1=línea o 0=línea) depende de set1IsOnLine().
         */
        virtual bool getLine() = 0;

        /**
         * @brief Configura la lógica de detección.
         * @param value true si el sensor retorna 1 cuando está sobre la línea,
         *              false si retorna 0 cuando está sobre la línea.
         * @note Llamar una vez tras init() para calibrar según el sensor.
         */
        virtual void set1IsOnLine(bool value = true) = 0;
    };

} // namespace robolib

#endif // I_DRIVER_DIGITAL_LINE_SENSOR_H