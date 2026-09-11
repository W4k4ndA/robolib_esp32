#ifndef PASSIVE_BUZZER_SERVICE_H
#define PASSIVE_BUZZER_SERVICE_H

#include <cstdint>
#include "Helper.h"
#include "../Interfaces/IDriverPassiveBuzzer.h"

namespace robolib
{

    class PassiveBuzzerService : public ModuleHelper
    {
        public:

            using ModuleHelper::ModuleHelper;
            
            void sound(size_t index = 0, uint16_t frequency)
            {
                auto *buzzerDriver = getModule<IDriverPassiveBuzzer>(index);
                if (buzzerDriver)
                {
                    buzzerDriver->sound(frequency);
                }
            }

            size_t count() const
            {
                return countModules<IDriverPassiveBuzzer>();
            }

            bool exists() const {
                return count() > 0;
            }
    };

}

#endif