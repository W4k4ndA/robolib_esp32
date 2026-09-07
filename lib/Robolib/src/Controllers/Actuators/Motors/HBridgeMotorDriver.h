#ifndef HBRIDGE_DRIVER_H
#define HBRIDGE_DRIVER_H

#include "../../../Interfaces/IDriverHBMotor.h"

namespace robolib
{

    /**
     * @file HBridgeMotorDriver.h
     * @brief Driver para motores DC con puente H (L298N, TB6612, L9110, etc.).
     *
     * Implementa IDriverMotor usando dos canales PWM del periférico LEDC del ESP32
     * para control de velocidad y dos pines GPIO para dirección.
     *
     * @ingroup drivers
     */

    /**
     * @brief Driver genérico para puente H de 2 canales (un motor DC).
     *
     * Esquema de control típico:
     * - Canal PWM 1 (CH1) -> IN1 del puente H
     * - Canal PWM 2 (CH2) -> IN2 del puente H
     *
     * Tabla de estados:
     * | CH1 | CH2 | Estado        |
     * |-----|-----|---------------|
     * | PWM | 0   | Adelante      |
     * | 0   | PWM | Reversa       |
     * | 0   | 0   | Libre (coast) |
     * | 255 | 255 | Freno (brake) |
     *
     * @note Requiere ESP32 (usa LEDC). No compatible con AVR/Arduino clásico.
     * @note Pines PWM deben ser capaces de salida (no GPIO 34-39 solo entrada).
     *
     * Ejemplo de uso:
     * @code
     * // Motor en pines IN1=5, IN2=18, canales LEDC 0 y 1
     * HBridgeMotorDriver motor(5, 18, 0, 1);
     * motor.begin();
     * motor.move(200);   // Adelante ~78%
     * motor.move(-150);  // Reversa ~59%
     * motor.stop();      // Libre
     * motor.stop(true);  // Freno activo
     * @endcode
     *
     * @see IDriverMotor
     * @ingroup drivers
     */
    class HBridgeMotorDriver : public IDriverHBMotor
    {
    private:
        uint8_t IN1; ///< Pin GPIO dirección 1 (conectado a IN1 del puente H)
        uint8_t IN2; ///< Pin GPIO dirección 2 (conectado a IN2 del puente H)
        uint8_t CH1; ///< Canal LEDC para IN1 (0-15)
        uint8_t CH2; ///< Canal LEDC para IN2 (0-15)

        static constexpr uint32_t PWM_FREQ = 25000; ///< Frecuencia PWM: 25 kHz (inaudible, buena para motores)
        static constexpr uint8_t PWM_RES = 8;       ///< Resolución PWM: 8 bits (0-255)

    public:
        /**
         * @brief Constructor.
         * Driver para motores DC con puente H (L298N, TB6612, L9110, etc.).
         *
         * @param pinIN1 Pin GPIO conectado a IN1 del puente H 
         * @param pinIN2 Pin GPIO conectado a IN2 del puente H 
         * @param ch1    Canal LEDC (0-15) para pinIN1
         * @param ch2    Canal LEDC (0-15) para pinIN2
         *
         * @note Los canales deben ser distintos y no usados por otros drivers.
         *       El ESP32-C3 tiene 6 canales (0-5), ESP32 clásico tiene 16 (0-15).
         * @note No inicializa hardware; llamar a begin() después.
         */
        HBridgeMotorDriver(uint8_t pinIN1, uint8_t pinIN2, uint8_t ch1, uint8_t ch2);

        /**
         * @brief Destructor.
         *
         * No libera canales LEDC ni pines (gestionado por begin()/stop()).
         * Si se requiere limpieza explícita, llamar stop() antes de destruir.
         */
        ~HBridgeMotorDriver() override = default;

        /**
         * @brief Inicializa pines y configuración PWM.
         *
         * Configura:
         * - IN1, IN2 como OUTPUT
         * - Canales LEDC CH1, CH2 con PWM_FREQ y PWM_RES
         * - Adjunta pines a canales LEDC
         * - Deja motor en estado detenido (freno libre)
         *
         * @post Motor listo para recibir comandos move()/stop()
         */
        void init() override;

        /**
         * @brief Establece velocidad del motor.
         *
         * @param speed Velocidad objetivo [-255, 255].
         *              Saturado internamente al rango válido.
         *
         * Implementación:
         * - speed > 0: CH1 = speed, CH2 = 0 (adelante)
         * - speed < 0: CH1 = 0, CH2 = |speed| (reversa)
         * - speed = 0: stop() -> freno libre
         */
        void move(int16_t speed) override;

        /**
         * @brief Detiene el motor.
         *
         * @param stacked Modo de frenado:
         *                - false: freno libre (coasting), ambos canales a 0
         *                - true:  freno activo (brake), ambos canales a 255
         *
         * @note Freno activo (stacked=true) cortocircuita el motor en el puente H,
         *       deteniéndolo más rápido pero consumiendo corriente de mantenimiento.
         */
        void stop(bool stacked = false) override;
    };

} // namespace robolib

#endif