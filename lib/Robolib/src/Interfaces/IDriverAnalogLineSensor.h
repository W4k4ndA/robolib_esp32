#ifndef I_DRIVER_ANALOG_LINE_SENSOR_H
#define I_DRIVER_ANALOG_LINE_SENSOR_H

#include "IRobotModule.h"

/**
 * @interface IDriverAnalogLineSensor
 * @brief Interfaz abstracta para controladores de sensores de línea analogicos.
 *
 * Esta interfaz define el contrato para las implementaciones de sensores de línea analogicos.
 * Los sensores de línea detectan si un robot está sobre una línea (típicamente una línea
 * oscura sobre superficie clara o viceversa) proporcionan lecturas analógicas
 * para un posicionamiento más preciso que solo true o false.
 */

namespace robolib
{
  class IDriverAnalogLineSensor : virtual public IRobotModule
  {
  private:
    bool threshold;

  public:
    /**
     * @brief Destructor virtual para limpieza adecuada de clases derivadas.
     */
    virtual ~IDriverAnalogLineSensor() = default;

    /**
     * @brief Obtiene el valor analógico crudo del sensor de línea.
     *
     * @return La lectura analógica del sensor. Valores más altos típicamente
     *         indican superficies más brillantes (fuera de línea), valores más bajos
     *         indican superficies más oscuras (sobre línea), pero esto depende del sensor específico.
     */
    virtual int getAnalogLine() = 0;

    /**
     * @brief Establece el umbral de detección para el sensor de línea.
     *
     * @param value El valor umbral a establecer. Este valor determina el límite
     *              entre lo que se considera "sobre línea" y "fuera de línea".
     */
    virtual void setThreshold(int value) = 0;

    /**
     * @brief Obtiene el estado digital del sensor de línea analogico.
     *
     * @return true si el valor del sensor esta por encima del umbral (threshold) y false si no
     */
    virtual bool getIsOnLine() = 0;
  };
}

#endif