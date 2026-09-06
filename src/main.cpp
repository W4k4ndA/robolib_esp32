#include <Arduino.h>
#include <Robolib.h>
using namespace robolib;

Robot *r;

void setup()
{
    r = RobotBuilder()
            .addModule(new TCRT5000SensorDriver(2))
            .build();
}

void loop()
{
}