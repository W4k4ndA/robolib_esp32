#ifndef I_DRIVER_DIGITAL_LINE_SENSOR_H
#define I_DRIVER_DIGITAL_LINE_SENSOR_H

#include "IRobotModule.h"

/**
 * @brief Interfaz para controladores de sensores de línea digitales.
 *
 * Esta interfaz define el contrato para las implementaciones de sensores de línea digitales.
 * Los sensores de línea digitales detectan si un robot está sobre una línea (típicamente una línea
 * oscura sobre superficie clara o viceversa) y proporcionan una salida digital (0 o 1).
 *
 */

namespace robolib
{

    class IDriverDigitalLineSensor : virtual public IRobotModule
    {

    private:
        bool is1onLine;

    public:
        /**
         * @brief Destructor virtual por defecto.
         */
        virtual ~IDriverDigitalLineSensor() = default;

        /**
         * @brief Obtiene el estado del sensor de línea digital.
         * @return true si el sensor detecta una línea, false en caso contrario.
         */
        virtual bool getLine() = 0;

        /**
         * @brief Establece el comportamiento del sensor
         *  indicando que valor devuelve el sensor si esta sobre la linea
         * @param is1onLine true si el sensor devuelve 1 si esta en la linea,
         * false en caso contrario (devuelve 0) si esta sobre la linea.
         */
        virtual void set1IsOnLine(bool value = true) = 0;
    };
}

#endif