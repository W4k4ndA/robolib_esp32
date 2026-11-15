#ifndef BUTTON_SERVICE_H
#define BUTTON_SERVICE_H

/**
 * @file ButtonService.h
 * @brief Servicio para gestión centralizada de botones/pulsadores.
 *
 * Fachada que simplifica el acceso a botones registrados en el robot.
 * Hereda de ModuleHelper para búsqueda y filtrado genérico.
 *
 * @ingroup services
 */

#include "Helper.h"
#include "../Interfaces/IDriverButton.h"

namespace robolib
{

    /**
     * @class ButtonService
     * @brief Servicio de alto nivel para botones/pulsadores.
     *
     * Proporciona API simplificada para:
     * - Lectura de estado actual (presionado/soltado)
     * - Detección de flanco de subida (wasPressed - detecta si fue presionado en el ciclo anterior)
     * - Conteo y verificación de existencia
     *
     * @code
     * auto* btn = robot->button;
     * if (btn->exists()) {
     *     if (btn->isPressed(0)) { ... }        // Estado actual
     *     if (btn->wasPressed(0)) { ... }       // Flanco: se presionó en este ciclo
     * }
     * @endcode
     *
     * @ingroup services
     */
    class ButtonService : public ModuleHelper
    {
    public:
        using ModuleHelper::ModuleHelper; // Hereda constructor

        /**
         * @brief Cuenta botones registrados.
         * @return Número de botones registrados en el robot.
         */
        size_t count() const
        {
            return countModules<IDriverButton>();
        }

        /**
         * @brief Verifica si hay al menos un botón registrado.
         * @return true si hay botones registrados, false en caso contrario.
         */
        bool exists() const
        {
            return count() > 0;
        }

        /**
         * @brief Lee el estado actual del botón (presionado o no).
         *
         * Realiza una lectura instantánea del estado físico del botón.
         * Útil para detectar si el botón está siendo mantenido presionado.
         *
         * @param index Índice del botón (0 por defecto para el primero).
         * @return true si el botón está presionado ahora, false si no lo está o no existe.
         */
        bool isPressed(size_t index = 0) const
        {
            IDriverButton* button = getModule<IDriverButton>(index);
            if (button)
                return button->readButton();
            return false;
        }

        /**
         * @brief Detecta si el botón fue presionado en el ciclo anterior (flanco de subida).
         *
         * Compara el estado actual con el anterior para detectar el momento exacto
         * en que el botón pasa de no presionado a presionado. Útil para acciones
         * que deben ejecutarse una sola vez por pulsación (ej. cambiar modo, confirmar).
         *
         * @param index Índice del botón (0 por defecto para el primero).
         * @return true si el botón acaba de ser presionado (flanco subida), false en caso contrario.
         */
        bool wasPressed(size_t index = 0) const
        {
            auto button = getModule<IDriverButton>(index);
            if (button)
                return button->lastStateButton();
            return false;
        }
    };

}
#endif /* BUTTON_SERVICE_H */