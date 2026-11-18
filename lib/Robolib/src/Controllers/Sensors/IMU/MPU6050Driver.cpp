#include "MPU6050Driver.h"

namespace robolib
{

    /**
     * @file MPU6050Driver.cpp
     * @brief Implementación del driver para sensor IMU MPU6050 sobre I2C.
     *
     * @ingroup drivers
     */

    MPU6050Driver::MPU6050Driver(uint8_t address, int sdaPin, int sclPin)
        : address(address), sdaPin(sdaPin), sclPin(sclPin), initialized(false),
          accelRange(0), gyroRange(0), roll(0.0f), pitch(0.0f), yaw(0.0f), lastTime(0)
    {
    }

    void MPU6050Driver::init()
    {
        // Iniciar bus I2C con pines configurados
        Wire.begin(sdaPin, sclPin);
        Wire.setClock(400000); // 400 kHz Fast Mode

        // Verificar que el sensor responde en la dirección I2C
        uint8_t whoAmI = 0;
        if (!readReg(REG_WHO_AM_I, whoAmI))
        {
            initialized = false;
            return;
        }

        // El WHO_AM_I debe retornar 0x68 o 0x69 (a veces 0x70/0x71 en clones)
        if ((whoAmI & 0x7E) != 0x68)
        {
            initialized = false;
            return;
        }

        // 1. Wake up del sensor (PWR_MGMT_1 = 0x00 deshabilita sleep)
        if (!writeReg(REG_PWR_MGMT_1, 0x00))
        {
            initialized = false;
            return;
        }
        delay(100); // Espera a que el sensor se estabilice

        // 2. Configurar rango del acelerómetro (±2g por defecto)
        setAccelRange(accelRange);

        // 3. Configurar rango del giroscopio (±250 °/s por defecto)
        setGyroRange(gyroRange);

        // 4. Configurar divisor de frecuencia de muestreo (100 Hz)
        //    sampleRate = 1000 / (1 + SMPLRT_DIV) => SMPLRT_DIV = 9 para 100 Hz
        writeReg(REG_SMPLRT_DIV, 0x09);

        // 5. Configurar filtro digital (DLPF) — 0x00 = 260 Hz BW
        writeReg(REG_CONFIG, 0x00);

        // 6. PWR_MGMT_2 = 0x00 (todos los ejes activos)
        writeReg(REG_PWR_MGMT_2, 0x00);

        initialized = true;
    }

    bool MPU6050Driver::writeReg(uint8_t reg, uint8_t data)
    {
        Wire.beginTransmission(address);
        Wire.write(reg);
        Wire.write(data);
        return Wire.endTransmission() == 0;
    }

    bool MPU6050Driver::readReg(uint8_t reg, uint8_t &data)
    {
        Wire.beginTransmission(address);
        Wire.write(reg);
        if (Wire.endTransmission(false) != 0)
            return false;

        Wire.requestFrom(address, (uint8_t)1);
        if (Wire.available() < 1)
            return false;

        data = Wire.read();
        return true;
    }

    bool MPU6050Driver::readRegs(uint8_t reg, uint8_t *buffer, uint8_t length)
    {
        Wire.beginTransmission(address);
        Wire.write(reg);
        if (Wire.endTransmission(false) != 0)
            return false;

        Wire.requestFrom(address, length);
        if ((uint8_t)Wire.available() < length)
            return false;

        for (uint8_t i = 0; i < length; i++)
        {
            buffer[i] = Wire.read();
        }
        return true;
    }

    bool MPU6050Driver::readAcceleration(float &ax, float &ay, float &az)
    {
        if (!initialized)
            return false;

        // Leer 6 bytes: ACCEL_XOUT_H, ACCEL_XOUT_L, ACCEL_YOUT_H, ...
        uint8_t buffer[6];
        if (!readRegs(REG_ACCEL_XOUT_H, buffer, 6))
            return false;

        // Reconstruir enteros de 16 bits (big-endian: alto primero)
        int16_t rawX = (int16_t)(buffer[0] << 8 | buffer[1]);
        int16_t rawY = (int16_t)(buffer[2] << 8 | buffer[3]);
        int16_t rawZ = (int16_t)(buffer[4] << 8 | buffer[5]);

        // Convertir a g usando el factor de escala según el rango
        float scale = ACCEL_SCALE[accelRange];
        ax = rawX / scale;
        ay = rawY / scale;
        az = rawZ / scale;

        return true;
    }

