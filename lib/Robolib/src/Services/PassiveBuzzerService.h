#ifndef PASSIVE_BUZZER_SERVICE_H
#define PASSIVE_BUZZER_SERVICE_H

/**
 * @file PassiveBuzzerService.h
 * @brief Servicio para gestión centralizada de zumbadores pasivos (buzzers).
 *
 * Fachada que simplifica el control de buzzers pasivos registrados en el robot.
 * Un buzzer pasivo requiere una señal PWM de frecuencia variable para generar tonos.
 * Hereda de ModuleHelper para búsqueda y filtrado genérico.
 *
 * @ingroup services
 */

#include <cstdint>
#include "Helper.h"
#include "../Interfaces/IDriverPassiveBuzzer.h"

namespace robolib
{

    /**
     * @class PassiveBuzzerService
     * @brief Servicio de alto nivel para zumbadores pasivos.
     *
     * Proporciona API simplificada para:
     * - Generación de tonos por frecuencia (Hz)
     * - Conteo y verificación de existencia
     *
     * Nota: Para silenciar el buzzer, llamar a sound() con frequency = 0
     * o usar el método stop() del driver directamente si está disponible.
     *
     * @code
     * auto* buzzer = robot->buzzer;
     * if (buzzer->exists()) {
     *     buzzer->sound(0, 440);   // Tono La4 (440 Hz)
     *     buzzer->sound(0, 880);   // Tono La5 (880 Hz)
     *     buzzer->sound(0, 0);     // Silencio
     * }
     * @endcode
     *
     * @ingroup services
     */
    class PassiveBuzzerService : public ModuleHelper
    {
    public:
        using ModuleHelper::ModuleHelper;

        /**
         * @brief Genera un tono en el buzzer pasivo.
         *
         * Envía una señal PWM de la frecuencia especificada al buzzer.
         * Para silenciar, pasar frequency = 0.
         *
         * @param index Índice del buzzer (0 por defecto para el primero).
         * @param frequency Frecuencia del tono en Hz (0 = silencio, rango típico 20-20000 Hz).
         *                  Valores comunes: 262 (Do4), 440 (La4), 523 (Do5), 880 (La5).
         */
        void sound(size_t index = 0, uint16_t frequency = 440)
        {
            auto *buzzerDriver = getModule<IDriverPassiveBuzzer>(index);
            if (buzzerDriver)
            {
                buzzerDriver->sound(frequency);
            }
        }

        /**
         * @brief Cuenta buzzers pasivos registrados.
         * @return Número de buzzers pasivos registrados en el robot.
         */
        size_t count() const
        {
            return countModules<IDriverPassiveBuzzer>();
        }

        /**
         * @brief Verifica si hay al menos un buzzer pasivo registrado.
         * @return true si hay buzzers registrados, false en caso contrario.
         */
        bool exists() const
        {
            return count() > 0;
        }
    };

}
#endif