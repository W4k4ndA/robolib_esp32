#ifndef ROBOT_H
#define ROBOT_H

/**
 * @file Robot.h
 * @brief Clase principal que representa un robot compuesto por drivers de hardware.
 *
 * Implementa el patrón **Composite** para agrupar múltiples actuadores y sensores
 * bajo una API unificada. Usa **inyección de dependencias** vía RobotBuilder
 * para desacoplar la configuración de hardware de la lógica de control.
 *
 * @ingroup core
 */

#include <vector>

#include "../Interfaces/IRobotModule.h"
#include "../Services/DistanceService.h"
#include "../Services/HBridgeMotorService.h"
#include "../Services/AnalogLineSensorService.h"
#include "../Services/DigitalLineSensorService.h"

namespace robolib
{
    /**
     * @brief Tipo de callback para función de loop del usuario.
     *
     * Permite al estudiante/usuario registrar su función loop() personalizada
     * que será ejecutada por Robot::run().
     */
    using UserCallback = void (*)();

    // Forward declaration
    class RobotBuilder;

    /**
     * @class Robot
     * @brief Robot compuesto por motores y sensores gestionados como módulos.
     *
     * Responsabilidades:
     * - Gestiona ciclo de vida de drivers (ownership: los elimina en destructor)
     * - Expone API unificada para control diferencial y lecturas de sensores
     * - Valida índices y punteros nulos antes de delegar a drivers
     * - Proporciona callback de usuario para loop educativo
     *
     * Diseño:
     * - Constructor privado + RobotBuilder: construcción fluida y segura
     * - Rule of Five: destructor, copy/move = delete (no copiable)
     * - Drivers almacenados como punteros crudos (owner) por simplicidad embebida
     *   (migración futura: std::unique_ptr para Rule of Zero)
     * - Servicios expuestos como punteros públicos directos para API intuitiva
     *
     * Ejemplo de uso:
     * @code
     * auto robot = RobotBuilder()
     *     .addModule(new HBridgeMotorDriver(5, 18, 0, 1))   // Motor izquierdo
     *     .addModule(new HBridgeMotorDriver(19, 21, 2, 3))  // Motor derecho
     *     .addModule(new HCSR04SensorDriver(4, 16))         // Sensor frontal
     *     .addModule(new TCRT5kDigitalDriver(2))            // Sensor línea
     *     .build();
     *
     * robot->begin();
     * robot->attachFunc([]() {
     *     // Loop del estudiante
     *     float dist = robot->distanceSensor->getCM(0);
     *     robot->motorDC->moveDifferential(100, 100);
     * });
     * while (true) robot->run();
     * @endcode
     *
     * @see RobotBuilder, IDriverHBMotor, IDriverDistanceSensor
     * @ingroup core
     */
    class Robot
    {
    private:
        UserCallback userLoopCallback = nullptr;           ///< Callback de loop del usuario
        std::vector<IRobotModule *> modules;               ///< Lista de módulos (ownership)

        // Servicios como miembros (acceso directo con .)
        DistanceService distanceService{modules};
        HBridgeMotorService dcMotorService{modules};
        AnalogLineSensorService analogLineService{modules};
        DigitalLineSensorService digitalLineService{modules};

        // Constructor privado. Solo RobotBuilder puede construir
        Robot() = default;
        friend class RobotBuilder;

        // No copiable (ownership exclusiva)
        Robot(const Robot &) = delete;
        Robot &operator=(const Robot &) = delete;

    public:
        /**
         * @brief Acceso a servicios de distancia.
         * Permite acceder al sensor o sensores de distancia instalados.
         * Retorna nullptr si no hay sensores registrados.
         */
        DistanceService *distanceSensor = &distanceService;

        /**
         * @brief Acceso a servicios de motores DC (puente H).
         * Permite control diferencial e individual de motores.
         * Retorna nullptr si no hay motores registrados.
         */
        HBridgeMotorService *motorDC = &dcMotorService;

        /**
         * @brief Acceso a servicios de sensores de línea analógicos.
         * Permite lectura analógica y configuración de umbrales.
         * Retorna nullptr si no hay sensores registrados.
         */
        AnalogLineSensorService *analogLineSensor = &analogLineService;

        /**
         * @brief Acceso a servicios de sensores de línea digitales.
         * Permite lectura digital (on/off) y configuración de lógica.
         * Retorna nullptr si no hay sensores registrados.
         */
        DigitalLineSensorService *digitalLineSensor = &digitalLineService;

        /**
         * @brief Destructor: libera todos los drivers inyectados.
         *
         * Itera el vector de módulos y llama delete en cada puntero no nulo.
         * Luego limpia el vector.
         *
         * @post Todos los drivers destruidos, memoria liberada.
         */
        ~Robot();

        /**
         * @brief Inicializa todos los módulos del robot.
         *
         * Llama init() en cada módulo registrado. Debe llamarse una vez
         * tras build() y antes de usar cualquier servicio.
         */
        void begin();

        /**
         * @brief Actualiza todos los módulos del robot.
         *
         * Llama update() en cada módulo que lo implemente.
         * Útil para filtrado, PID, máquinas de estado.
         */
        void update();

        /**
         * @brief Registra la función de loop del usuario.
         *
         * @param callback Puntero a función void() sin parámetros.
         *                 Se ejecutará en cada llamada a run().
         *
         * @code
         * robot->attachFunc([]() {
         *     float d = robot->distanceSensor->getCM(0);
         *     if (d < 20) robot->motorDC->stopAll();
         * });
         * @endcode
         */
        void attachFunc(UserCallback callback);

        /**
         * @brief Ejecuta la función de loop registrada.
         *
         * Llama al callback registrado via attachFunc().
         * Si no hay callback, no hace nada.
         * Diseñado para ser llamado en bucle principal (while/loop).
         *
         * @code
         * while (true) robot->run();
         * @endcode
         */
        void run();
    };

    //----------------CLASE ROBOT BUILDER-----------------------------------------------------
    /**
     * @class RobotBuilder
     * @brief Builder para construcción fluida de Robot (Builder Pattern).
     *
     * Permite configurar el robot paso a paso antes de construirlo:
     * - Encadena llamadas: builder.addModule(...).addModule(...).build()
     * - Transfiere ownership al Robot final
     * - Reutilizable: tras build(), el builder queda vacío para otro robot
     *
     * @ingroup core
     */
    class RobotBuilder
    {
    private:
        std::vector<IRobotModule *> tempModules;

    public:
        /**
         * @brief Constructor por defecto.
         * Inicializa vectores vacíos.
         */
        RobotBuilder() = default;

        /**
         * @brief Añade un módulo al robot (Actuadores y/o Sensores).
         *
         * @param module Puntero a implementación de IRobotModule (owner transferido).
         *               Debe ser puntero válido (no nullptr) y no estar ya en otro Robot.
         * @return Referencia a *this para encadenamiento fluido.
         *
         * @note El RobotBuilder toma ownership del puntero.
         *       No eliminar manualmente tras añadir.
         */
        RobotBuilder &addModule(IRobotModule *module);

        /**
         * @brief Construye el Robot transfiriendo ownership de drivers.
         *
         * @return Puntero a Robot nuevo (heap). Owner: llamador debe delete.
         *
         * @post Vectores internos del builder quedan vacíos.
         *       Builder reutilizable para otro robot.
         *
         * @note Responsabilidad del llamador: delete robot cuando termine.
         */
        Robot *build();
    };

} // namespace robolib

#endif