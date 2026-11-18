#ifndef IMU_SERVICE_H
#define IMU_SERVICE_H

/**
 * @file IMUService.h
 * @brief Servicio para gestión centralizada de sensores IMU.
 *
 * Fachada que simplifica el acceso a sensores IMU registrados en el robot.
 * Proporciona API unificada para acelerómetro, giroscopio y temperatura.
 *
 * @ingroup services
 */

#include <cstdint>
#include <cmath>
#include "Helper.h"
#include "../Interfaces/IDriverIMUSensor.h"

namespace robolib
{

    /**
     * @class IMUService
     * @brief Servicio de alto nivel para sensores IMU.
     *
     * Proporciona API simplificada para:
     * - Lectura de aceleración (g)
     * - Lectura de giroscopio (°/s)
     * - Lectura de temperatura (°C)
     * - Lectura combinada en una sola operación
     * - Conteo y verificación de existencia
     *
     * @code
     * auto* imu = robot->imu;
     * if (imu->exists()) {
     *     float ax, ay, az;
     *     imu->getAcceleration(0, ax, ay, az);
     *     float temp = imu->getTemperature(0);
     * }
     * @endcode
     *
     * @ingroup services
     */
    class IMUService : public ModuleHelper
    {
    public:
        using ModuleHelper::ModuleHelper;

        /**
         * @brief Lee la aceleración en los tres ejes de un sensor IMU.
         *
         * @param index Índice del sensor IMU (0 por defecto para el primero).
         * @param ax    Referencia donde se almacena aceleración eje X (g).
         * @param ay    Referencia donde se almacena aceleración eje Y (g).
         * @param az    Referencia donde se almacena aceleración eje Z (g).
         * @return true si la lectura fue exitosa, false si el sensor no existe o hay error.
         */
        bool getAcceleration(size_t index, float &ax, float &ay, float &az)
        {
            auto *sensor = getModule<IDriverIMUSensor>(index);
            if (sensor)
                return sensor->readAcceleration(ax, ay, az);
            ax = ay = az = 0.0f;
            return false;
        }

        /**
         * @brief Lee la velocidad angular (giroscopio) en los tres ejes de un sensor IMU.
         *
         * @param index Índice del sensor IMU (0 por defecto para el primero).
         * @param gx    Referencia donde se almacena giro eje X (°/s).
         * @param gy    Referencia donde se almacena giro eje Y (°/s).
         * @param gz    Referencia donde se almacena giro eje Z (°/s).
         * @return true si la lectura fue exitosa, false si el sensor no existe o hay error.
         */
        bool getGyroscope(size_t index, float &gx, float &gy, float &gz)
        {
            auto *sensor = getModule<IDriverIMUSensor>(index);
            if (sensor)
                return sensor->readGyroscope(gx, gy, gz);
            gx = gy = gz = 0.0f;
            return false;
        }

        /**
         * @brief Lee la temperatura interna de un sensor IMU.
         *
         * @param index Índice del sensor IMU (0 por defecto para el primero).
         * @return Temperatura en grados Celsius, o NaN si el sensor no existe o hay error.
         */
        float getTemperature(size_t index = 0)
        {
            auto *sensor = getModule<IDriverIMUSensor>(index);
            if (sensor)
                return sensor->readTemperature();
            return NAN;
        }

        /**
         * @brief Lee todos los datos (aceleración, giro y temperatura) de un sensor IMU.
         *
         * @param index Índice del sensor IMU (0 por defecto para el primero).
         * @param ax    Referencia donde se almacena aceleración eje X (g).
         * @param ay    Referencia donde se almacena aceleración eje Y (g).
         * @param az    Referencia donde se almacena aceleración eje Z (g).
         * @param gx    Referencia donde se almacena giro eje X (°/s).
         * @param gy    Referencia donde se almacena giro eje Y (°/s).
         * @param gz    Referencia donde se almacena giro eje Z (°/s).
         * @param temp  Referencia donde se almacena temperatura (°C).
         * @return true si la lectura fue exitosa, false si el sensor no existe o hay error.
         */
        bool getAll(size_t index,
                    float &ax, float &ay, float &az,
                    float &gx, float &gy, float &gz,
                    float &temp)
        {
            auto *sensor = getModule<IDriverIMUSensor>(index);
            if (sensor)
                return sensor->readAll(ax, ay, az, gx, gy, gz, temp);
            ax = ay = az = gx = gy = gz = 0.0f;
            temp = 0.0f;
            return false;
        }

        /**
         * @brief Obtiene el ángulo de alabeo (roll) de un sensor IMU.
         *
         * Roll: rotación alrededor del eje X (adelante-atrás).
         * Calculado a partir del acelerómetro (no sufre deriva).
         *
         * @param index Índice del sensor IMU (0 por defecto para el primero).
         * @return Ángulo en grados (-180° a +180°), o NaN si el sensor no existe o hay error.
         */
        float getRoll(size_t index = 0)
        {
            auto *sensor = getModule<IDriverIMUSensor>(index);
            if (sensor)
                return sensor->getRoll();
            return NAN;
        }

        /**
         * @brief Obtiene el ángulo de cabeceo (pitch) de un sensor IMU.
         *
         * Pitch: rotación alrededor del eje Y (izquierda-derecha).
         * Calculado a partir del acelerómetro (no sufre deriva).
         *
         * @param index Índice del sensor IMU (0 por defecto para el primero).
         * @return Ángulo en grados (-180° a +180°), o NaN si el sensor no existe o hay error.
         */
        float getPitch(size_t index = 0)
        {
            auto *sensor = getModule<IDriverIMUSensor>(index);
            if (sensor)
                return sensor->getPitch();
            return NAN;
        }

        /**
         * @brief Obtiene el ángulo de guiñada (yaw) de un sensor IMU.
         *
         * Yaw: rotación alrededor del eje Z (vertical).
         * Calculado por integración del giroscopio con fusión complementaria.
         * Sufrirá deriva gradual a menos que se añada un magnetómetro.
         *
         * @param index Índice del sensor IMU (0 por defecto para el primero).
         * @return Ángulo en grados (-180° a +180°), o NaN si el sensor no existe o hay error.
         */
        float getYaw(size_t index = 0)
        {
            auto *sensor = getModule<IDriverIMUSensor>(index);
            if (sensor)
                return sensor->getYaw();
            return NAN;
        }

        /**
         * @brief Cuenta sensores IMU registrados.
         * @return Número de sensores IMU registrados en el robot.
         */
        size_t count() const
        {
            return countModules<IDriverIMUSensor>();
        }

        /**
         * @brief Verifica si hay al menos un sensor IMU registrado.
         * @return true si hay sensores IMU registrados, false en caso contrario.
         */
        bool exists() const
        {
            return count() > 0;
        }
    };

} // namespace robolib

#endif