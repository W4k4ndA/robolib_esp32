#ifndef I_ROBOT_MODULE_H
#define I_ROBOT_MODULE_H

namespace robolib
{

    /**
     * @brief Tipos de módulos disponibles en el sistema.
     *
     * @note Se usa en lugar de RTTI para evitar dependencias externas.
     */
    enum class ModuleType
    {
        DCMotor,
        DistanceSensor,
        DigitalLineSensor,
        AnalogLineSensor
    };

    class IRobotModule
    {
    public:
        virtual ~IRobotModule() = default;

        // metodos de ciclo de vida estandar
        virtual void init() = 0;
        virtual void update() {};               // implementacion opcional para modulos que no requieran actualizacion constante
        virtual ModuleType getType() const = 0; // obtiene el tipo de cada modulo segun el enum
    };

}

#endif // I_ROBOT_MODULE_H