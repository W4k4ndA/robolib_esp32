#ifndef BUTTON_SENSOR_DRIVER_H
#define BUTTON_SENSOR_DRIVER_H

#include "../../../Interfaces/IDriverButton.h"

namespace robolib
{

    class ButtonSensorDriver : public IDriverButton
    {

    private:
        bool lastButtonState = false;
        uint8_t buttonPin;

    public:
    /**
     * @brief Constructor de la clase ButtonSensorDriver.
     *
     * @param pin Pin digital al que está conectado el botón.
     */
        ButtonSensorDriver(uint8_t pin);

        /**
         * @brief Destructor de la clase ButtonSensorDriver.
         */
        ~ButtonSensorDriver() = default;

        /**
         * @brief Inicializa el módulo.
         */
        void init() override;

        /**
         * @brief Lee el estado del botón.
         * @return true si el botón está presionado, false en caso contrario.
         */
        bool readButton() override;

        bool lastStateButton() const;
    };

}

#endif