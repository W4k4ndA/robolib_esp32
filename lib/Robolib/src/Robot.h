#ifndef ROBOT_H
#define ROBOT_H

#include <vector>

#include "Interfaces/IRobotModule.h"
#include "Services/DistanceService.h"
#include "Services/HBridgeMotorService.h"
#include "Services/AnalogLineSensorService.h"
#include "Services/DigitalLineSensorService.h"

namespace robolib
{
    // Forward declaration
    class RobotBuilder;

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

    /**
     * @brief Robot compuesto por motores y sensores de distancia.
     *
     * Responsabilidades:
     * - Gestiona ciclo de vida de drivers (ownership: los elimina en destructor)
     * - Expone API unificada para control diferencial y lecturas de sensores
     * - Valida índices y punteros nulos antes de delegar a drivers
     *
     * Diseño:
     * - Constructor privado + RobotBuilder: construcción fluida y segura
     * - Rule of Three: destructor, copy constructor/assignment = delete (no copiable)
     * - Drivers almacenados como punteros crudos (owner) por simplicidad embebida
     *   (migración futura: std::unique_ptr para Rule of Zero)
     *
     * Ejemplo de uso:
     * @code
     * auto robot = RobotBuilder()
     *     .addMotor(new HBridgeMotorDriver(5, 18, 0, 1))   // Motor izquierdo
     *     .addMotor(new HBridgeMotorDriver(19, 21, 2, 3))  // Motor derecho
     *     .addDistanceSensor(new HCSR04SensorDriver(4, 16)) // Sensor frontal
     *     .build();
     *
     * robot->moveDifferential(200, 200);  // Adelante
     * float dist = robot->getDistance(0); // Leer sensor 0
     * delete robot; // Libera drivers
     * @endcode
     *
     * @see RobotBuilder, IDriverMotor, IDriverDistanceSensor
     * @ingroup core
     */
    class Robot
    {
    private:
        std::vector<IRobotModule *> modules; ///< lista de modulos del robot (incluye
                                             /// todos los drivers)(owner>

        Robot() = default;         // Constructor privado. Solo RobotBuilder puede construir
        friend class RobotBuilder; // friend permie manipular variables privadas desde RobotBuilder

        // No copiable (ownership exclusiva)
        Robot(const Robot &) = delete;
        Robot &operator=(const Robot &) = delete;

        // Servicios como miembros (acceso directo con .)
        DistanceService distanceSensor{modules};
        HBridgeMotorService dcMotor{modules};
        AnalogLineSensorService analogLineSensor{modules};
        DigitalLineSensorService digitalLineSensor{modules};


    public:
        
        /// Getters para sintaxis con -> (punteros a servicios)
        DistanceService* distance() { return &distanceSensor; }
        // const DistanceService* distance() const { return &distanceSensor; }

        HBridgeMotorService* motors() { return &dcMotor; }
        // const HBridgeMotorService* motors() const { return &dcMotor; }

        AnalogLineSensorService* analogLine() { return &analogLineSensor; }
        // const AnalogLineSensorService* analogLine() const { return &analogLineSensor; }

        DigitalLineSensorService* digitalLine() { return &digitalLineSensor; }
        // const DigitalLineSensorService* digitalLine() const { return &digitalLineSensor; }

        /**
         * @brief Destructor: libera todos los drivers inyectados.
         *
         * Itera ambos vectores y llama delete en cada puntero no nulo.
         * Luego limpia los vectores.
         *
         * @post Todos los drivers destruidos, memoria liberada.
         */
        ~Robot();

        /**
         * @brief Inicializa todos los modulos del robot
         */
        void begin();

        void update();
    };

    //----------------CLASE ROBOT BUILDER-----------------------------------------------------
    /**
     * @file Robot.h
     * @brief Builder para construcción fluida de Robot (Builder Pattern).
     *
     * Permite configurar el robot paso a paso antes de construirlo:
     * - Encadena llamadas: builder.addMotor(...).addSensor(...).build()
     * - Transfiere ownership al Robot final (std::move)
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
         *
         * Inicializa vectores vacíos.
         */
        RobotBuilder() = default;

        /**
         * @brief Añade un modulo al robot (Actuadores y/o Sensores).
         *
         * @param moodule Puntero a implementación de IRobotDriver (owner transferido).
         *              Debe ser puntero válido (no nullptr) y no estar ya en otro Robot.
         * @return Referencia a *this para encadenamiento.
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
         * @post Vectores internos del builder quedan vacíos (std::move).
         *       Builder reutilizable para otro robot.
         *
         * @note Responsabilidad del llamador: delete robot cuando termine.
         */
        Robot *build();
    };

} // namespace robolib

#endif