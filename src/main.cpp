#include <Arduino.h>
#include <Robolib.h>

using namespace robolib;

auto robot = RobotBuilder()
        .addModule(new HCSR04SensorDriver(1, 2))
        .addModule(new HBridgeMotorDriver(3, 4, 5, 6))
        .build();

void setup()
{
    robot->begin();
}

void loop()
{

    robot->distanceSensor->getCM();
}   