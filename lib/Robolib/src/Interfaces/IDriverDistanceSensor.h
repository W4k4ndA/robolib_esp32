#ifndef I_DRIVER_DISTANCE_SENSOR_H
#define I_DRIVER_DISTANCE_SENSOR_H

#include "IRobotModule.h"
/**
 * @file IDriverDistanceSensor.h
 * @brief Interfaz abstracta para sensores de distancia.
 * 
 * Define el contrato que deben implementar todos los sensores de distancia
 * compatibles con el sistema (ultrasonido HC-SR04, VL53L0X, TF-Luna, etc.).
 * 
 * @ingroup drivers
 */

namespace robolib {

/**
 * @brief Interfaz base para sensores de distancia.
 * 
 * Abstrae la medición de distancia permitiendo intercambiar tecnologías
 * (ultrasonido, ToF láser, infrarrojo) sin afectar la lógica de navegación.
 * 
 * Unidades: todas las mediciones se reportan en **centímetros (cm)**.
 * 
 * Valores de retorno especiales:
 * - > 0: distancia válida en cm
 * - < 0: error (timeout, fuera de rango, fallo de hardware)
 * 
 * @see HCSR04SensorDriver
 * @ingroup drivers
 */
class IDriverDistanceSensor : public IRobotModule {
public:
    /**
     * @brief Destructor virtual por defecto.
     * 
     * Necesario para destrucción correcta de objetos derivados mediante
     * puntero a la interfaz (polimorfismo).
     */
    virtual ~IDriverDistanceSensor() = default;


    /**
     * @brief Obtiene el tipo de módulo.
     *
     * @return ModuleType::DistanceSensor
     */
    static constexpr ModuleType staticType() { return ModuleType::DistanceSensor; }
    ModuleType getType() const override { return staticType(); }


    /**
     * @brief Realiza una medición de distancia única en cm.
     * 
     * @return Distancia medida en centímetros.
     *         Retorna valor negativo en caso de error:
     *         - -1.0f: timeout (sin eco / objeto muy lejos)
     *         - -2.0f: error de hardware / comunicación
     *         - -3.0f: fuera de rango válido del sensor
     * 
     * @note Función bloqueante. Duración típica: 10-60 ms según tecnología.
     *       Para mediciones continuas, llamar en loop con delay apropiado.
     * @note El filtrado (media móvil, mediana) es responsabilidad del llamador.
     */
    virtual float measureCM() = 0;
};

} // namespace robolib

#endif