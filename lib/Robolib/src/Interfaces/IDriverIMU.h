#ifndef I_DRIVER_IMU_H
#define I_DRIVER_IMU_H

/**
 * @file IDriverIMU.h
 * @brief Interfaz abstracta para Unidades de Medición Inercial (IMU).
 *
 * Define el contrato para sensores IMU como MPU6050, MPU9250, ICM20948, etc.
 * Proporciona acceso unificado a acelerómetro, giroscopio y temperatura.
 *
 * @ingroup interfaces
 */

#include "IRobotModule.h"
#include <cstdint>

namespace robolib
{

    /**
     * @struct IMUData
     * @brief Estructura que contiene todos los datos del IMU.
     *
     * Agrupa aceleración, velocidad angular y temperatura en una sola
     * estructura para lecturas atómicas y coherentes.
     */
    struct IMUData
    {
        // Acelerómetro (m/s^2)
        float accelX = 0.0f;
        float accelY = 0.0f;
        float accelZ = 0.0f;

        // Giroscopio (grados/segundo)
        float gyroX = 0.0f;
        float gyroY = 0.0f;
        float gyroZ = 0.0f;

        // Temperatura (°C)
        float temperature = 0.0f;

        // Timestamp de la lectura (ms desde boot)
        uint32_t timestamp = 0;
    };

    /**
     * @struct IMURawData
     * @brief Datos crudos del IMU (valores sin escalar del ADC).
     *
     * Útil para calibración avanzada o cuando se requiere acceso directo
     * a los registros del sensor.
     */
    struct IMURawData
    {
        int16_t accelX = 0;
        int16_t accelY = 0;
        int16_t accelZ = 0;
        int16_t gyroX = 0;
        int16_t gyroY = 0;
        int16_t gyroZ = 0;
        int16_t temperature = 0;
    };

    /**
     * @enum AccelRange
     * @brief Rangos de medición del acelerómetro.
     */
    enum class AccelRange
    {
        RANGE_2G  = 0,  ///< ±2g  (mayor precisión)
        RANGE_4G  = 1,  ///< ±4g
        RANGE_8G  = 2,  ///< ±8g
        RANGE_16G = 3   ///< ±16g (mayor rango)
    };

    /**
     * @enum GyroRange
     * @brief Rangos de medición del giroscopio.
     */
    enum class GyroRange
    {
        RANGE_250DPS  = 0,  ///< ±250 °/s  (mayor precisión)
        RANGE_500DPS  = 1,  ///< ±500 °/s
        RANGE_1000DPS = 2,  ///< ±1000 °/s
        RANGE_2000DPS = 3   ///< ±2000 °/s (mayor rango)
    };

    /**
     * @class IDriverIMU
     * @brief Interfaz base para drivers de IMU.
     *
     * Abstrae la comunicación I2C/SPI y la configuración de registros
     * para permitir intercambiar sensores (MPU6050, MPU9250, ICM20948, etc.)
     * sin afectar la lógica de navegación o estabilización.
     *
     * Unidades estándar:
     * - Aceleración: m/s²
     * - Velocidad angular: °/s (grados por segundo)
     * - Temperatura: °C
     *
     * @code
     * MPU6050Driver imu(21, 22);  // SDA=21, SCL=22
     * imu.init();
     * imu.setAccelRange(AccelRange::RANGE_4G);
     * imu.setGyroRange(GyroRange::RANGE_500DPS);
     *
     * IMUData data = imu.read();
     * float pitch = atan2(data.accelY, data.accelZ) * 180 / PI;
     * @endcode
     *
     * @see MPU6050Driver
     * @ingroup interfaces
     */
    class IDriverIMU : public IRobotModule
    {
    public:
        /**
         * @brief Destructor virtual por defecto.
         */
        virtual ~IDriverIMU() = default;

        /**
         * @brief Obtiene el tipo de módulo.
         * @return ModuleType::IMU
         */
        static constexpr ModuleType staticType() { return ModuleType::IMU; }
        ModuleType getType() const override { return staticType(); }

