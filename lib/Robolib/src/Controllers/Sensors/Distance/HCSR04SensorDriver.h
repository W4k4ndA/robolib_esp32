#ifndef HCSR04_SENSOR_DRIVER_H
#define HCSR04_SENSOR_DRIVER_H

#include "../../../Interfaces/IDriverDistanceSensor.h"
#include <cstdint>

namespace robolib {

/**
 * @file HCSR04SensorDriver.h
 * @brief Driver para sensor ultrasónico HC-SR04 / HC-SR04P.
 * 
 * Implementa IDriverDistanceSensor usando medición por tiempo de vuelo
 * (Time of Flight) con pulso ultrasónico a 40 kHz.
 * 
 * @ingroup drivers
 */

/**
 * @brief Driver para sensor de distancia ultrasónico HC-SR04.
 * 
 * Especificaciones típicas:
 * - Rango: 2 cm – 400 cm (especificación); 3 cm – 350 cm (práctico)
 * - Resolución: ~0.3 cm
 * - Ángulo de haz: ~15°
 * - Voltaje: 5V (HC-SR04) o 3.3V/5V (HC-SR04P)
 * - Corriente: ~15 mA
 * 
 * Conexión típica ESP32:
 * - VCC -> 5V (o 3.3V para SR04P)
 * - Trig -> GPIO salida (ej. 5)
 * - Echo -> GPIO entrada (ej. 18) **con divisor de voltaje si sensor es 5V**
 * - GND -> GND
 * 
 * @warning HC-SR04 clásico trabaja a 5V. El pin Echo entrega 5V.
 *          **Usar divisor resistivo (1k/2k) o level shifter** para proteger GPIO ESP32 (3.3V).
 *          HC-SR04P es compatible 3.3V y no requiere divisor.
 * 
 * Limitaciones:
 * - No funciona bien en superficies blandas/absorbentes (alfombra, espuma)
 * - Ángulo estrecho: objeto debe estar frente al sensor
 * - Mínimo ~2-3 cm (zona ciega cerca del sensor)
 * - Interferencia entre múltiples sensores (usar multiplexado temporal)
 * 
 * Ejemplo de uso:
 * @code
 * HCSR04SensorDriver sensor(5, 18);  // Trig=5, Echo=18
 * sensor.begin();
 * float dist = sensor.measureCM();
 * if (dist > 0) Serial.printf("Distancia: %.1f cm\n", dist);
 * @endcode
 * 
 * @see IDriverDistanceSensor
 * @ingroup drivers
 */
class HCSR04SensorDriver : public IDriverDistanceSensor {
private:
    int trigPin;    ///< Pin GPIO Trigger (salida, pulso 10 µs)
    int echoPin;    ///< Pin GPIO Echo (entrada, mide ancho de pulso)

    static constexpr float SOUND_SPEED_CM_US = 0.0343f;  ///< Velocidad sonido: 343 m/s = 0.0343 cm/µs
    static constexpr uint32_t TIMEOUT_US = 30000;        ///< Timeout echo: 30 ms (~5 m ida+vuelta)
    static constexpr float MIN_DISTANCE_CM = 2.0f;       ///< Límite inferior válido
    static constexpr float MAX_DISTANCE_CM = 400.0f;     ///< Límite superior válido

public:
    /**
     * @brief Constructor.
     * 
     * @param triggerPin Pin GPIO conectado a Trig del sensor (salida)
     * @param echoPin    Pin GPIO conectado a Echo del sensor (entrada)
     * 
     * @note No configura hardware; llamar a begin() después.
     * @note Para HC-SR04 5V: echoPin DEBE tener divisor de voltaje a 3.3V.
     */
    HCSR04SensorDriver(uint8_t triggerPin, uint8_t echoPin);

    /**
     * @brief Destructor.
     * 
     * No requiere limpieza especial (pines GPIO se restauran en reset).
     */
    ~HCSR04SensorDriver() override = default;

    /**
     * @brief Inicializa pines del sensor.
     * 
     * Configura:
     * - trigPin como OUTPUT (inicial LOW)
     * - echoPin como INPUT
     * 
     * @post Sensor listo para medir con measureCM()
     */
    void init() override;

    /**
     * @brief Realiza medición de distancia por ultrasonidos.
     * 
     * Secuencia:
     * 1. Pulso LOW 2 µs en Trig (estabilización)
     * 2. Pulso HIGH 10 µs en Trig (disparo ultrasónico)
     * 3. Mide duración pulso HIGH en Echo con pulseIn()
     * 4. Calcula: distancia = (duración * velocidad_sonido) / 2
     * 5. Aplica límites [MIN_DISTANCE_CM, MAX_DISTANCE_CM]
     * 
     * @return Distancia en cm, o valor negativo si error:
     *         - -1.0f: timeout (pulseIn excedió TIMEOUT_US)
     *         - -2.0f: error de medición (duración = 0)
     * 
     * @note Bloqueante: hasta ~30 ms (timeout) + overhead.
     * @note pulseIn() tiene resolución ~1-2 µs -> precisión ~0.03 cm teórica,
     *       pero variabilidad real ~0.5-1 cm.
     * @note Para mayor robustez, promediar 3-5 lecturas consecutivas.
     */
    float measureCM() override;
};

} // namespace robolib

#endif