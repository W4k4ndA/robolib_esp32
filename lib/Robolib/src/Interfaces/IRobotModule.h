#ifndef I_ROBOT_MODULE_H
#define I_ROBOT_MODULE_H

namespace robolib
{

    class IRobotModule
    {
    public:
        virtual ~IRobotModule() = default;

        // metodos de ciclo de vida estandar
        virtual void init() = 0;
        virtual void update() {}; // implementacion opcional para modulos que no requieran actualizacion constante
    };

}

#endif // I_ROBOT_MODULE_H