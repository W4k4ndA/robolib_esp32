#ifndef I_DRIVER_LED_H
#define I_DRIVER_LED_H

#include "IRobotModule.h"
#include <cstdint>

namespace robolib
{

    class IDriverLed : public IRobotModule
    {
    public:
        /**
         * @brief Destructor virtual por defecto.
         *
         * Necesario para destrucción correcta de objetos derivados mediante
         * puntero a la interfaz (polimorfismo).
         */
        virtual ~IDriverLed() = default;

        /**
         * @brief Obtiene el tipo de módulo.
         *
         * @return ModuleType::LED
         */
        static constexpr ModuleType staticType() { return ModuleType::LED; }
        ModuleType getType() const override { return staticType(); }

        /**
         * @brief Establece el estado del LED.
         *
         * @param state Estado del LED (true = encendido, false = apagado).
         *
         */
        virtual void setLedState(bool state) = 0;

        /**
         * @brief Establece la intensidad del LED.
         *
         * @param intensity Intensidad de brillo del LED (0-255).
         */
        virtual void setLedIntensity(uint8_t intensity) = 0; // Añadir método para controlar intensidad (0-255)
        
        /**
         * @brief Lee el estado actual del LED.
         *
         * @return Estado actual del LED (true = encendido, false = apagado).
         *
         */
        virtual bool readLedState() = 0;
    };

}

#endif /* I_DRIVER_LED_H */