#ifndef MPU6050_DRIVER_H
#define MPU6050_DRIVER_H

/**
 * @file MPU6050Driver.h
 * @brief Driver para IMU MPU6050 (acelerómetro + giroscopio 6-DOF).
 *
 * Implementa IDriverIMU usando comunicación I2C.
 * Soporta rangos configurables, calibración de offsets, auto-test y modo sueño.
 *
 * Especificaciones MPU6050:
 * - Acelerómetro: 3 ejes, ±2g/±4g/±8g/±16g, 16-bit ADC
 * - Giroscopio: 3 ejes, ±250/±500/±1000/±2000 °/s, 16-bit ADC
 * - Temperatura: integrado, -40 a +85 °C
 * - I2C: hasta 400 kHz (Fast Mode), dirección 0x68 (AD0=GND) o 0x69 (AD0=VCC)
 * - DMP: Digital Motion Processor (no implementado en este driver)
 *
 * Conexión típica ESP32:
 * - VCC -> 3.3V
 * - GND -> GND
 * - SDA -> GPIO 21 (default I2C0)
 * - SCL -> GPIO 22 (default I2C0)
 * - AD0 -> GND (addr 0x68) o 3.3V (addr 0x69)
 * - INT -> GPIO opcional (interrupción data ready)
 *
 * @warning MPU6050 trabaja a 3.3V. No conectar directamente a 5V.
 * @note Para DMP (fusión de sensores), usar librería externa (ej. I2Cdevlib).
 *
 * Ejemplo de uso:
 * @code
 * MPU6050Driver imu(21, 22);  // SDA, SCL
 * imu.init();
 * imu.setAccelRange(AccelRange::RANGE_4G);
 * imu.setGyroRange(GyroRange::RANGE_500DPS);
 * imu.calibrateGyro(500);  // Robot QUIETO
 *
 * IMUData data = imu.read();
 * float roll  = atan2(data.accelY, data.accelZ) * 180 / PI;
 * float pitch = atan2(-data.accelX, sqrt(data.accelY*data.accelY + data.accelZ*data.accelZ)) * 180 / PI;
 * @endcode
 *
 * @see IDriverIMU
 * @ingroup drivers
 */

#include "../../../Interfaces/IDriverIMU.h"
#include <Wire.h>

namespace robolib
{

    /**
     * @class MPU6050Driver
     * @brief Driver para sensor MPU6050 via I2C.
     */
    class MPU6050Driver : public IDriverIMU
    {
    private:
        // Configuración I2C
        TwoWire &wire;           ///< Bus I2C (Wire o Wire1)
        uint8_t i2cAddr;         ///< Dirección I2C (0x68 o 0x69)
        uint8_t sdaPin;          ///< Pin SDA (para begin personalizado)
        uint8_t sclPin;          ///< Pin SCL (para begin personalizado)
        uint32_t i2cFreq;        ///< Frecuencia I2C (default 400 kHz)

        // Rangos configurados
        AccelRange accelRange = AccelRange::RANGE_4G;
        GyroRange gyroRange = GyroRange::RANGE_500DPS;

        // Factores de escala (se actualizan con setAccelRange/setGyroRange)
        float accelScale = 8192.0f;   ///< LSB/g para ±4g
        float gyroScale = 65.5f;      ///< LSB/(°/s) para ±500°/s

        // Offsets de calibración
        int16_t accelOffsetX = 0;
        int16_t accelOffsetY = 0;
        int16_t accelOffsetZ = 0;
        int16_t gyroOffsetX = 0;
        int16_t gyroOffsetY = 0;
        int16_t gyroOffsetZ = 0;

        bool calibrated = false;     ///< Flag de calibración completada
        bool initialized = false;    ///< Flag de init() llamado

        // Registros MPU6050
        static constexpr uint8_t REG_WHO_AM_I     = 0x75;
        static constexpr uint8_t REG_PWR_MGMT_1   = 0x6B;
        static constexpr uint8_t REG_PWR_MGMT_2   = 0x6C;
        static constexpr uint8_t REG_CONFIG       = 0x1A;
        static constexpr uint8_t REG_GYRO_CONFIG  = 0x1B;
        static constexpr uint8_t REG_ACCEL_CONFIG = 0x1C;
        static constexpr uint8_t REG_ACCEL_XOUT_H = 0x3B;
        static constexpr uint8_t REG_TEMP_OUT_H   = 0x41;
        static constexpr uint8_t REG_GYRO_XOUT_H  = 0x43;
        static constexpr uint8_t REG_SIGNAL_PATH  = 0x68;
        static constexpr uint8_t REG_USER_CTRL    = 0x6A;
        static constexpr uint8_t REG_FIFO_EN      = 0x23;
        static constexpr uint8_t REG_I2C_MST_CTRL = 0x24;

