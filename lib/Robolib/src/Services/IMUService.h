#ifndef IMU_SERVICE_H
#define IMU_SERVICE_H

/**
 * @file IMUService.h
 * @brief Servicio para gestión centralizada de IMU (MPU6050, etc.).
 *
 * Fachada que simplifica el acceso a sensores IMU registrados en el robot.
 * Proporciona API de alto nivel para lectura de orientación, aceleración,
 * velocidad angular y utilidades de navegación.
 *
 * Hereda de ModuleHelper para búsqueda y filtrado genérico.
 *
 * @ingroup services
 */

#include "Helper.h"
#include <cstdint>
#include <cmath>
#include "../Interfaces/IDriverIMU.h"

namespace robolib
{

    /**
     * @class IMUService
     * @brief Servicio de alto nivel para Unidades de Medición Inercial.
     *
     * Proporciona API simplificada para:
     * - Lectura de datos crudos (acelerómetro, giroscopio, temperatura)
     * - Cálculo de orientación (roll, pitch, yaw)
     * - Configuración de rangos
     * - Calibración de offsets
     * - Conteo y verificación de existencia
     *
     * @note Para yaw (rumbo) real se requiere magnetómetro (MPU9250) o
     *       fusión sensor con filtro complementario/Kalman.
     *       Este servicio calcula yaw solo por integración del giroscopio
     *       (sujeto a deriva).
     *
     * @code
     * auto* imu = robot->imu;
     * if (imu->exists()) {
     *     imu->setAccelRange(AccelRange::RANGE_4G);
     *     imu->setGyroRange(GyroRange::RANGE_500DPS);
     *     imu->calibrateGyro(500);  // Robot QUIETO
     *
     *     IMUData data = imu->read(0);
     *     float roll  = imu->getRoll(data);
     *     float pitch = imu->getPitch(data);
     *     float yaw   = imu->getYaw();  // Requiere update() periódico
     * }
     * @endcode
     *
     * @ingroup services
     */
    class IMUService : public ModuleHelper
    {
    private:
        // Para cálculo de yaw por integración
        float yawAngle = 0.0f;           ///< Ángulo yaw acumulado (grados)
        uint32_t lastUpdateTime = 0;     ///< Timestamp última actualización

    public:
        using ModuleHelper::ModuleHelper; // Hereda constructor

        /**
         * @brief Obtiene lectura completa del IMU.
         *
         * @param index Índice del IMU (0 por defecto).
         * @return IMUData con aceleración (m/s²), giroscopio (°/s), temperatura (°C).
         *         Retorna estructura con ceros si el sensor no existe.
         */
        IMUData read(size_t index = 0) const
        {
            auto *imu = getModule<IDriverIMU>(index);
            return imu ? imu->read() : IMUData{};
        }

        /**
         * @brief Obtiene lectura cruda del IMU (sin escalar).
         *
         * @param index Índice del IMU (0 por defecto).
         * @return IMURawData con valores ADC de 16 bits.
         */
        IMURawData readRaw(size_t index = 0) const
        {
            auto *imu = getModule<IDriverIMU>(index);
            return imu ? imu->readRaw() : IMURawData{};
        }

        /**
         * @brief Configura el rango del acelerómetro.
         *
         * @param index Índice del IMU (0 por defecto).
         * @param range Rango deseado (RANGE_2G, RANGE_4G, RANGE_8G, RANGE_16G).
         */
        void setAccelRange(size_t index = 0, AccelRange range = AccelRange::RANGE_4G)
        {
            auto *imu = getModule<IDriverIMU>(index);
            if (imu) imu->setAccelRange(range);
        }

        /**
         * @brief Configura el rango del giroscopio.
         *
         * @param index Índice del IMU (0 por defecto).
         * @param range Rango deseado (RANGE_250DPS, RANGE_500DPS, RANGE_1000DPS, RANGE_2000DPS).
         */
        void setGyroRange(size_t index = 0, GyroRange range = GyroRange::RANGE_500DPS)
        {
            auto *imu = getModule<IDriverIMU>(index);
            if (imu) imu->setGyroRange(range);
        }

        /**
         * @brief Obtiene el rango actual del acelerómetro.
         * @param index Índice del IMU (0 por defecto).
         * @return Rango configurado.
         */
        AccelRange getAccelRange(size_t index = 0) const
        {
            auto *imu = getModule<IDriverIMU>(index);
            return imu ? imu->getAccelRange() : AccelRange::RANGE_4G;
        }

        /**
         * @brief Obtiene el rango actual del giroscopio.
         * @param index Índice del IMU (0 por defecto).
         * @return Rango configurado.
         */
        GyroRange getGyroRange(size_t index = 0) const
        {
            auto *imu = getModule<IDriverIMU>(index);
            return imu ? imu->getGyroRange() : GyroRange::RANGE_500DPS;
        }

