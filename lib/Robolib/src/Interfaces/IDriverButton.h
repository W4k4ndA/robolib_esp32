#ifndef I_DRIVER_BUTTON_H
#define I_DRIVER_BUTTON_H

#include "IRobotModule.h"
#include <stdint.h>

namespace robolib
{

    class IDriverButton : public IRobotModule
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

        /**
         * @brief Lee el estado del botón.
         * @return true si el botón está presionado, false en caso contrario.
         */
        virtual bool readButton() = 0;

        /**
         * @brief Lee el estado anterior del botón.
         * @return true si el botón estaba presionado, false en caso contrario.
         */
        virtual bool lastStateButton() const = 0;
    };

}

#endif