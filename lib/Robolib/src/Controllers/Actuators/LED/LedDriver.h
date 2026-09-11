#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include "../../../Interfaces/IDriverLed.h"

namespace robolib
{
    class LedDriver : public IDriverLed
    {
    private:
        uint8_t ledPin;

    public:
        ~LedDriver() = default;


        /**
         * @brief Inicializa el modulo de LED.
         *
         */
        void init() override;

        /**
         * @brief Constructor de la clase LedDriver.
         *
         * @param pin Pin al que está conectado el LED.
         */

        LedDriver(uint8_t pin);

        /**
         * @brief Establece el estado del LED.
         *
         * @param state Estado del LED (true = encendido, false = apagado).
         *
         */
        void setLedState(bool state) override;

        /**
         * @brief Establece la intensidad del LED.
         *
         * @param intensity Intensidad de brillo del LED (0-255).
         *
         */
        void setLedIntensity(uint8_t intensity) override;

        /**
         * @brief Lee el estado del LED.
         *
         * @return true (1) si el LED está encendido, false (0) si está apagado.
         */
        bool readLedState() override;
    };
}

#endif