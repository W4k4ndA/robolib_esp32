#ifndef H_BRIDGE_MOTOR_SERVICE_H
#define H_BRIDGE_MOTOR_SERVICE_H

/**
 * @file HBridgeMotorService.h
 * @brief Servicio para gestión centralizada de motores DC con puente H.
 *
 * Fachada que encapsula la interacción con drivers de motores puente H.
 * Proporciona API simplificada para control individual y diferencial.
 *
 * @ingroup services
 */

#include <stdint.h>
#include "../Interfaces/IDriverHBMotor.h"
#include "Helper.h"

namespace robolib
{

    /**
     * @class HBridgeMotorService
     * @brief Servicio de alto nivel para motores DC con puente H.
     *
     * Proporciona API simplificada para:
     * - Control individual de motores (velocidad, parada)
     * - Control diferencial para tracción diferencial (2 ruedas)
     * - Parada de emergencia (todos los motores)
     * - Conteo y verificación de existencia
     *
     * Convención de velocidad: -255 a 255 (8 bits PWM)
     * - Positivo: adelante
     * - Negativo: reversa
     * - 0: detenido
     *
     * Para control diferencial (tracción 2 ruedas):
     * - motor 0 = rueda izquierda
     * - motor 1 = rueda derecha
     *
     * @code
     * auto* motor = robot->motorDC;
     * if (motor->exists()) {
     *     motor->moveDifferential(200, 200);   // Adelante
     *     motor->moveDifferential(-100, 100);  // Giro izquierda
     *     motor->moveMotor(0, 150);            // Solo motor 0
     *     motor->stopAll(true);                // Freno activo
     * }
     * @endcode
     *
     * @ingroup services
     */
    class HBridgeMotorService : public ModuleHelper
    {
    public:
        using ModuleHelper::ModuleHelper; // Hereda constructor

        /**
         * @brief Mueve un motor específico a la velocidad indicada.
         *
         * @param index Índice del motor (0 = primero, 1 = segundo, etc.).
         * @param speed Velocidad objetivo (-255 a 255).
         *              Negativo = reversa, Positivo = adelante.
         */
        void moveMotor(size_t index, int16_t speed)
        {
            auto* motor = getModule<IDriverHBMotor>(index);
            if (motor) motor->move(speed);
        }

        /**
         * @brief Detiene un motor específico.
         *
         * @param index Índice del motor.
         * @param breaked Modo de frenado:
         *                - false (default): freno libre (coasting)
         *                - true: freno activo (brake)
         * @note Freno activo consume más corriente pero detiene más rápido.
         */
        void stopMotor(size_t index, bool breaked = false)
        {
            auto* motor = getModule<IDriverHBMotor>(index);
            if (motor) motor->stop(breaked);
        }

        /**
         * @brief Detiene todos los motores registrados.
         *
         * @param breaked Modo de frenado aplicado a todos.
         */
        void stopAll(bool breaked = false)
        {
            for (size_t i = 0; i < count(); i++)
            {
                stopMotor(i, breaked);
            }
        }

        /**
         * @brief Control diferencial para robot de tracción diferencial (2 ruedas).
         *
         * Asume:
         * - motor 0 = rueda izquierda
         * - motor 1 = rueda derecha
         *
         * @param leftSpeed  Velocidad rueda izquierda [-255, 255]
         * @param rightSpeed Velocidad rueda derecha [-255, 255]
         *
         * @note Convención: positivo = adelante para ambas ruedas.
         *       Giro: moveDifferential(200, -200) -> giro sobre eje.
         */
        void moveDifferential(int16_t leftSpeed, int16_t rightSpeed)
        {
            moveMotor(0, leftSpeed);
            moveMotor(1, rightSpeed);
        }

        /**
         * @brief Cuenta cuántos motores puente H están registrados.
         * @return Número de motores.
         */
        size_t count()
        {
            return countModules<IDriverHBMotor>();
        }

        /**
         * @brief Verifica si hay al menos un motor registrado.
         * @return true si hay motores, false en caso contrario.
         */
        bool exists()
        {
            return count() > 0;
        }
    };

} // namespace robolib

#endif /// H_BRIDGE_MOTOR_SERVICE_H