        /**
         * @brief Ejecuta auto-test del sensor.
         *
         * @param index Índice del IMU (0 por defecto).
         * @return true si el auto-test pasa.
         */
        bool selfTest(size_t index = 0) const
        {
            auto *imu = getModule<IDriverIMU>(index);
            return imu ? imu->selfTest() : false;
        }

        /**
         * @brief Calibra offsets del giroscopio (bias en reposo).
         *
         * @param index Índice del IMU (0 por defecto).
         * @param samples Número de muestras para promediar (default: 200).
         * @return true si calibración exitosa.
         *
         * @warning El robot DEBE estar completamente QUIETO durante la calibración.
         *          Duración aproximada: samples * 2 ms.
         */
        bool calibrateGyro(size_t index = 0, uint16_t samples = 200)
        {
            auto *imu = getModule<IDriverIMU>(index);
            return imu ? imu->calibrateGyro(samples) : false;
        }

        /**
         * @brief Calibra offsets del acelerómetro.
         *
         * Calibración simple asumiendo sensor horizontal (Z = +1g).
         * Para calibración de 6 posiciones, usar el driver directamente.
         *
         * @param index Índice del IMU (0 por defecto).
         * @param samples Muestras por posición (default: 100).
         * @return true si calibración exitosa.
         */
        bool calibrateAccel(size_t index = 0, uint16_t samples = 100)
        {
            auto *imu = getModule<IDriverIMU>(index);
            return imu ? imu->calibrateAccel(samples) : false;
        }

        /**
         * @brief Verifica si el IMU está conectado y responde.
         * @param index Índice del IMU (0 por defecto).
         * @return true si el sensor responde (WHO_AM_I correcto).
         */
        bool isConnected(size_t index = 0) const
        {
            auto *imu = getModule<IDriverIMU>(index);
            return imu ? imu->isConnected() : false;
        }

        /**
         * @brief Obtiene la ID del dispositivo (WHO_AM_I).
         * @param index Índice del IMU (0 por defecto).
         * @return Valor del registro (0x68 para MPU6050).
         */
        uint8_t getDeviceID(size_t index = 0) const
        {
            auto *imu = getModule<IDriverIMU>(index);
            return imu ? imu->getDeviceID() : 0;
        }

        /**
         * @brief Pone el IMU en modo sueño (bajo consumo).
         * @param index Índice del IMU (0 por defecto).
         */
        void sleep(size_t index = 0)
        {
            auto *imu = getModule<IDriverIMU>(index);
            if (imu) imu->sleep();
        }

        /**
         * @brief Despierta el IMU del modo sueño.
         * @param index Índice del IMU (0 por defecto).
         */
        void wakeUp(size_t index = 0)
        {
            auto *imu = getModule<IDriverIMU>(index);
            if (imu) imu->wakeUp();
        }

        // ===== CÁLCULO DE ORIENTACIÓN =====

        /**
         * @brief Calcula Roll (rotación eje X) en grados.
         *
         * Fórmula: roll = atan2(accelY, accelZ) * 180/PI
         * Rango: -180° a +180°
         * - Positivo: inclinación a la derecha
         * - Negativo: inclinación a la izquierda
         *
         * @param data Datos del IMU (obtenidos via read()).
         * @return Ángulo roll en grados.
         */
        float getRoll(const IMUData &data) const
        {
            return atan2(data.accelY, data.accelZ) * 180.0f / M_PI;
        }

        /**
         * @brief Calcula Pitch (rotación eje Y) en grados.
         *
         * Fórmula: pitch = atan2(-accelX, sqrt(accelY² + accelZ²)) * 180/PI
         * Rango: -90° a +90°
         * - Positivo: morro arriba
         * - Negativo: morro abajo
         *
         * @param data Datos del IMU (obtenidos via read()).
         * @return Ángulo pitch en grados.
         */
        float getPitch(const IMUData &data) const
        {
            return atan2(-data.accelX, sqrt(data.accelY * data.accelY + data.accelZ * data.accelZ)) * 180.0f / M_PI;
        }

