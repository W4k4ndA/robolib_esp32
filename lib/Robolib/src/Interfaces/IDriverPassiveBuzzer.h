#ifndef I_DRIVER_PASIVE_BUZZER_H
#define I_DRIVER_PASIVE_BUZZER_H

#include "IRobotModule.h"
#include <stdint.h>

namespace robolib
{

    class IDriverPassiveBuzzer : public IRobotModule
    {
    public:
        /**
         * @brief Destructor virtual por defecto.
         *
         * Necesario para destrucción correcta de objetos derivados mediante
         * puntero a la interfaz (polimorfismo).
         */
        virtual ~IDriverPassiveBuzzer() = default;

        static constexpr ModuleType staticType() { return ModuleType::PassiveBuzzer; }
        ModuleType getType() const override { return staticType(); }

        /**
         * @brief Establece la frecuencia del sonido a generar con el zumbador.
         *
         * @param frequency Frecuencia en Hz del sonido a producir
         */
        virtual void sound(uint16_t frequency) = 0;
    };

}

#endif