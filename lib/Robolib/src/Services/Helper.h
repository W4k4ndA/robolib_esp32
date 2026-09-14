#ifndef HELPER_H
#define HELPER_H

/**
 * @file Helper.h
 * @brief Utilidades genéricas para búsqueda y filtrado de módulos por tipo.
 *
 * Proporciona ModuleHelper: clase base para servicios que necesitan buscar
 * módulos específicos en el vector de módulos del robot.
 *
 * @ingroup services
 */

#include <vector>
#include <cstdint>
#include "../Interfaces/IRobotModule.h"

namespace robolib {

/**
 * @class ModuleHelper
 * @brief Helper genérico para filtrado de módulos por interfaz (tipo).
 *
 * Encapsula la lógica común de búsqueda: iterar el vector de módulos,
 * filtrar por ModuleType via getType(), y hacer static_cast seguro.
 *
 * Evita duplicación de código en DistanceService, HBridgeMotorService, etc.
 *
 * @note Requiere que los módulos implementen getType() y staticType() correctamente.
 * @ingroup services
 */
class ModuleHelper {
protected:
    /// Referencia al vector de módulos del Robot (ownership en Robot)
    const std::vector<IRobotModule*>& modules;

public:
    /**
     * @brief Constructor.
     * @param mods Referencia al vector de módulos del Robot.
     */
    explicit ModuleHelper(const std::vector<IRobotModule*>& mods) : modules(mods) {}

    /**
     * @brief Obtiene el n-ésimo módulo que implementa la interfaz T.
     *
     * @tparam T Tipo de interfaz buscado (ej: IDriverHBMotor).
     * @param index Índice base 0 entre los módulos de tipo T (default: 0 = primero).
     * @return Puntero al módulo casteado a T*, o nullptr si no existe.
     *
     * @code
     * // Obtener primer motor
     * auto* motor = getModule<IDriverHBMotor>(0);
     * // Obtener segundo sensor de distancia
     * auto* sensor = getModule<IDriverDistanceSensor>(1);
     * @endcode
     */
    template <typename T>
    T* getModule(size_t index = 0) const {
        size_t count = 0;
        for (auto* module : modules) {
            if (module->getType() == T::staticType()) {
                if (count == index) return static_cast<T*>(module);
                count++;
            }
        }
        return nullptr;
    }

    /**
     * @brief Cuenta cuántos módulos registrados implementan la interfaz T.
     * @tparam T Tipo de interfaz a contar.
     * @return Número de módulos de tipo T registrados.
     */
    template <typename T>
    size_t countModules() const {
        size_t count = 0;
        for (auto* module : modules) {
            if (module->getType() == T::staticType()) {
                count++;
            }
        }
        return count;
    }
};

} // namespace robolib

#endif // MODULE_REGISTRY_H