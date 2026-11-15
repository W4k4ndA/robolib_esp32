#ifndef LED_SERVICE_H
#define LED_SERVICE_H

/**
 * @file LedService.h
 * @brief Servicio para gestión centralizada de LEDs.
 *
 * Fachada que simplifica el control de LEDs registrados en el robot.
 * Soporta LEDs simples (on/off) y LEDs con PWM para control de brillo.
 * Hereda de ModuleHelper para búsqueda y filtrado genérico.
 *
 * @ingroup services
 */

#include "../Services/Helper.h"
#include "../Interfaces/IDriverLed.h"
#include <cstdint>

namespace robolib
{

    /**
     * @class LEDService
     * @brief Servicio de alto nivel para control de LEDs.
     *
     * Proporciona API simplificada para:
     * - Encendido/apagado simple (on/off)
     * - Control de brillo via PWM (0-255)
     * - Alternancia de estado (toggle)
     * - Conteo y verificación de existencia
     *
     * @code
     * auto* led = robot->led;
     * if (led->exists()) {
     *     led->ledON(0);              // Encender LED 0
     *     led->ledBright(0, 128);     // Brillo 50% en LED 0
     *     led->ledToggle(0);          // Alternar LED 0
     *     led->ledOFF(0);             // Apagar LED 0
     * }
     * @endcode
     *
     * @ingroup services
     */
    class LEDService : public ModuleHelper
    {
    public:
        using ModuleHelper::ModuleHelper; // Hereda constructor de ModuleHelper

        /**
         * @brief Enciende el LED en el índice especificado.
         *
         * Establece el estado del LED a ON (encendido). Si el LED soporta PWM,
         * se enciende al brillo máximo (255).
         *
         * @param index Índice del LED a encender (0 por defecto para el primero).
         */
        void ledON(size_t index = 0)
        {
            auto *ledDriver = getModule<IDriverLed>(index);
            if (ledDriver)
                ledDriver->setLedState(true);
        }

        /**
         * @brief Apaga el LED en el índice especificado.
         *
         * Establece el estado del LED a OFF (apagado).
         *
         * @param index Índice del LED a apagar (0 por defecto para el primero).
         */
        void ledOFF(size_t index = 0)
        {
            auto *ledDriver = getModule<IDriverLed>(index);
            if (ledDriver)
                ledDriver->setLedState(false);
        }

        /**
         * @brief Establece la intensidad (brillo) del LED en el índice especificado.
         *
         * Controla el brillo mediante PWM. Solo funciona en LEDs conectados a pines
         * con capacidad PWM. En LEDs simples (solo on/off), valores > 0 encienden el LED.
         *
         * @param index Índice del LED a ajustar (0 por defecto para el primero).
         * @param brightness Intensidad del LED (0-255).
         *                   0 = apagado, 255 = brillo máximo.
         */
        void ledBright(size_t index = 0, uint8_t brightness = 0)
        {
            auto *ledDriver = getModule<IDriverLed>(index);
            if (ledDriver)
                ledDriver->setLedIntensity(brightness);
        }

        /**
         * @brief Alterna el estado del LED en el índice especificado.
         *
         * Si el LED está encendido, lo apaga. Si está apagado, lo enciende.
         * Útil para parpadeo simple sin necesidad de llevar control de estado.
         *
         * @param index Índice del LED a alternar (0 por defecto para el primero).
         */
        void ledToggle(size_t index = 0)
        {
            auto *ledDriver = getModule<IDriverLed>(index);
            if (ledDriver)
            {
                ledDriver->setLedState(!ledDriver->readLedState());
            }
        }

        /**
         * @brief Cuenta LEDs registrados.
         * @return Número de LEDs registrados en el robot.
         */
        size_t count() const
        {
            return countModules<IDriverLed>();
        }

        /**
         * @brief Verifica si hay al menos un LED registrado.
         * @return true si hay LEDs registrados, false en caso contrario.
         */
        bool exists() const
        {
            return count() > 0;
        }
    };

}
#endif