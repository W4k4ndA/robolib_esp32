#ifndef ROBOLIB_H
#define ROBOLIB_H


//Nucleo
// #include "src/Interfaces/IRobotModule.h"
#include "src/Robot.h"

//Interfaces
// #include "src/Interfaces/IDriverDistanceSensor.h"
// #include "src/Interfaces/IDriverHBMotor.h"
// #include "src/Interfaces/IDriverDigitalLineSensor.h"
// #include "src/Interfaces/IDriverAnalogLineSensor.h"


//Servicios
// #include "src/Services/Helper.h"
// #include "src/Services/DistanceService.h"
// #include "src/Services/HBridgeMotorService.h"
// #include "src/Services/DigitalLineSensorService.h"
// #include "src/Services/AnalogLineSensorService.h"

//Controladores
#include "src/Controllers/Actuators/Motors/HBridgeMotorDriver.h"
#include "src/Controllers/Sensors/Distance/HCSR04SensorDriver.h"
#include "src/Controllers/Sensors/Line/TCRT5kDigitalDriver.h"
#include "src/Controllers/Sensors/Line/TCRT5kAnalogDriver.h"




#endif