#include "HBridgeMotorDriver.h"
#include <Arduino.h>

namespace robolib
{

    /**
     * @file HBridgeMotorDriver.cpp
     * @brief Implementación de driver para puente H genérico (L298N, TB6612, etc.).
     *
     * Usa periférico LEDC del ESP32 para PWM de 2 canales.
     * Frecuencia: 25 kHz (fuera de rango audible, reduce vibración audible en motores).
     * Resolución: 8 bits (0-255).
     *
     * @ingroup drivers
     */

    HBridgeMotorDriver::HBridgeMotorDriver(uint8_t pinIN1, uint8_t pinIN2)
        : IN1(pinIN1), IN2(pinIN2)
    {
    }

    void HBridgeMotorDriver::init()
    {
        // Configurar pines de dirección como salidas
        pinMode(IN1, OUTPUT);
        pinMode(IN2, OUTPUT);

        CH1 = PWMChannelManager::getChannel();
        CH2 = PWMChannelManager::getChannel();

        availableChannels = (CH1 != -1 && CH2 != -1);

        if (availableChannels)
        {
            // Configurar canales LEDC: frecuencia 25 kHz, resolución 8 bits
            ledcSetup(CH1, PWM_FREQ, PWM_RES);
            ledcSetup(CH2, PWM_FREQ, PWM_RES);

            // Adjuntar pines GPIO a canales LEDC
            ledcAttachPin(IN1, CH1);
            ledcAttachPin(IN2, CH2);

            // Estado inicial: motor detenido (freno libre)
            HBridgeMotorDriver::stop(false);
        }
        else
        {
            // Manejo de error: Se agotaron los canales PWM del ESP32
            Serial.println("Error: No hay canales PWM (LEDC) disponibles para el motor.");
        }
    }

    void HBridgeMotorDriver::move(int16_t speed)
    {
        if (!availableChannels)
            return; // Salir si no hay canales disponibles

        // obtenemos el valor maximo dinamicamente segun PWM_RES
        int16_t maxDuty = (1 << PWM_RES) - 1;

        // Restringimos el valor de speed a los valores maximos y minimos de la resolucion de PWM
        speed = constrain(speed, -maxDuty, maxDuty);

        uint16_t absSpeed = abs(speed);

        if (speed > 0)
        {
            // Adelante: CH1 = PWM, CH2 = 0
            ledcWrite(CH1, absSpeed);
            ledcWrite(CH2, 0);
        }
        else if (speed < 0)
        {
            // Reversa: CH1 = 0, CH2 = |PWM|
            ledcWrite(CH1, 0);
            ledcWrite(CH2, absSpeed);
        }
        else
        {
            // Cero: detener con freno libre
            HBridgeMotorDriver::stop(false);
        }
    }

    void HBridgeMotorDriver::stop(bool breaked)
    {

        if (!availableChannels)
            return;
        // Freno activo (brake): ambos canales a MAX -> cortocircuito en puente H
        // Detiene el motor rápidamente pero consume corriente de mantenimiento
        
        if (breaked)
        {
           // Freno activo: Ambos canales al máximo ciclo de trabajo permitido por PWM_RES
            uint32_t maxDuty = (1 << PWM_RES) - 1;
            ledcWrite(CH1, maxDuty);
            ledcWrite(CH2, maxDuty);
        }
        else
        {
            // Freno libre (coasting): ambos canales a 0 -> motor gira libre
            ledcWrite(CH1, 0);
            ledcWrite(CH2, 0);
        }
    }

} // namespace robolib