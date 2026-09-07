#ifndef TCRT5000_SENSOR_DRIVER_H
#define TCRT5000_SENSOR_DRIVER_H

#include "../../../Interfaces/IDriverDigitalLineSensor.h"
#include <cstdint>

namespace robolib
{

    /**
     * @class TCRT5000SensorDriver
     * @brief Driver para sensor de línea TCRT5000 (reflectancia infrarroja).
     *
     * Implementa la interfaz IDriverLineSensor para el sensor TCRT5000,
     * un sensor de reflectancia IR compuesto por un emisor IR y un fototransistor.
     * Detecta contrastes de superficie (línea negra sobre fondo blanco, o viceversa).
     *
     * Características:
     * - Soporta modo analógico (lectura ADC) y digital (GPIO con comparador interno)
     * - Umbral configurable para detección de línea
     * - Compatible con ESP32/ESP32-C3 (ADC 12-bit, 0-4095)
     *
     * Conexión típica:
     * - VCC -> 3.3V/5V
     * - GND -> GND
     * - OUT (analógico) -> Pin ADC (ej: GPIO 4)
     * - DO (digital) -> Pin GPIO (ej: GPIO 5) + potenciómetro onboard para umbral
     *
     * @see IDriverLineSensor
     * @ingroup drivers
     */
    class TCRT5kDigitalDriver :  public IDriverDigitalLineSensor
    {
    private:
        uint8_t sensorPin;      ///< Pin GPIO/ADC conectado a la salida del sensor
        bool is1onLine = false; ///< Estado interno: true si el sensor retorna 1 al leer linea, false si no

    public:
        /**
         * @brief Constructor del driver TCRT5000.
         *
         * @param pin Número de pin GPIO/ADC donde está conectado el sensor.
         *            En ESP32-C3: pines ADC1 (GPIO 1-10) recomendados.
         * @param sensorType Tipo de conexión:
         *                   - false (default): modo analógico, lectura por ADC
         *                   - true: modo digital, lectura por GPIO (usa comparador del módulo)
         *
         * @note El pin debe ser válido para ADC si isDigital=false.
         *       No configura hardware; llamar a begin() después.
         */
        TCRT5kDigitalDriver(uint8_t pin);

        /**
         * @brief Destructor por defecto.
         *
         * No libera recursos de hardware (pines GPIO/ADC son gestionados por framework Arduino).
         */
        ~TCRT5kDigitalDriver() override = default;

        /**
         * @brief Inicializa el hardware del sensor.
         *
         * Configura el pin según el modo:
         * - Analógico: pinMode(pin, INPUT) / analogRead()
         * - Digital: pinMode(pin, INPUT) / digitalRead()
         *
         * Debe llamarse una vez en setup() antes de usar el sensor.
         *
         * @post Pin configurado y listo para lecturas.
         */
        void init() override;

        void update() override;

        /**
         * @brief Verifica si el sensor está actualmente sobre una línea.
         *
         * En modo analógico: compara getAnalogValue() contra threshold.
         * En modo digital: lee estado GPIO directo (el módulo TCRT5000 tiene comparador + potenciómetro).
         *
         * @return true si detecta línea (reflectancia baja / GPIO LOW según configuración),
         *         false si no detecta línea.
         *
         * @see setThreshold() para ajustar sensibilidad en modo analógico.
         */
        bool getLine() override;

        /**
         * @brief Actualiza el estado interno is1onLine.
         *
         * Define si 1 significa estar en la linea. Si se llama, cada vez que
         * se use getLine() este retornara true si el sensor manda 1.
         * Esta activado por defecto.
         *
         * @post is1onLine refleja el estado actual de detección.
         */
        void set1IsOnLine(bool value = true) override;


    };

}
#endif // TCRT5000_SENSOR_DRIVER_H
