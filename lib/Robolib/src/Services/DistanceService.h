#ifndef DISTANCE_SERVICE_H
#define DISTANCE_SERVICE_H

/**
 * @file DistanceService.h
 * @brief Servicio para gestión centralizada de sensores de distancia.
 *
 * Fachada que encapsula la interacción con sensores de distancia registrados.
 * Proporciona API simplificada para mediciones en centímetros.
 *
 * @ingroup services
 */

#include <vector>
#include <cstddef>
#include "../Interfaces/IRobotModule.h"
#include "../Interfaces/IDriverDistanceSensor.h"
#include "Helper.h"

namespace robolib
{

    /**
     * @class DistanceService
     * @brief Servicio de alto nivel para sensores de distancia.
     *
     * Proporciona API simplificada para:
     * - Medición de distancia en centímetros
     * - Conteo y verificación de existencia de sensores
     *
     * @code
     * auto* dist = robot->distanceSensor;
     * if (dist->exists()) {
     *     float d = dist->getCM(0);  // Primer sensor
     *     float d2 = dist->getCM(1); // Segundo sensor
     * }
     * @endcode
     *
     * @ingroup services
     */
    class DistanceService : public ModuleHelper
    {
    public:
        using ModuleHelper::ModuleHelper; // Hereda constructor

        /**
         * @brief Obtiene la medición en cm de un sensor específico.
         *
         * @param index Índice del sensor (0 por defecto para el primero).
         * @return Distancia en cm o valor negativo en caso de error:
         *         - -1.0f: timeout (sin eco / objeto muy lejos)
         *         - -2.0f: error de hardware / comunicación
         *         - -3.0f: fuera de rango válido del sensor
         */
        float getCM(size_t index = 0) const
        {
            auto *sensor = getModule<IDriverDistanceSensor>(index);
            return sensor ? sensor->measureCM() : -1.0f;
        }

        /**
         * @brief Cuenta cuántos sensores de distancia están registrados.
         * @return Número de sensores de distancia.
         */
        size_t count() const
        {
            return countModules<IDriverDistanceSensor>();
        }

        /**
         * @brief Verifica si hay al menos un sensor de distancia.
         * @return true si hay sensores registrados.
         */
        bool exists() const { return count() > 0; }
    };

} // namespace robolib

#endif // DISTANCE_SERVICE_H