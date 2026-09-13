#ifndef PASSIVE_BUZZER_DRIVER_H
#define PASSIVE_BUZZER_DRIVER_H

#include "../../../Interfaces/IDriverPassiveBuzzer.h"
#
#include <stdint.h>

namespace robolib
{

    class PassiveBuzzerDriver : public IDriverPassiveBuzzer
    {
    private:
        uint8_t buzzerPin;

    public:
        /**
         * @brief Destructor de la clase PasiveBuzzerDriver.
         */
        ~PassiveBuzzerDriver() override = default;

        /**
         * @brief Constructor de la clase PasiveBuzzerDriver.
         *
         * @param pin Pin al que está conectado el zumbador pasivo.
         */
        PassiveBuzzerDriver(uint8_t pin);

        /**
         * @brief Inicializa el zumbador pasivo.
         *
         */
        void init() override;

        /**
         * @brief Establece la frecuencia del sonido a generar con el zumbador.
         *
         * @param frequency Frecuencia en Hz del sonido a producir
         */
        void sound(uint16_t frequency) override;
    };

}

#endif