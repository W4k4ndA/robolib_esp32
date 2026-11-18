#ifndef MPU6050_DRIVER_H
#define MPU6050_DRIVER_H

/**
 * @file MPU6050Driver.h
 * @brief Driver para sensor IMU MPU6050 sobre I2C.
 *
 * Implementa IDriverIMUSensor para el MPU6050 (6 ejes: 3 acelerómetro + 3 giroscopio).
 * El MPU6050 también incluye un sensor de temperatura interno.
 *
 * Especificaciones típicas:
 * - Voltaje: 3.3V (tolerante a 5V en VCC, pero lógica I2C a 3.3V)
 * - Interfaz: I2C (dirección 0x68 o 0x69 si AD0 = HIGH)
 * - Acelerómetro: rangos configurables ±2g, ±4g, ±8g, ±16g
 * - Giroscopio: rangos configurables ±250, ±500, ±1000, ±2000 °/s
 * - ADC: 16 bits por eje
 * - Temperatura interna: -40°C a +85°C
 *
 * Conexión típica ESP32 (I2C):
 * - VCC -> 3.3V
 * - GND -> GND
 * - SDA -> GPIO SDA del ESP32 (ej. 21)
 * - SCL -> GPIO SCL del ESP32 (ej. 22)
 * - INT -> (opcional) GPIO para interrupciones
 * - AD0 -> GND (dirección 0x68) o VCC (dirección 0x69)
 *
 * @ingroup drivers
 */

#include "../../../Interfaces/IDriverIMUSensor.h"
#include <Arduino.h>
#include <Wire.h>

namespace robolib
{

    /**
     * @brief Driver para sensor IMU MPU6050.
     *
     * Gestiona la comunicación I2C con el MPU6050 y entrega los datos ya
     * convertidos a unidades físicas (g, °/s, °C).
     *
     * Ejemplo de uso:
     * @code
     * MPU6050Driver imu(0x68, 21, 22);  // addr, SDA, SCL
     * imu.init();
     * float ax, ay, az, gx, gy, gz, temp;
     * if (imu.readAll(ax, ay, az, gx, gy, gz, temp)) {
     *     Serial.printf("Temp: %.1f C\n", temp);
     * }
     * @endcode
     *
     * @see IDriverIMUSensor
     * @ingroup drivers
     */
    class MPU6050Driver : public IDriverIMUSensor
    {
    private:
        uint8_t address;  ///< Dirección I2C del MPU6050 (0x68 o 0x69)
        int sdaPin;       ///< Pin GPIO SDA del bus I2C
        int sclPin;       ///< Pin GPIO SCL del bus I2C
        bool initialized; ///< Indica si init() se llamó correctamente

        // Rangos configurables (por defecto ±2g y ±250 °/s)
        uint8_t accelRange; ///< Rango acelerómetro (0=±2g, 1=±4g, 2=±8g, 3=±16g)
        uint8_t gyroRange;  ///< Rango giroscopio (0=±250, 1=±500, 2=±1000, 3=±2000 °/s)

        // Ángulos de orientación (grados)
        float roll;   ///< Ángulo de alabeo (°)
        float pitch;  ///< Ángulo de cabeceo (°)
        float yaw;    ///< Ángulo de guiñada (°)

        // Timestamp para integración del giroscopio
        unsigned long lastTime; ///< Última lectura en milisegundos

        // Factor de fusión complementaria (0.0 = solo giro, 1.0 = solo accel)
        static constexpr float ALPHA = 0.98f;

        // Factores de escala según el rango seleccionado
        static constexpr float ACCEL_SCALE[4] = {16384.0f, 8192.0f, 4096.0f, 2048.0f}; ///< LSB/g según rango
        static constexpr float GYRO_SCALE[4] = {131.0f, 65.5f, 32.8f, 16.4f};         ///< LSB/(°/s) según rango

        // Direcciones de registro MPU6050
        static constexpr uint8_t REG_SMPLRT_DIV = 0x19; ///< Divisor de frecuencia de muestreo
        static constexpr uint8_t REG_CONFIG = 0x1A;     ///< Configuración filtro digital
        static constexpr uint8_t REG_GYRO_CONFIG = 0x1B; ///< Configuración giroscopio
        static constexpr uint8_t REG_ACCEL_CONFIG = 0x1C; ///< Configuración acelerómetro
        static constexpr uint8_t REG_PWR_MGMT_1 = 0x6B; ///< Gestión de energía 1
        static constexpr uint8_t REG_PWR_MGMT_2 = 0x6C; ///< Gestión de energía 2
        static constexpr uint8_t REG_ACCEL_XOUT_H = 0x3B; ///< Acelerómetro X alto
        static constexpr uint8_t REG_TEMP_OUT_H = 0x41;   ///< Temperatura alto
        static constexpr uint8_t REG_GYRO_XOUT_H = 0x43;  ///< Giroscopio X alto
        static constexpr uint8_t REG_WHO_AM_I = 0x75;     ///< Quién soy (identificación)

        // Constantes de temperatura
        static constexpr float TEMP_OFFSET = 36.53f;    ///< Offset temperatura (°C)
        static constexpr float TEMP_SCALE = 340.0f;     ///< Escala temperatura (°C/LSB)

