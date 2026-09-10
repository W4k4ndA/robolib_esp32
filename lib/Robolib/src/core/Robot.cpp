#include "Robot.h"

namespace robolib
{

    /**
     * @file Robot.cpp
     * @brief Implementación de Robot y RobotBuilder.
     *
     * Gestión de ciclo de vida de drivers con ownership explícita.
     * Validación defensiva de índices y punteros nulos.
     *
     * @ingroup core
     */

    // ============================================================================
    // Robot
    // ============================================================================

    /**
     * @brief Destructor: libera todos los drivers inyectados.
     *
     * Itera el vector de módulos y llama delete en cada puntero no nulo.
     * Luego limpia el vector.
     *
     * @post Todos los drivers destruidos, memoria liberada.
     */
    Robot::~Robot()
    {
        for (auto *m : modules)
        {
            delete m;
        }
        modules.clear();
    }

    /**
     * @brief Inicializa todos los módulos del robot.
     *
     * Llama init() en cada módulo registrado. Debe llamarse exactamente
     * una vez tras RobotBuilder::build() y antes de usar cualquier servicio.
     *
     * @post Todos los módulos inicializados y listos para operar.
     */
    void Robot::begin()
    {
        for (auto m : modules)
        {
            if (m)
                m->init();
        }
    }

    /**
     * @brief Ciclo de actualización general.
     *
     * Recorre la lista de módulos y llama al update() de cada uno
     * si es que lo tiene implementado (implementación por defecto vacía).
     *
     * Útil para: filtrado de señales, control PID, máquinas de estado.
     */
    void Robot::update()
    {
        for (auto m : modules)
        {
            if (m)
                m->update();
        }
    }

    /**
     * @brief Registra la función de loop del usuario.
     *
     * @param callback Puntero a función void() sin parámetros.
     *                 Se almacenará para ejecución posterior via run().
     *
     * @note Sobrescribe cualquier callback previo.
     * @note El callback debe ser una función sin captura (function pointer),
     *       no un std::function o lambda con captura.
     */
    void Robot::attachFunc(UserCallback callback)
    {
        userLoopCallback = callback;
    }

    /**
     * @brief Ejecuta la función de loop registrada.
     *
     * Invoca el callback registrado via attachFunc().
     * Si no hay callback registrado, no hace nada.
     *
     * Diseñado para ser llamado en bucle principal (while/loop).
     *
     * @code
     * while (true) {
     *     robot->run();
     *     delay(10);
     * }
     * @endcode
     */
    void Robot::run()
    {
        if (userLoopCallback)
        {
            userLoopCallback();
        }
    }

    // =========================================================
    // ROBOT BUILDER
    // =========================================================

    /**
     * @brief Agrega un nuevo módulo a la lista temporal de construcción.
     *
     * @param module Puntero al módulo (debe heredar de IRobotModule).
     *               Se transfiere ownership al builder.
     * @return Referencia a *this para encadenamiento fluido.
     *
     * @note El builder toma ownership del puntero.
     *       No eliminar manualmente tras añadir.
     * @note Silencia nullptr para permitir encadenamiento condicional.
     */
    RobotBuilder &RobotBuilder::addModule(IRobotModule *module)
    {
        if (module)
            tempModules.push_back(module);

        return *this; // Retorna el propio builder para permitir Fluent Interface
    }

    /**
     * @brief Construye el Robot con los módulos agregados.
     *
     * Flujo:
     * 1. Crea instancia Robot en heap
     * 2. Para cada módulo: init() + transfiere ownership a Robot
     * 3. Limpia lista temporal del builder
     *
     * @return Puntero a Robot completamente inicializado (heap).
     *         Owner: llamador debe delete.
     *
     * @post Builder queda en estado válido (vacío) para reutilización.
     */
    Robot *RobotBuilder::build()
    {
        // 1. Instanciación del objeto Robot principal
        Robot *robot = new Robot();

        // 2. Inicialización y transferencia de módulos
        for (auto *module : tempModules)
        {
            if (module)
            {
                // Inicializa el hardware del módulo (setup de pines, I2C, etc.)
                module->init();

                // Pasa el control del módulo a la instancia final del Robot
                robot->modules.push_back(module);
            }
        }

        // 3. Limpia la lista temporal del builder por seguridad
        tempModules.clear();

        return robot;
    }

} // namespace robolib