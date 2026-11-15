#include "MPU6050Driver.h"
#include <Arduino.h>
#include <math.h>

namespace robolib
{

/**
 * @file MPU6050Driver.cpp
 * @brief Implementación del driver MPU6050 para ESP32 via I2C.
 *
 * Basado en hoja de datos MPU-6000/MPU-6050 Register Map v4.2.
 * Usa comunicación I2C estándar (Wire.h) a 400 kHz.
 *
 * @ingroup drivers
 */

// Constructor con bus Wire por defecto
MPU6050Driver::MPU6050Driver(uint8_t sda, uint8_t scl, uint8_t addr, uint32_t freq)
    : wire(Wire), i2cAddr(addr), sdaPin(sda), sclPin(scl), i2cFreq(freq) {}

// Constructor con bus I2C personalizado
MPU6050Driver::MPU6050Driver(TwoWire &wireRef, uint8_t sda, uint8_t scl, uint8_t addr, uint32_t freq)
    : wire(wireRef), i2cAddr(addr), sdaPin(sda), sclPin(scl), i2cFreq(freq) {}

void MPU6050Driver::init()
{
    // Inicializar bus I2C con pines personalizados
    wire.begin(sdaPin, sclPin, i2cFreq);

    // Verificar conexión
    uint8_t whoami = 0;
    if (!readRegister(REG_WHO_AM_I, whoami) || whoami != WHO_AM_I_MPU6050)
    {
        initialized = false;
        return;
    }

    // 1. Reset del dispositivo
    writeRegister(REG_PWR_MGMT_1, 0x80);  // Reset
    delay(100);

    // 2. Salir de modo sueño, usar PLL con referencia X gyro
    writeRegister(REG_PWR_MGMT_1, 0x01);
    delay(10);

    // 3. Configurar DLPF (Digital Low Pass Filter) - Config register
    // 0x03 = DLPF 44Hz, sample rate 1kHz
    writeRegister(REG_CONFIG, 0x03);

    // 4. Configurar rangos (actualiza escalas internas)
    updateScales();
    writeRegister(REG_GYRO_CONFIG, static_cast<uint8_t>(gyroRange) << 3);
    writeRegister(REG_ACCEL_CONFIG, static_cast<uint8_t>(accelRange) << 3);

    // 5. Deshabilitar FIFO, I2C Master, etc.
    writeRegister(REG_USER_CTRL, 0x00);
    writeRegister(REG_FIFO_EN, 0x00);
    writeRegister(REG_I2C_MST_CTRL, 0x00);

    // 6. Configurar signal path reset (opcional)
    writeRegister(REG_SIGNAL_PATH, 0x07);  // Reset gyro, accel, temp
    delay(10);

    initialized = true;
}

void MPU6050Driver::update()
{
    // Para el MPU6050 básico, no se requiere actualización periódica
    // Las lecturas son on-demand via read()
    // Aquí se podría implementar filtrado, fusión, etc. en el futuro
}

// --- I2C Helpers ---

bool MPU6050Driver::writeRegister(uint8_t reg, uint8_t value)
{
    wire.beginTransmission(i2cAddr);
    wire.write(reg);
    wire.write(value);
    return wire.endTransmission() == 0;
}

bool MPU6050Driver::readRegister(uint8_t reg, uint8_t &value)
{
    wire.beginTransmission(i2cAddr);
    wire.write(reg);
    if (wire.endTransmission(false) != 0) return false;

    if (wire.requestFrom(i2cAddr, (uint8_t)1) != 1) return false;
    value = wire.read();
    return true;
}

bool MPU6050Driver::readRegisters(uint8_t reg, uint8_t *buffer, size_t len)
{
    wire.beginTransmission(i2cAddr);
    wire.write(reg);
    if (wire.endTransmission(false) != 0) return false;

    return wire.requestFrom(i2cAddr, len) == len && wire.readBytes(buffer, len) == len;
}

int16_t MPU6050Driver::readInt16(uint8_t regHigh)
{
    uint8_t buffer[2];
    if (!readRegisters(regHigh, buffer, 2)) return 0;
    return (int16_t)((buffer[0] << 8) | buffer[1]);
}

// --- Conversión raw -> unidades físicas ---

float MPU6050Driver::convertAccel(int16_t raw) const
{
    return (raw / accelScale) * 9.80665f;  // m/s²
}

float MPU6050Driver::convertGyro(int16_t raw) const
{
    return raw / gyroScale;  // °/s
}

float MPU6050Driver::convertTemp(int16_t raw) const
{
    // Fórmula del datasheet: Temp(°C) = (TEMP_OUT / 340) + 36.53
    return (raw / 340.0f) + 36.53f;
}

void MPU6050Driver::updateScales()
{
    switch (accelRange)
    {
        case AccelRange::RANGE_2G:  accelScale = 16384.0f; break;  // LSB/g
        case AccelRange::RANGE_4G:  accelScale = 8192.0f;  break;
        case AccelRange::RANGE_8G:  accelScale = 4096.0f;  break;
        case AccelRange::RANGE_16G: accelScale = 2048.0f;  break;
    }

    switch (gyroRange)
    {
        case GyroRange::RANGE_250DPS:  gyroScale = 131.0f;   break;  // LSB/(°/s)
        case GyroRange::RANGE_500DPS:  gyroScale = 65.5f;    break;
        case GyroRange::RANGE_1000DPS: gyroScale = 32.8f;    break;
        case GyroRange::RANGE_2000DPS: gyroScale = 16.4f;    break;
    }
}

// --- IDriverIMU Implementation ---

void MPU6050Driver::setAccelRange(AccelRange range)
{
    accelRange = range;
    if (initialized)
    {
        updateScales();
        writeRegister(REG_ACCEL_CONFIG, static_cast<uint8_t>(range) << 3);
    }
}

void MPU6050Driver::setGyroRange(GyroRange range)
{
    gyroRange = range;
    if (initialized)
    {
        updateScales();
        writeRegister(REG_GYRO_CONFIG, static_cast<uint8_t>(range) << 3);
    }
}

IMUData MPU6050Driver::read()
{
    IMUData data;
    data.timestamp = millis();

    if (!initialized) return data;

    // Leer 14 bytes de golpe: ACCEL_XOUT_H (0x3B) a GYRO_ZOUT_L (0x48)
    // Orden: AX, AY, AZ, TEMP, GX, GY, GZ (cada uno 2 bytes = 14 bytes)
    uint8_t buffer[14];
    if (!readRegisters(REG_ACCEL_XOUT_H, buffer, 14))
    {
        return data;  // Retorna ceros si error I2C
    }

    // Convertir con offsets aplicados
    int16_t ax = (int16_t)((buffer[0] << 8) | buffer[1]) - accelOffsetX;
    int16_t ay = (int16_t)((buffer[2] << 8) | buffer[3]) - accelOffsetY;
    int16_t az = (int16_t)((buffer[4] << 8) | buffer[5]) - accelOffsetZ;

    int16_t tempRaw = (int16_t)((buffer[6] << 8) | buffer[7]);

    int16_t gx = (int16_t)((buffer[8] << 8) | buffer[9]) - gyroOffsetX;
    int16_t gy = (int16_t)((buffer[10] << 8) | buffer[11]) - gyroOffsetY;
    int16_t gz = (int16_t)((buffer[12] << 8) | buffer[13]) - gyroOffsetZ;

    data.accelX = convertAccel(ax);
    data.accelY = convertAccel(ay);
    data.accelZ = convertAccel(az);

    data.gyroX = convertGyro(gx);
    data.gyroY = convertGyro(gy);
    data.gyroZ = convertGyro(gz);

    data.temperature = convertTemp(tempRaw);

    return data;
}

IMURawData MPU6050Driver::readRaw()
{
    IMURawData data;

    if (!initialized) return data;

    uint8_t buffer[14];
    if (!readRegisters(REG_ACCEL_XOUT_H, buffer, 14))
    {
        return data;
    }

    data.accelX = (int16_t)((buffer[0] << 8) | buffer[1]);
    data.accelY = (int16_t)((buffer[2] << 8) | buffer[3]);
    data.accelZ = (int16_t)((buffer[4] << 8) | buffer[5]);
    data.temperature = (int16_t)((buffer[6] << 8) | buffer[7]);
    data.gyroX = (int16_t)((buffer[8] << 8) | buffer[9]);
    data.gyroY = (int16_t)((buffer[10] << 8) | buffer[11]);
    data.gyroZ = (int16_t)((buffer[12] << 8) | buffer[13]);

    return data;
}

bool MPU6050Driver::selfTest()
{
    if (!initialized) return false;

    // Habilitar self-test en registros de config
    // Self-test: bit 5-7 en GYRO_CONFIG y ACCEL_CONFIG
    uint8_t gyroConfig, accelConfig;
    readRegister(REG_GYRO_CONFIG, gyroConfig);
    readRegister(REG_ACCEL_CONFIG, accelConfig);

    // Activar self-test en todos los ejes
    writeRegister(REG_GYRO_CONFIG, gyroConfig | 0xE0);  // Bits 5,6,7 = X,Y,Z gyro self-test
    writeRegister(REG_ACCEL_CONFIG, accelConfig | 0xE0); // Bits 5,6,7 = X,Y,Z accel self-test

    delay(20);  // Esperar estabilización

    // Leer valores con self-test activado
    IMURawData stData = readRaw();

    // Desactivar self-test
    writeRegister(REG_GYRO_CONFIG, gyroConfig);
    writeRegister(REG_ACCEL_CONFIG, accelConfig);

    // Leer valores normales
    IMURawData normalData = readRaw();

    // Evaluar: la diferencia debe estar dentro de rangos esperados
    // (Simplificado: solo verificar que hubo cambio significativo)
    bool accelOK = (abs(stData.accelX - normalData.accelX) > 1000) &&
                   (abs(stData.accelY - normalData.accelY) > 1000) &&
                   (abs(stData.accelZ - normalData.accelZ) > 1000);

    bool gyroOK = (abs(stData.gyroX - normalData.gyroX) > 1000) &&
                  (abs(stData.gyroY - normalData.gyroY) > 1000) &&
                  (abs(stData.gyroZ - normalData.gyroZ) > 1000);

    return accelOK && gyroOK;
}

bool MPU6050Driver::calibrateGyro(uint16_t samples)
{
    if (!initialized) return false;

    int32_t sumX = 0, sumY = 0, sumZ = 0;

    // Leer muestras en reposo
    for (uint16_t i = 0; i < samples; i++)
    {
        IMURawData raw = readRaw();
        sumX += raw.gyroX;
        sumY += raw.gyroY;
        sumZ += raw.gyroZ;
        delay(2);  // ~500 Hz sample rate
    }

    gyroOffsetX = sumX / samples;
    gyroOffsetY = sumY / samples;
    gyroOffsetZ = sumZ / samples;

    calibrated = true;
    return true;
}

bool MPU6050Driver::calibrateAccel(uint16_t samples)
{
    if (!initialized) return false;

    int32_t sumX = 0, sumY = 0, sumZ = 0;

    // Calibración simple: asumir sensor horizontal (Z = +1g, X=Y=0)
    // Para calibración completa 6-posiciones, se requeriría interacción del usuario
    for (uint16_t i = 0; i < samples; i++)
    {
        IMURawData raw = readRaw();
        sumX += raw.accelX;
        sumY += raw.accelY;
        sumZ += raw.accelZ;
        delay(2);
    }

    // Offset para dejar X=0, Y=0, Z=+1g (en LSB)
    int16_t targetZ = static_cast<int16_t>(accelScale);  // 1g en LSB según rango actual

    accelOffsetX = sumX / samples;
    accelOffsetY = sumY / samples;
    accelOffsetZ = (sumZ / samples) - targetZ;

    calibrated = true;
    return true;
}

bool MPU6050Driver::isConnected() const
{
    uint8_t whoami = 0;
    // Necesitamos const_cast porque Wire no es const-correct
    const_cast<MPU6050Driver *>(this)->wire.beginTransmission(i2cAddr);
    const_cast<MPU6050Driver *>(this)->wire.write(REG_WHO_AM_I);
    if (const_cast<MPU6050Driver *>(this)->wire.endTransmission(false) != 0) return false;
    if (const_cast<MPU6050Driver *>(this)->wire.requestFrom(i2cAddr, (uint8_t)1) != 1) return false;
    whoami = const_cast<MPU6050Driver *>(this)->wire.read();
    return whoami == WHO_AM_I_MPU6050;
}

uint8_t MPU6050Driver::getDeviceID() const
{
    uint8_t whoami = 0;
    const_cast<MPU6050Driver *>(this)->wire.beginTransmission(i2cAddr);
    const_cast<MPU6050Driver *>(this)->wire.write(REG_WHO_AM_I);
    if (const_cast<MPU6050Driver *>(this)->wire.endTransmission(false) != 0) return 0;
    if (const_cast<MPU6050Driver *>(this)->wire.requestFrom(i2cAddr, (uint8_t)1) != 1) return 0;
    whoami = const_cast<MPU6050Driver *>(this)->wire.read();
    return whoami;
}

void MPU6050Driver::sleep()
{
    if (initialized)
    {
        writeRegister(REG_PWR_MGMT_1, 0x40);  // Bit 6 = SLEEP
    }
}

void MPU6050Driver::wakeUp()
{
    if (initialized)
    {
        writeRegister(REG_PWR_MGMT_1, 0x01);  // Bit 6 = 0, Clock = PLL X gyro
        delay(10);  // Esperar estabilización oscilador
    }
}

void MPU6050Driver::setOffsets(int16_t gx, int16_t gy, int16_t gz,
                               int16_t ax, int16_t ay, int16_t az)
{
    gyroOffsetX = gx; gyroOffsetY = gy; gyroOffsetZ = gz;
    accelOffsetX = ax; accelOffsetY = ay; accelOffsetZ = az;
    calibrated = true;
}

} // namespace robolib