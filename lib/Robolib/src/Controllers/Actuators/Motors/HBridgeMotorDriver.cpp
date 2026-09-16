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
            stop(false);
        }
        else
        {
            // Manejo de error: Se agotaron los canales PWM del ESP32
            Serial.println("Error: No hay canales PWM (LEDC) disponibles para el motor.");
        }
    }

    void HBridgeMotorDriver::move(uint32_t speed)
    {
        // Saturar a rango PWM válido [-255, 255]-[-1023, 1023]-[-4095, 4095]
        switch (PWM_RES)
        {
        case 8:
            speed = constrain(speed, -255, 255);
            break;

        case 10:
            speed = constrain(speed, -1023, 1023);
            break;

        case 12:
            speed = constrain(speed, -4095, 4095);
            break;

        default:
            break;
        }

        if (availableChannels)
        {
            if (speed > 0)
            {
                // Adelante: CH1 = PWM, CH2 = 0
                ledcWrite(CH1, speed);
                ledcWrite(CH2, 0);
            }
            else if (speed < 0)
            {
                // Reversa: CH1 = 0, CH2 = |PWM|
                ledcWrite(CH1, 0);
                ledcWrite(CH2, speed);
            }
            else
            {
                // Cero: detener con freno libre
                stop(false);
            }
        }
    }

    void HBridgeMotorDriver::stop(bool breaked)
    {
        // Freno activo (brake): ambos canales a 255 -> cortocircuito en puente H
        // Detiene el motor rápidamente pero consume corriente de mantenimiento
        if (availableChannels)
        {
            if (breaked)
            {
                switch (PWM_RES)
                {
                case 8:

                    ledcWrite(CH1, 255);
                    ledcWrite(CH2, 255);
                    break;

                case 10:

                    ledcWrite(CH1, 1023);
                    ledcWrite(CH2, 1023);
                    break;

                case 12:

                    ledcWrite(CH1, 4095);
                    ledcWrite(CH2, 4095);
                    break;

                default:
                    break;
                }

                ledcWrite(CH1, 255);
                ledcWrite(CH2, 255);
            }
            else
            {
                // Freno libre (coasting): ambos canales a 0 -> motor gira libre
                ledcWrite(CH1, 0);
                ledcWrite(CH2, 0);
            }
        }
    }

} // namespace robolib