#ifndef DIGITAL_LINE_SENSOR_SERVICE_H
#define DIGITAL_LINE_SENSOR_SERVICE_H

#include "Helper.h"
#include <cstdint>
#include <vector>
#include "../Interfaces/IDriverDigitalLineSensor.h"

namespace robolib
{
    class DigitalLineSensorService : public ModuleHelper
    {
    public:
        using ModuleHelper::ModuleHelper;


        bool isOnLine(size_t index = 0) const
        {
            auto *sensor = getModule<IDriverDigitalLineSensor>(index);
            return sensor ? sensor->getLine() : false;
        }

        size_t count() const
        {
            return countModules<IDriverDigitalLineSensor>();
        }

        bool exists() const { return count() > 0; }


        void setIsOnLine(size_t index = 0, bool value = true){
            auto *sensor = getModule<IDriverDigitalLineSensor>(index);

            if(sensor) return sensor->set1IsOnLine(value);
        }

    };
}

#endif