        /**
         * @brief Escribe un byte en un registro del MPU6050.
         * @param reg Registro destino.
         * @param data Valor a escribir.
         * @return true si la escritura fue exitosa.
         */
        bool writeReg(uint8_t reg, uint8_t data);

        /**
         * @brief Lee un byte de un registro del MPU6050.
         * @param reg Registro origen.
         * @param data Referencia donde se almacena el valor leído.
         * @return true si la lectura fue exitosa.
         */
        bool readReg(uint8_t reg, uint8_t &data);

        /**
         * @brief Lee múltiples bytes consecutivos desde un registro inicial.
         * @param reg Registro inicial.
         * @param buffer Buffer destino.
         * @param length Cantidad de bytes a leer.
         * @return true si la lectura fue exitosa.
         */
        bool readRegs(uint8_t reg, uint8_t *buffer, uint8_t length);

    public:
        /**
         * @brief Constructor.
         *
         * @param address Dirección I2C del MPU6050 (0x68 por defecto, 0x69 si AD0 = HIGH).
         * @param sdaPin  Pin GPIO SDA del bus I2C (por defecto 21 en ESP32).
         * @param sclPin  Pin GPIO SCL del bus I2C (por defecto 22 en ESP32).
         *
         * @note No inicializa el hardware; llamar a init() después.
         */
        MPU6050Driver(uint8_t address = 0x68, int sdaPin = 21, int sclPin = 22);

        /**
         * @brief Destructor.
         */
        ~MPU6050Driver() override = default;

        /**
         * @brief Inicializa el bus I2C y configura el MPU6050.
         *
         * Configura:
         * - Pines SDA/SCL del bus Wire.
         * - Rango acelerómetro ±2g (por defecto).
         * - Rango giroscopio ±250 °/s (por defecto).
         * - Frecuencia de muestreo 100 Hz.
         * - Filtro digital bajo (DLPF).
         * - Modo activo (sale del sleep).
         *
         * @post Sensor listo para leer con readAcceleration/readGyroscope/readAll.
         * @note Si el sensor no responde en I2C, init() retorna void pero `initialized`
         *       queda en false y las lecturas fallarán hasta reiniciar.
         */
        void init() override;

        /**
         * @brief Lee la aceleración en los tres ejes.
         *
         * @param ax Referencia donde se almacena aceleración eje X (g).
         * @param ay Referencia donde se almacena aceleración eje Y (g).
         * @param az Referencia donde se almacena aceleración eje Z (g).
         * @return true si la lectura fue exitosa, false en caso de error de comunicación.
         */
        bool readAcceleration(float &ax, float &ay, float &az) override;

        /**
         * @brief Lee la velocidad angular (giroscopio) en los tres ejes.
         *
         * @param gx Referencia donde se almacena giro eje X (°/s).
         * @param gy Referencia donde se almacena giro eje Y (°/s).
         * @param gz Referencia donde se almacena giro eje Z (°/s).
         * @return true si la lectura fue exitosa, false en caso de error de comunicación.
         */
        bool readGyroscope(float &gx, float &gy, float &gz) override;

        /**
         * @brief Lee la temperatura interna del sensor.
         *
         * @return Temperatura en grados Celsius, o NaN si hay error de lectura.
         */
        float readTemperature() override;

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
         */
        bool readAll(float &ax, float &ay, float &az,
                     float &gx, float &gy, float &gz,
                     float &temp) override;

        /**
         * @brief Obtiene el ángulo de alabeo (roll) en grados.
         *
         * Roll: rotación alrededor del eje X (adelante-atrás).
         * Calculado a partir del acelerómetro (no sufre deriva).
         *
         * @return Ángulo en grados (-180° a +180°).
         */
        float getRoll() override;

        /**
         * @brief Obtiene el ángulo de cabeceo (pitch) en grados.
         *
         * Pitch: rotación alrededor del eje Y (izquierda-derecha).
         * Calculado a partir del acelerómetro (no sufre deriva).
         *
         * @return Ángulo en grados (-180° a +180°).
         */
        float getPitch() override;

        /**
         * @brief Obtiene el ángulo de guiñada (yaw) en grados.
         *
         * Yaw: rotación alrededor del eje Z (vertical).
         * Calculado por integración del giroscopio con fusión complementaria.
         * Sufrirá deriva gradual a menos que se añada un magnetómetro.
         *
         * @return Ángulo en grados (-180° a +180°).
         */
        float getYaw() override;

        /**
         * @brief Establece el rango del acelerómetro.
         *
         * @param range 0 = ±2g, 1 = ±4g, 2 = ±8g, 3 = ±16g.
         */
        void setAccelRange(uint8_t range);

        /**
         * @brief Establece el rango del giroscopio.
         *
         * @param range 0 = ±250 °/s, 1 = ±500 °/s, 2 = ±1000 °/s, 3 = ±2000 °/s.
         */
        void setGyroRange(uint8_t range);

        /**
         * @brief Obtiene el rango actual del acelerómetro.
         * @return Rango configurado (0-3).
         */
        uint8_t getAccelRange() const { return accelRange; }

        /**
         * @brief Obtiene el rango actual del giroscopio.
         * @return Rango configurado (0-3).
         */
        uint8_t getGyroRange() const { return gyroRange; }
    };

} // namespace robolib

#endif