    bool MPU6050Driver::readGyroscope(float &gx, float &gy, float &gz)
    {
        if (!initialized)
            return false;

        // Leer 6 bytes: GYRO_XOUT_H, GYRO_XOUT_L, GYRO_YOUT_H, ...
        uint8_t buffer[6];
        if (!readRegs(REG_GYRO_XOUT_H, buffer, 6))
            return false;

        // Reconstruir enteros de 16 bits
        int16_t rawX = (int16_t)(buffer[0] << 8 | buffer[1]);
        int16_t rawY = (int16_t)(buffer[2] << 8 | buffer[3]);
        int16_t rawZ = (int16_t)(buffer[4] << 8 | buffer[5]);

        // Convertir a °/s usando el factor de escala según el rango
        float scale = GYRO_SCALE[gyroRange];
        gx = rawX / scale;
        gy = rawY / scale;
        gz = rawZ / scale;

        return true;
    }

    float MPU6050Driver::readTemperature()
    {
        if (!initialized)
            return NAN;

        uint8_t buffer[2];
        if (!readRegs(REG_TEMP_OUT_H, buffer, 2))
            return NAN;

        int16_t rawTemp = (int16_t)(buffer[0] << 8 | buffer[1]);

        // Fórmula oficial: temp = (raw / 340.0) + 36.53
        float temp = rawTemp / TEMP_SCALE + TEMP_OFFSET;
        return temp;
    }

    bool MPU6050Driver::readAll(float &ax, float &ay, float &az,
                                float &gx, float &gy, float &gz,
                                float &temp)
    {
        if (!initialized)
            return false;

        // Leer 14 bytes consecutivos desde ACCEL_XOUT_H (0x3B):
        // [0..5] = Accel X/Y/Z
        // [6..7] = Temp
        // [8..13] = Gyro X/Y/Z
        uint8_t buffer[14];
        if (!readRegs(REG_ACCEL_XOUT_H, buffer, 14))
            return false;

        // Aceleración
        int16_t rawAX = (int16_t)(buffer[0] << 8 | buffer[1]);
        int16_t rawAY = (int16_t)(buffer[2] << 8 | buffer[3]);
        int16_t rawAZ = (int16_t)(buffer[4] << 8 | buffer[5]);
        float accelScale = ACCEL_SCALE[accelRange];
        ax = rawAX / accelScale;
        ay = rawAY / accelScale;
        az = rawAZ / accelScale;

        // Temperatura
        int16_t rawTemp = (int16_t)(buffer[6] << 8 | buffer[7]);
        temp = rawTemp / TEMP_SCALE + TEMP_OFFSET;

        // Giroscopio
        int16_t rawGX = (int16_t)(buffer[8] << 8 | buffer[9]);
        int16_t rawGY = (int16_t)(buffer[10] << 8 | buffer[11]);
        int16_t rawGZ = (int16_t)(buffer[12] << 8 | buffer[13]);
        float gyroScale = GYRO_SCALE[gyroRange];
        gx = rawGX / gyroScale;
        gy = rawGY / gyroScale;
        gz = rawGZ / gyroScale;

        return true;
    }

    float MPU6050Driver::getRoll()
    {
        if (!initialized)
            return NAN;

        // Leer aceleración para calcular roll (atan2 del eje Y y Z)
        float ax, ay, az;
        if (!readAcceleration(ax, ay, az))
            return NAN;

        // roll = atan2(ay, az) * 180 / PI
        roll = atan2(ay, az) * 180.0f / PI;
        return roll;
    }

    float MPU6050Driver::getPitch()
    {
        if (!initialized)
            return NAN;

        // Leer aceleración para calcular pitch
        float ax, ay, az;
        if (!readAcceleration(ax, ay, az))
            return NAN;

        // pitch = atan2(-ax, sqrt(ay^2 + az^2)) * 180 / PI
        float denom = sqrt(ay * ay + az * az);
        pitch = atan2(-ax, denom) * 180.0f / PI;
        return pitch;
    }

    float MPU6050Driver::getYaw()
    {
        if (!initialized)
            return NAN;

        // Leer giroscopio para integrar yaw
        float gx, gy, gz;
        if (!readGyroscope(gx, gy, gz))
            return NAN;

        // Calcular delta de tiempo
        unsigned long now = millis();
        float dt = (now - lastTime) / 1000.0f; // dt en segundos
        lastTime = now;

        // Integrar yaw: yaw += gz * dt
        yaw += gz * dt;

        // Normalizar a [-180, 180]
        while (yaw > 180.0f) yaw -= 360.0f;
        while (yaw < -180.0f) yaw += 360.0f;

        return yaw;
    }

    void MPU6050Driver::setAccelRange(uint8_t range)
    {
        if (range > 3)
            range = 0; // Default ±2g
        accelRange = range;
        // ASEL[1:0] = bits 3:2 de ACCEL_CONFIG
        writeReg(REG_ACCEL_CONFIG, range << 3);
    }

    void MPU6050Driver::setGyroRange(uint8_t range)
    {
        if (range > 3)
            range = 0; // Default ±250 °/s
        gyroRange = range;
        // FS_SEL[1:0] = bits 3:2 de GYRO_CONFIG
        writeReg(REG_GYRO_CONFIG, range << 3);
    }

} // namespace robolib