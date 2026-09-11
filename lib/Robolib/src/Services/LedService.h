#ifndef LED_SERVICE_H
#define LED_SERVICE_H

#include "../Services/Helper.h"
#include "../Interfaces/IDriverLed.h"
#include <cstdint>

namespace robolib
{

    class LEDService : public ModuleHelper
    {
    public:
        using ModuleHelper::ModuleHelper; // Hereda constructor de ModuleHelper

        /**
         * @brief Enciende el LED en el índice especificado.
         * @param index Índice del LED a encender. Por defecto es 0.
         */
        void ledON(size_t index = 0)
        {
            auto *ledDriver = getModule<IDriverLed>(index);
            if (ledDriver)
                ledDriver->setLedState(true);
        }

        /**
         * @brief Apaga el LED en el índice especificado.
         * @param index Índice del LED a apagar. Por defecto es 0.
         */
        void ledOFF(size_t index = 0)
        {
            auto *ledDriver = getModule<IDriverLed>(index);
            if (ledDriver)
                ledDriver->setLedState(false);
        }


        /**
         * @brief Establece la intensidad del LED en el índice especificado.
         * @param index Índice del LED a ajustar. Por defecto es 0.
         * @param intensity Intensidad del LED (0-255).
         */
        void ledBrigth(size_t index = 0, uint8_t intensity)
        {
            auto *ledDriver = getModule<IDriverLed>(index);
            if (ledDriver)
                ledDriver->setLedIntensity(intensity);
        }

        /**
         * @brief Alterna el estado del LED en el índice especificado.
         * @param index Índice del LED a alternar. Por defecto es 0.
         */
        void ledToggle(size_t index = 0)
        {
            auto *ledDriver = getModule<IDriverLed>(index);
            if (ledDriver)
            {
                ledDriver->setLedState(!ledDriver->readLedState());
            }
        }

        size_t count() const
        {
            return countModules<IDriverLed>();
        }

        bool exists() const
        {
            return count() > 0;
        }
   
    };

}

#endif