#ifndef ROBOLIB_H
#define ROBOLIB_H

/**
 * @file Robolib.h
 * @brief Header principal de la librería Robolib.
 *
 * Incluye todos los componentes públicos de la librería para uso directo:
 * - Núcleo: Robot, RobotBuilder
 * - Controladores: Drivers de motores y sensores
 *
 * @note Los headers de interfaces y servicios se incluyen transitivamente
 *       a través de Robot.h y los controladores.
 * @ingroup core
 */

// Núcleo
#include "core/Robot.h"

// Controladores (incluyen sus interfaces transitivamente)
#include "Controllers/Actuators/Motors/HBridgeMotorDriver.h"
#include "Controllers/Sensors/Distance/HCSR04SensorDriver.h"
#include "Controllers/Sensors/Line/TCRT5kDigitalDriver.h"
#include "Controllers/Sensors/Line/TCRT5kAnalogDriver.h"
#include "Controllers/Actuators/LED/LedDriver.h"
#include "Controllers/Actuators/Buzzer/PassiveBuzzerDriver.h"
#include "Controllers/Sensors/Button/ButtonSensorDriver.h"

// Servicios (incluidos transitivamente por los controladores)

#endif