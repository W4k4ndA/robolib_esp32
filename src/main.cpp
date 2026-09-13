#include <Arduino.h>
#include <Robolib.h>

using namespace robolib;

/**
 * @brief Ejemplo completo de uso de Robolib.
 *
 * Robot con:
 * - 2 motores DC (tracción diferencial) en pines 3,4 (ch 5,6) y 7,8 (ch 9,10)
 * - 1 sensor ultrasónico HC-SR04 en Trig=1, Echo=2
 * - 1 sensor de línea digital TCRT5000 en GPIO 4
 */

// Instancia global del robot
Robot *robot = nullptr;

// Callback del estudiante (si lo desea)
void studentLoop()
{
    // Ejemplo: Leer distancia y mostrar por serial
    float distance = robot->distanceSensor->getCM();
    Serial.print(F("Distancia: "));
    Serial.print(distance);
    Serial.println(F(" cm"));
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        delay(10);

    // 1. Construir robot con Builder Pattern
    robot = RobotBuilder()
                // Motor izquierdo: IN1=3, IN2=4, CH1=5, CH2=6
                .addModule(new HBridgeMotorDriver(3, 4, 5, 6))
                // Motor derecho: IN1=7, IN2=8, CH1=9, CH2=10
                .addModule(new HBridgeMotorDriver(7, 8, 9, 10))
                // Sensor ultrasónico frontal: Trig=1, Echo=2
                .addModule(new HCSR04SensorDriver(1, 2))
                // Sensor línea digital (DO): GPIO 4
                .addModule(new TCRT5kDigitalDriver(4))
                // LED indicador: GPIO 13
                .addModule(new LedDriver(13))
                // Completar construcción
                .build();

    // 2. Inicializar TODO el hardware (pines, PWM, ADC, etc.)
    robot->begin();

    // 3. Registrar callback del estudiante
    robot->attachFunc(studentLoop);

    // 4. Encender un led
    robot->led->ledON();

    Serial.println(F("Robolib inicializado - Robot listo"));
}

void loop()
{
    // Ejecutar callback registrado (loop del estudiante)
    robot->run();

    // Pequeño delay para no saturar CPU
    delay(20);
}

// Fin del programa