        /**
         * @brief Configura el rango del acelerómetro.
         *
         * @param range Rango deseado (RANGE_2G, RANGE_4G, RANGE_8G, RANGE_16G).
         *              Ranges mayores = menos resolución, más rango dinámico.
         * @note Debe llamarse después de init() para tener efecto.
         */
        virtual void setAccelRange(AccelRange range) = 0;

        /**
         * @brief Configura el rango del giroscopio.
         *
         * @param range Rango deseado (RANGE_250DPS, RANGE_500DPS, RANGE_1000DPS, RANGE_2000DPS).
         * @note Debe llamarse después de init() para tener efecto.
         */
        virtual void setGyroRange(GyroRange range) = 0;

        /**
         * @brief Obtiene el rango actual del acelerómetro.
         * @return Rango configurado.
         */
        virtual AccelRange getAccelRange() const = 0;

        /**
         * @brief Obtiene el rango actual del giroscopio.
         * @return Rango configurado.
         */
        virtual GyroRange getGyroRange() const = 0;

        /**
         * @brief Realiza una lectura completa del IMU (bloqueante).
         *
         * Lee acelerómetro, giroscopio y temperatura en una sola transacción I2C
         * para garantizar coherencia temporal entre ejes.
         *
         * @return IMUData con valores escalados en unidades físicas (m/s², °/s, °C).
         *         Retorna estructura con ceros si hay error de comunicación.
         *
         * @note Función bloqueante: ~1-3 ms típico por transacción I2C.
         * @note Para alta frecuencia, usar readRaw() + conversión manual.
         */
        virtual IMUData read() = 0;

        /**
         * @brief Lee datos crudos del IMU (sin escalar).
         *
         * @return IMURawData con valores ADC de 16 bits sin procesar.
         *         Útil para calibración o procesamiento personalizado.
         */
        virtual IMURawData readRaw() = 0;

        /**
         * @brief Realiza auto-test del sensor.
         *
         * Ejecuta la rutina de auto-test interno del MPU6050 para verificar
         * integridad del acelerómetro y giroscopio.
         *
         * @return true si el auto-test pasa, false si falla.
         */
        virtual bool selfTest() = 0;

        /**
         * @brief Calibra offsets del giroscopio (bias).
         *
         * Realiza N lecturas en reposo y calcula el offset promedio para
         * compensar deriva del giroscopio. El sensor DEBE estar completamente
         * quieto durante la calibración.
         *
         * @param samples Número de muestras para promediar (default: 200).
         * @return true si calibración exitosa.
         *
         * @warning El robot debe estar INMÓVIL durante la calibración.
         * @note Los offsets se aplican automáticamente en read().
         */
        virtual bool calibrateGyro(uint16_t samples = 200) = 0;

        /**
         * @brief Calibra offsets del acelerómetro.
         *
         * Realiza calibración en 6 posiciones (opcional) o simple offset
         * en posición horizontal. Compensa gravedad y offset de fábrica.
         *
         * @param samples Muestras por posición (default: 100).
         * @return true si calibración exitosa.
         *
         * @warning Para calibración completa (6 posiciones), rotar el robot
         *          siguiendo las instrucciones en consola/serial.
         */
        virtual bool calibrateAccel(uint16_t samples = 100) = 0;

        /**
         * @brief Verifica si el sensor está conectado y responde.
         *
         * Lee el registro WHO_AM_I para confirmar identidad del dispositivo.
         *
         * @return true si el sensor responde correctamente.
         */
        virtual bool isConnected() const = 0;

        /**
         * @brief Obtiene la ID del dispositivo (WHO_AM_I).
         * @return Valor del registro WHO_AM_I (0x68 para MPU6050).
         */
        virtual uint8_t getDeviceID() const = 0;

        /**
         * @brief Pone el sensor en modo sueño (bajo consumo).
         *
         * Reduce consumo drásticamente. Debe llamarse wakeUp() para reactivar.
         */
        virtual void sleep() = 0;

        /**
         * @brief Despierta el sensor del modo sueño.
         *
         * Reactiva el sensor tras sleep(). Requiere ~10 ms para estabilizarse.
         */
        virtual void wakeUp() = 0;
    };

} // namespace robolib

#endif // I_DRIVER_IMU_H