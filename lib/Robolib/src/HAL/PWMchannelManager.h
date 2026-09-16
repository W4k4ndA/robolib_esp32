#ifndef PWM_CHANNEL_MANAGER_H
#define PWM_CHANNEL_MANAGER_H

#include <Arduino.h>

// 1. Detección de la cantidad de canales según el SoC
#if defined(SOC_LEDC_CHANNEL_NUM)
#define MAX_PWM_CHANNELS SOC_LEDC_CHANNEL_NUM

#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C2)
// ESP32-C3 y C2 tienen 6 canales LEDC (0 a 5)
#define MAX_PWM_CHANNELS 6

#elif defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
// ESP32-S2 y S3 tienen 8 canales LEDC (0 a 7)
#define MAX_LEDC_CHANNELS 8

#else
// ESP32 Clásico (Dual Core) posee 16 canales LEDC (0 a 15)
#define MAX_LEDC_CHANNELS 16
#endif

namespace robolib
{

    class PWMChannelManager
    {
    private:
        // Usamos uint16_t como bitmask para soportar hasta 16 canales (ESP32 clásico)
        static uint16_t channelMap;

    public:
        /**
         * Obtiene la cantidad máxima de canales PWM disponibles en el SoC.
         */
        static constexpr uint8_t getMaxChannels()
        {
            return MAX_PWM_CHANNELS;
        }

        /**
         * @brief Reserva automáticamente el primer canal PWM disponible
         * @return int8_t ID del canal, o -1 si no hay canales libres
         */
        static int8_t getChannel()
        {
            for (uint8_t ch; ch < MAX_PWM_CHANNELS; ch++)
            {
                // Se verifica si es cierto (true) que el bit correspondiente al canal está libre (0)
                if (!(channelMap & (1 << ch)))
                {
                    // si esta libre se marca ahora como ocupado y se retorna el canal (0, 1, 5, etc.)
                    channelMap |= (1 << ch);
                    return ch;
                }
            }
            return -1; // No hay canales libres en el chip
        }

        /**
         * @brief Libera un canal para que pueda ser utilizado por otro driver
         */
        static void freeChannel(uint8_t channel)
        {
            if (channel < MAX_PWM_CHANNELS)
            {
                channelMap &= ~(1 << channel); // Marcar canal como libre
            }
        }
    };

    // Inicialización del mapa de bits
    inline uint16_t PWMChannelManager::channelMap = 0;

}

#endif