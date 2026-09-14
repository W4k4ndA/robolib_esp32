#ifndef I_DRIVER_BUTTON_H
#define I_DRIVER_BUTTON_H

#include "IRobotModule.h"
#include <stdint.h>

namespace robolib
{

    class IDriverButton : IRobotModule
    {
    public:
        /**
         * @brief Destructor virtual por defecto.
         *
         * Necesario para destrucción correcta de objetos derivados mediante
         * puntero a la interfaz (polimorfismo).
         */
        virtual ~IDriverButton() = default;
        /**
         * @brief Obtiene el tipo de módulo.
         * @return ModuleType::Button
         */
        static constexpr ModuleType staticType() { return ModuleType::Button; }
        ModuleType getType() const override { return staticType(); }

        virtual bool readButton(uint8_t pin) = 0;
    };

}

#endif