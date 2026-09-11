#ifndef I_ROBOT_MODULE_H
#define I_ROBOT_MODULE_H

/**
 * @file IRobotModule.h
 * @brief Interfaz base para todos los módulos del robot (módulo = driver de hardware).
 *
 * Define el contrato mínimo que debe implementar cualquier driver para ser gestionado
 * por el sistema Robot. Proporciona ciclo de vida estándar y identificación de tipo
 * sin requerir RTTI (Run-Time Type Information).
 *
 * @ingroup interfaces
 */

namespace robolib
{

    /**
     * @brief Tipos de módulos disponibles en el sistema.
     *
     * Usado para identificación de tipo en tiempo de compilación/ejecución
     * sin depender de RTTI (deshabilitado en Arduino/ESP32 por defecto).
     *
     * @note Cada interfaz derivada debe definir `staticType()` retornando
     *       el valor correspondiente de este enum.
     */
    enum class ModuleType
    {
        DCMotor,            ///< Motor DC con puente H
        DistanceSensor,     ///< Sensor de distancia (ultrasónico, ToF, etc.)
        DigitalLineSensor,  ///< Sensor de línea digital (GPIO)
        AnalogLineSensor,    ///< Sensor de línea analógico (ADC)
        LED                 /// < Controlador de LEDs
    };

    /**
     * @class IRobotModule
     * @brief Interfaz base para todos los módulos de hardware del robot.
     *
     * Cualquier driver que se agregue al Robot debe heredar de esta interfaz
     * (directamente o a través de una interfaz derivada como IDriverHBMotor).
     *
     * Proporciona:
     * - Ciclo de vida estándar: init() obligatorio, update() opcional
     * - Identificación de tipo sin RTTI via getType()/staticType()
     *
     * @see IDriverHBMotor, IDriverDistanceSensor, IDriverDigitalLineSensor, IDriverAnalogLineSensor
     * @ingroup interfaces
     */
    class IRobotModule
    {
    public:
        virtual ~IRobotModule() = default;

        /**
         * @brief Inicializa el hardware del módulo.
         *
         * Debe configurar pines, periféricos (PWM, I2C, SPI, ADC), registros,
         * y dejar el dispositivo en estado listo para operar.
         *
         * @post Módulo completamente inicializado y listo para recibir comandos.
         * @note Se llama exactamente una vez durante Robot::begin()
         */
        virtual void init() = 0;

        /**
         * @brief Actualización periódica de los módulos (opcional).
         *
         * Se llama en cada iteración de Robot::update().
         * Útil para: filtrado de señales, PID, máquinas de estado, lecturas continuas.
         *
         * @note Implementación por defecto vacía. Sobrescribir solo si necesario.
         */
        virtual void update() {}

        /**
         * @brief Obtiene el tipo de módulo en tiempo de ejecución.
         *
         * @return Valor del enum ModuleType correspondiente al tipo concreto.
         * @note Implementado automáticamente por las interfaces derivadas via staticType().
         */
        virtual ModuleType getType() const = 0;
    };

} // namespace robolib

#endif // I_ROBOT_MODULE_H