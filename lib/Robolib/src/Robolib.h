#ifndef ROBOLIB_H
#define ROBOLIB_H


//Nucleo
// #include "Interfaces/IRobotModule.h"
#include "core/Robot.h"

//Interfaces
// #include "Interfaces/IDriverDistanceSensor.h"
// #include "Interfaces/IDriverHBMotor.h"
// #include "Interfaces/IDriverDigitalLineSensor.h"
// #include "Interfaces/IDriverAnalogLineSensor.h"


//Servicios
// #include "Services/Helper.h"
// #include "Services/DistanceService.h"
// #include "Services/HBridgeMotorService.h"
// #include "Services/DigitalLineSensorService.h"
// #include "Services/AnalogLineSensorService.h"

//Controladores
#include "Controllers/Actuators/Motors/HBridgeMotorDriver.h"
#include "Controllers/Sensors/Distance/HCSR04SensorDriver.h"
#include "Controllers/Sensors/Line/TCRT5kDigitalDriver.h"
#include "Controllers/Sensors/Line/TCRT5kAnalogDriver.h"




#endif