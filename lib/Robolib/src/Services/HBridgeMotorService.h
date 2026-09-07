#ifndef H_BRIDGE_MOTOR_SERVICE_H
#define H_BRIDGE_MOTOR_SERVICE_H

#include <stdint.h>
#include "../Interfaces/IDriverHBMotor.h"
#include "Helper.h"

namespace robolib
{
    /**
     * @brief Servicio para gestión centralizada de motores DC controlados
     * por un puente H.
     *Este servicio encapsula toda la interacción con el driver puente h.
     *Aquí vive la lógica de fachada especializada.
     * Proporciona acceso simplificado a drivers puente h registrados en el robot.
     * @ingroup services
     */

    class HBridgeMotorService: public ModuleHelper
    {
    public:
    using ModuleHelper::ModuleHelper; // Hereda constructores

    /**
     * @brief Mueve el motor a la velocidad especificada.
     * @param speed Velocidad del motor (rango: -255 a 255). Depende del microncontrolador
     * @note si la velocidad es negativa, el motor gira en sentido contrario
     * a la velocidad positiva.
     */
    void moveMotor(size_t index, int16_t speed){
        auto* motor = getModule<IDriverHBMotor>(index);
        if(motor) motor->move(speed);
    }
    
    /** @brief Detiene el motor. 
     * @param breaked Si es true, el motor se detiene activando ambas salidas.
     * Esto puede ser útil para frenar el motor rápidamente pero consume más energía
     * Ademas, hay driver que no soportan este modo.
    */
    void stopMotor(size_t index, bool breaked = false){
        auto* motor = getModule<IDriverHBMotor>(index);
        if(motor) motor->stop(breaked);
    }

    /**
     * @brief Detiene todos los motores registrados en el robot.
     * @param breaked Si es true, los motores se detienen activando ambas salidas.
     */
    void stopAll(bool breaked = false){
        for(size_t i = 0; i < count(); i++){
            stopMotor(i, breaked);
        }
    }

    void moveDifferential(int16_t leftSpeed, int16_t rightSpeed){
        moveMotor(0, leftSpeed);
        moveMotor(1, rightSpeed);

    }

    /**
     * @brief Cuenta cuántos modulos de motor con puente h están registrados en el robot.
     */
    size_t count(){
        return countModules<IDriverHBMotor>();
    }

    /**
     * @brief Verifica si hay al menos modulo de tipo motor con puente h registrado.
     * @return true si hay al menos un motor, false en caso contrario.
     */
    bool exists(){
        return count() > 0;
    }


    };
}

#endif /// H_BRIDGE_MOTOR_SERVICE_H