#ifndef BUTTON_SERVICE_H
#define BUTTON_SERVICE_H

#include "Helper.h"
#include "../Interfaces/IDriverButton.h"

namespace robolib
{

    class ButtonService : ModuleHelper
    {

    public:
        using ModuleHelper::ModuleHelper; // Hereda constructor

        size_t count() const
        {
            return countModules<IDriverButton>();
        }

        bool exist() const {
            return count() > 0;
        }


        bool isPressed(size_t index = 0)const{
            IDriverButton* button = getModule<IDriverButton>(index);
            if (button) return button->readButton();            
        }


        bool wasPressed(size_t index = 0)const{
            auto button = getModule<IDriverButton>(index);
            if (button) return button->lastStateButton();
        }








    };

}

#endif /* BUTTON_SERVICE_H */