        /**
         * @brief Calcula Yaw (rotación eje Z) por integración del giroscopio.
         *
         * Integra velocidad angular Z para estimar rumbo.
         * **Sujeto a deriva** (drift) sin magnetómetro o filtro de fusión.
         *
         * Debe llamarse periódicamente (ej. en loop principal) para acumular.
         * Usar resetYaw() para reiniciar.
         *
         * @param index Índice del IMU (0 por defecto).
         * @return Ángulo yaw actual en grados (0-360, normalizado).
         */
        float getYaw(size_t index = 0)
        {
            auto *imu = getModule<IDriverIMU>(index);
            if (!imu) return yawAngle;

            IMUData data = imu->read();
            uint32_t now = millis();

            if (lastUpdateTime > 0)
            {
                float dt = (now - lastUpdateTime) / 1000.0f;  // segundos
                yawAngle += data.gyroZ * dt;  // Integración simple
            }
            lastUpdateTime = now;

            // Normalizar a [0, 360)
            while (yawAngle >= 360.0f) yawAngle -= 360.0f;
            while (yawAngle < 0.0f) yawAngle += 360.0f;

            return yawAngle;
        }

        /**
         * @brief Reinicia el ángulo yaw a cero.
         *
         * Útil al inicio o para recalibrar rumbo referencial.
         *
         * @param angle Nuevo valor de yaw en grados (default: 0).
         */
        void resetYaw(float angle = 0.0f)
        {
            yawAngle = angle;
            lastUpdateTime = millis();
        }

        /**
         * @brief Obtiene el ángulo yaw actual sin actualizar.
         * @return Yaw actual en grados [0, 360).
         */
        float getYawAngle() const { return yawAngle; }

        // ===== UTILIDADES DE MOVIMIENTO =====

        /**
         * @brief Detecta si el robot está en caída libre.
         *
         * Compara magnitud de aceleración con gravedad.
         * En caída libre, magnitud ≈ 0 m/s².
         *
         * @param data Datos del IMU.
         * @param threshold Umbral en m/s² (default: 2.0 ≈ 0.2g).
         * @return true si aceleración total < threshold.
         */
        bool isFreeFall(const IMUData &data, float threshold = 2.0f) const
        {
            float mag = sqrt(data.accelX * data.accelX +
                           data.accelY * data.accelY +
                           data.accelZ * data.accelZ);
            return mag < threshold;
        }

        /**
         * @brief Detecta impacto/golpe brusco.
         *
         * Compara magnitud de aceleración con umbral alto.
         *
         * @param data Datos del IMU.
         * @param threshold Umbral en m/s² (default: 30.0 ≈ 3g).
         * @return true si aceleración total > threshold.
         */
        bool isImpact(const IMUData &data, float threshold = 30.0f) const
        {
            float mag = sqrt(data.accelX * data.accelX +
                           data.accelY * data.accelY +
                           data.accelZ * data.accelZ);
            return mag > threshold;
        }

        /**
         * @brief Detecta si el robot está quieto (sin movimiento significativo).
         *
         * Verifica que giroscopio y variación de acelerómetro sean bajos.
         *
         * @param data Datos del IMU.
         * @param gyroThreshold Umbral giroscopio °/s (default: 2.0).
         * @param accelThreshold Umbral variación accel m/s² (default: 0.5).
         * @return true si quieto.
         */
        bool isStationary(const IMUData &data, float gyroThreshold = 2.0f, float accelThreshold = 0.5f) const
        {
            float gyroMag = sqrt(data.gyroX * data.gyroX +
                               data.gyroY * data.gyroY +
                               data.gyroZ * data.gyroZ);

            float accelMag = sqrt(data.accelX * data.accelX +
                                data.accelY * data.accelY +
                                data.accelZ * data.accelZ);

            // En reposo: accelMag ≈ 9.81 (gravedad), gyroMag ≈ 0
            return (gyroMag < gyroThreshold) && (fabs(accelMag - 9.81f) < accelThreshold);
        }

        /**
         * @brief Obtiene magnitud total de aceleración.
         * @param data Datos del IMU.
         * @return Magnitud en m/s².
         */
        float getAccelMagnitude(const IMUData &data) const
        {
            return sqrt(data.accelX * data.accelX +
                       data.accelY * data.accelY +
                       data.accelZ * data.accelZ);
        }

        /**
         * @brief Obtiene magnitud total de velocidad angular.
         * @param data Datos del IMU.
         * @return Magnitud en °/s.
         */
        float getGyroMagnitude(const IMUData &data) const
        {
            return sqrt(data.gyroX * data.gyroX +
                       data.gyroY * data.gyroY +
                       data.gyroZ * data.gyroZ);
        }

        // ===== CONTEO Y EXISTENCIA =====

        /**
         * @brief Cuenta IMUs registrados.
         * @return Número de IMUs registrados en el robot.
         */
        size_t count() const
        {
            return countModules<IDriverIMU>();
        }

        /**
         * @brief Verifica si hay al menos un IMU registrado.
         * @return true si hay IMUs registrados, false en caso contrario.
         */
        bool exists() const
        {
            return count() > 0;
        }
    };

} // namespace robolib

#endif // IMU_SERVICE_H