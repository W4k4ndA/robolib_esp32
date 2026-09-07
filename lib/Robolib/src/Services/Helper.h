#ifndef HELPER_H
#define HELPER_H

#include <vector>
#include <cstddef>
#include "../Interfaces/IRobotModule.h"

namespace robolib {

/**
 * @brief Helper de búsqueda genérica de módulos por tipo T (Interfaz).
 */
class ModuleHelper {
protected:
    const std::vector<IRobotModule*>& modules;

public:
    explicit ModuleHelper(const std::vector<IRobotModule*>& mods) : modules(mods) {}

    /**
     * @brief Obtiene el n-ésimo módulo que implemente la interfaz T.
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