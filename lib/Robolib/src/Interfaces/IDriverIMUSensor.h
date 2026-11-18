#ifndef I_DRIVER_IMU_SENSOR_H
#define I_DRIVER_IMU_SENSOR_H

/**
 * @file IDriverIMUSensor.h
 * @brief Interfaz abstracta para sensores IMU (acelerómetro + giroscopio).
 *
 * Define el contrato que deben implementar todos los sensores IMU compatibles
 * con el sistema (MPU6050, MPU9250, ICM20948, etc.).
 *
 * Unidades del sistema:
 * - Aceleración: g (gravedad) — 1 g ≈ 9.81 m/s²
 * - Velocidad angular: grados/segundo (°/s)
 * - Temperatura: grados Celsius (°C)
 *
 * Valores de retorno especiales en lectura:
 * - NaN: error de lectura / sensor no disponible
 *
 * @ingroup drivers
 */

#include "IRobotModule.h"

namespace robolib
{

    /**
     * @brief Interfaz base para sensores IMU.
     *
     * Abstrae la medición de aceleración, rotación y temperatura permitiendo
     * intercambiar modelos de IMU sin afectar la lógica de navegación.
     *
     * @see MPU6050Driver
     * @ingroup drivers
     */
    class IDriverIMUSensor : public IRobotModule
    {
    public:
        /**
         * @brief Destructor virtual por defecto.
         *
         * Necesario para destrucción correcta de objetos derivados mediante
         * puntero a la interfaz (polimorfismo).
         */
        virtual ~IDriverIMUSensor() = default;

        /**
         * @brief Obtiene el tipo de módulo.
         *
         * @return ModuleType::IMUSensor
         */
        static constexpr ModuleType staticType() { return ModuleType::IMUSensor; }
        ModuleType getType() const override { return staticType(); }

        /**
         * @brief Lee la aceleración en los tres ejes.
         *
         * @param ax Referencia donde se almacena aceleración eje X (g).
         * @param ay Referencia donde se almacena aceleración eje Y (g).
         * @param az Referencia donde se almacena aceleración eje Z (g).
         * @return true si la lectura fue exitosa, false en caso de error de comunicación.
         */
        virtual bool readAcceleration(float &ax, float &ay, float &az) = 0;

        /**
         * @brief Lee la velocidad angular (giroscopio) en los tres ejes.
         *
         * @param gx Referencia donde se almacena giro eje X (°/s).
         * @param gy Referencia donde se almacena giro eje Y (°/s).
         * @param gz Referencia donde se almacena giro eje Z (°/s).
         * @return true si la lectura fue exitosa, false en caso de error de comunicación.
         */
        virtual bool readGyroscope(float &gx, float &gy, float &gz) = 0;

        /**
         * @brief Lee la temperatura interna del sensor.
         *
         * @return Temperatura en grados Celsius, o NaN si hay error de lectura.
         */
        virtual float readTemperature() = 0;

        /**
         * @brief Lee todos los datos (aceleración, giro y temperatura) en una sola operación.
         *
         * @param ax Referencia donde se almacena aceleración eje X (g).
         * @param ay Referencia donde se almacena aceleración eje Y (g).
         * @param az Referencia donde se almacena aceleración eje Z (g).
         * @param gx Referencia donde se almacena giro eje X (°/s).
         * @param gy Referencia donde se almacena giro eje Y (°/s).
         * @param gz Referencia donde se almacena giro eje Z (°/s).
         * @param temp Referencia donde se almacena temperatura (°C).
         * @return true si la lectura fue exitosa, false en caso de error de comunicación.
         *
         * @note Más eficiente que llamar por separado a readAcceleration/readGyroscope/readTemperature.
         */
        virtual bool readAll(float &ax, float &ay, float &az,
                             float &gx, float &gy, float &gz,
                             float &temp) = 0;

        /**
         * @brief Obtiene el ángulo de alabeo (roll) en grados.
         *
         * Roll: rotación alrededor del eje X (adelante-atrás).
         * Calculado a partir del acelerómetro (no sufre deriva).
         *
         * @return Ángulo en grados (-180° a +180°).
         */
        virtual float getRoll() = 0;

        /**
         * @brief Obtiene el ángulo de cabeceo (pitch) en grados.
         *
         * Pitch: rotación alrededor del eje Y (izquierda-derecha).
         * Calculado a partir del acelerómetro (no sufre deriva).
         *
         * @return Ángulo en grados (-180° a +180°).
         */
        virtual float getPitch() = 0;

        /**
         * @brief Obtiene el ángulo de guiñada (yaw) en grados.
         *
         * Yaw: rotación alrededor del eje Z (vertical).
         * Calculado por integración del giroscopio con fusión complementaria.
         * Sufrirá deriva gradual a menos que se añada un magnetómetro.
         *
         * @return Ángulo en grados (-180° a +180°).
         */
        virtual float getYaw() = 0;
    };

} // namespace robolib

#endif