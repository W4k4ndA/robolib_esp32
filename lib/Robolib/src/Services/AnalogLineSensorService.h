#ifndef ANALOG_LINE_SENSOR_SERVICE_H
#define ANALOG_LINE_SENSOR_SERVICE_H

#include "Helper.h"
#include <cstdint>
#include <vector>
#include "../Interfaces/IDriverAnalogLineSensor.h"

namespace robolib
{
    class AnalogLineSensorService : public ModuleHelper
    {
    public:
        using ModuleHelper::ModuleHelper;

        uint16_t isOnLine(size_t index = 0) const
        {
            auto *sensor = getModule<IDriverAnalogLineSensor>(index);
            return sensor ? sensor->getAnalogLine() : 0;
        }

        size_t count() const
        {
            return countModules<IDriverAnalogLineSensor>();
        }

        bool exists() const { return count() > 0; }

        void updateThreshold(size_t index = 0, int value = 800)
        {
            auto *sensor = getModule<IDriverAnalogLineSensor>(index);

            if (sensor)
                sensor->setThreshold(value);
        }

        bool isLine(size_t index = 0)
        {
            auto *sensor = getModule<IDriverAnalogLineSensor>(index);
            if (sensor)
                return sensor->isOnLine();
            return false;
        }
    };
}

#endif