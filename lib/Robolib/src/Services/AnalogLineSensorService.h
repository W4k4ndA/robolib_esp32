#ifndef ANALOG_LINE_SENSOR_SERVICE_H
#define ANALOG_LINE_SENSOR_SERVICE_H

/**
 * @file AnalogLineSensorService.h
 * @brief Servicio para gestión centralizada de sensores de línea analógicos.
 *
 * Fachada que simplifica el acceso a sensores analógicos registrados en el robot.
 * Hereda de ModuleHelper para búsqueda y filtrado genérico.
 *
 * @ingroup services
 */

#include "Helper.h"
#include <cstdint>
#include <vector>
#include "../Interfaces/IDriverAnalogLineSensor.h"

namespace robolib
{

    /**
     * @class AnalogLineSensorService
     * @brief Servicio de alto nivel para sensores de línea analógicos.
     *
     * Proporciona API simplificada para:
     * - Lectura de valores analógicos crudos
     * - Detección de línea basada en umbral configurable
     * - Configuración de umbrales
     *
     * @code
     * auto* sensor = robot->analogLineSensor;
     * if (sensor->exists()) {
     *     uint16_t raw = sensor->isOnLine(0);
     *     bool onLine = sensor->isLine(0);
     *     sensor->updateThreshold(0, 2000);
     * }
     * @endcode
     *
     * @ingroup services
     */
    class AnalogLineSensorService : public ModuleHelper
    {
    public:
        using ModuleHelper::ModuleHelper; // Hereda constructor

        /**
         * @brief Obtiene el valor analógico crudo del sensor.
         * @param index Índice del sensor (0 por defecto).
         * @return Valor ADC (0-4095), 0 si sensor no existe.
         */
        uint16_t isOnLine(size_t index = 0) const
        {
            auto *sensor = getModule<IDriverAnalogLineSensor>(index);
            return sensor ? sensor->getAnalogLine() : 0;
        }

        /**
         * @brief Cuenta sensores analógicos registrados.
         * @return Número de sensores analógicos.
         */
        size_t count() const
        {
            return countModules<IDriverAnalogLineSensor>();
        }

        /**
         * @brief Verifica si hay al menos un sensor analógico.
         * @return true si hay sensores registrados.
         */
        bool exists() const { return count() > 0; }

        /**
         * @brief Actualiza el umbral de detección de un sensor.
         * @param index Índice del sensor (0 por defecto).
         * @param value Nuevo valor umbral ADC (0-4095).
         */
        void updateThreshold(size_t index = 0, int value = 800)
        {
            auto *sensor = getModule<IDriverAnalogLineSensor>(index);
            if (sensor)
                sensor->setThreshold(value);
        }

        /**
         * @brief Verifica si el sensor está sobre la línea (basado en umbral).
         * @param index Índice del sensor.
         * @return true si valor analógico >= threshold (línea detectada).
         */
        bool isLine(size_t index = 0)
        {
            auto *sensor = getModule<IDriverAnalogLineSensor>(index);
            if (sensor)
                return sensor->isOnLine();
            return false;
        }
    };

} // namespace robolib

#endif // ANALOG_LINE_SENSOR_SERVICE_H