        // Valores WHO_AM_I esperados
        static constexpr uint8_t WHO_AM_I_MPU6050 = 0x68;

        // Helpers I2C
        bool writeRegister(uint8_t reg, uint8_t value);
        bool readRegister(uint8_t reg, uint8_t &value);
        bool readRegisters(uint8_t reg, uint8_t *buffer, size_t len);
        int16_t readInt16(uint8_t regHigh);

        // Conversión raw -> unidades físicas
        float convertAccel(int16_t raw) const;
        float convertGyro(int16_t raw) const;
        float convertTemp(int16_t raw) const;

        // Actualizar factores de escala según rango
        void updateScales();

    public:
        /**
         * @brief Constructor con bus I2C por defecto (Wire).
         *
         * @param sda Pin SDA (default: 21 - I2C0 default ESP32).
         * @param scl Pin SCL (default: 22 - I2C0 default ESP32).
         * @param addr Dirección I2C (default: 0x68 con AD0=GND).
         * @param freq Frecuencia I2C en Hz (default: 400000 = 400 kHz).
         *
         * @note No inicializa hardware; llamar a init() después.
         */
        MPU6050Driver(uint8_t sda = 21, uint8_t scl = 22, uint8_t addr = 0x68, uint32_t freq = 400000);

        /**
         * @brief Constructor con bus I2C personalizado (ej. Wire1 en ESP32).
         *
         * @param wireRef Referencia al bus TwoWire (Wire, Wire1, etc.).
         * @param sda Pin SDA.
         * @param scl Pin SCL.
         * @param addr Dirección I2C (default: 0x68).
         * @param freq Frecuencia I2C en Hz (default: 400000).
         */
        MPU6050Driver(TwoWire &wireRef, uint8_t sda, uint8_t scl, uint8_t addr = 0x68, uint32_t freq = 400000);

        /**
         * @brief Destructor.
         */
        ~MPU6050Driver() override = default;

        // --- IRobotModule ---
        void init() override;
        void update() override;

        // --- IDriverIMU ---
        void setAccelRange(AccelRange range) override;
        void setGyroRange(GyroRange range) override;
        AccelRange getAccelRange() const override { return accelRange; }
        GyroRange getGyroRange() const override { return gyroRange; }

        IMUData read() override;
        IMURawData readRaw() override;

        bool selfTest() override;
        bool calibrateGyro(uint16_t samples = 200) override;
        bool calibrateAccel(uint16_t samples = 100) override;

        bool isConnected() const override;
        uint8_t getDeviceID() const override;

        void sleep() override;
        void wakeUp() override;

        /**
         * @brief Verifica si el driver está inicializado.
         * @return true si init() se completó exitosamente.
         */
        bool isInitialized() const { return initialized; }

        /**
         * @brief Verifica si hay calibración válida aplicada.
         * @return true si calibrateGyro() o calibrateAccel() tuvo éxito.
         */
        bool isCalibrated() const { return calibrated; }

        /**
         * @brief Obtiene offsets de giroscopio actuales.
         * @param x Offset eje X (LSB raw).
         * @param y Offset eje Y (LSB raw).
         * @param z Offset eje Z (LSB raw).
         */
        void getGyroOffsets(int16_t &x, int16_t &y, int16_t &z) const
        {
            x = gyroOffsetX; y = gyroOffsetY; z = gyroOffsetZ;
        }

        /**
         * @brief Obtiene offsets de acelerómetro actuales.
         * @param x Offset eje X (LSB raw).
         * @param y Offset eje Y (LSB raw).
         * @param z Offset eje Z (LSB raw).
         */
        void getAccelOffsets(int16_t &x, int16_t &y, int16_t &z) const
        {
            x = accelOffsetX; y = accelOffsetY; z = accelOffsetZ;
        }

        /**
         * @brief Establece offsets manualmente (ej. cargados de NVS/EEPROM).
         * @param gx, gy, gz Offsets giroscopio (LSB raw).
         * @param ax, ay, az Offsets acelerómetro (LSB raw).
         */
        void setOffsets(int16_t gx, int16_t gy, int16_t gz,
                        int16_t ax, int16_t ay, int16_t az);
    };

} // namespace robolib

#endif // MPU6050_DRIVER_H