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

    Robot::~Robot()
    {
        // Liberar drivers de motores (owner)
        for (auto *m : modules)
        {
            delete m;
        }
        modules.clear();
    }

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
     * Recorre la lista de módulos y llama al update() de cada uno
     * si es que lo tiene implementado.
     */
    void Robot::update()
    {
        for (auto m : modules)
        {
            if (m)
                m->update();
        }
    }
 
    // =========================================================
    // ROBOT BUILDER
    // =========================================================
    /**
     * @brief Agrega un nuevo módulo a la lista temporal de construcción.
     * @param module Puntero al módulo (debe heredar de RobotModule).
     * @return Referencia a RobotBuilder para encadenar llamadas (.addModule().addModule()).
     */
    RobotBuilder &RobotBuilder::addModule(IRobotModule *module)
    {
        if (module)
            tempModules.push_back(module);

        return *this; // Retorna el propio builder para permitir Fluent Interface
    }

    /**
     * @brief Construye el Robot con los módulos agregados.
     * @return Puntero a la instancia única del Robot completamente lista.
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