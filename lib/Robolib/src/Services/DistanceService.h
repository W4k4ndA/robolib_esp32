#ifndef DISTANCE_SERVICE_H
#define DISTANCE_SERVICE_H

#include <vector>
#include <cstddef>
#include "../Interfaces/IRobotModule.h"
#include "../Interfaces/IDriverDistanceSensor.h"
#include "Helper.h"

namespace robolib
{

    /**
     * @brief Servicio para gestión centralizada de sensores de distancia.
     *Este servicio encapsula toda la interacción con los sensores de
     *distancia. Aquí vive la lógica de fachada especializada.
     * Proporciona acceso simplificado a sensores de distancia registrados en el robot.
     * @ingroup services
     */

    class DistanceService : public ModuleHelper
    {
    public:
        using ModuleHelper::ModuleHelper; // Hereda constructores
        /**
         * @brief Obtiene la medición en cm de un sensor específico.
         * @param index Índice del sensor (0 por defecto para el primero).
         * @return Distancia en cm o valor negativo en caso de error.
         * @note Retorna valor negativo en caso de error:
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
         */
        size_t count() const
        {
           
            return countModules<IDriverDistanceSensor>();
        }

        bool exists() const { return count() > 0; }
    };

} // namespace robolib

#endif // DISTANCE_SERVICE_H