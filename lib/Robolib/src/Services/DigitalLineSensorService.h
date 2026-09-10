#ifndef DIGITAL_LINE_SENSOR_SERVICE_H
#define DIGITAL_LINE_SENSOR_SERVICE_H

/**
 * @file DigitalLineSensorService.h
 * @brief Servicio para gestión centralizada de sensores de línea digitales.
 *
 * Fachada que simplifica el acceso a sensores digitales registrados en el robot.
 *
 * @ingroup services
 */

#include "Helper.h"
#include <cstdint>
#include <vector>
#include "../Interfaces/IDriverDigitalLineSensor.h"

namespace robolib
{

    /**
     * @class DigitalLineSensorService
     * @brief Servicio de alto nivel para sensores de línea digitales.
     *
     * Proporciona API simplificada para:
     * - Lectura de estado digital (línea sí/no)
     * - Configuración de lógica (1=línea o 0=línea)
     * - Conteo y verificación de existencia
     *
     * @code
     * auto* sensor = robot->digitalLineSensor;
     * if (sensor->exists()) {
     *     bool onLine = sensor->isOnLine(0);
     *     sensor->setIsOnLine(0, true); // 1 = línea
     * }
     * @endcode
     *
     * @ingroup services
     */
    class DigitalLineSensorService : public ModuleHelper
    {
    public:
        using ModuleHelper::ModuleHelper; // Hereda constructor

        /**
         * @brief Verifica si el sensor está sobre una línea.
         * @param index Índice del sensor (0 por defecto).
         * @return true si detecta línea, false si no / sensor no existe.
         */
        bool isOnLine(size_t index = 0) const
        {
            auto *sensor = getModule<IDriverDigitalLineSensor>(index);
            return sensor ? sensor->getLine() : false;
        }

        /**
         * @brief Cuenta sensores digitales registrados.
         * @return Número de sensores digitales.
         */
        size_t count() const
        {
            return countModules<IDriverDigitalLineSensor>();
        }

        /**
         * @brief Verifica si hay al menos un sensor digital.
         * @return true si hay sensores registrados.
         */
        bool exists() const { return count() > 0; }

        /**
         * @brief Configura la lógica de detección del sensor.
         * @param index Índice del sensor.
         * @param value true si 1 = línea, false si 0 = línea.
         */
        void setIsOnLine(size_t index = 0, bool value = true)
        {
            auto *sensor = getModule<IDriverDigitalLineSensor>(index);
            if (sensor)
                sensor->set1IsOnLine(value);
        }
    };

} // namespace robolib

#endif // DIGITAL_LINE_SENSOR_SERVICE_H