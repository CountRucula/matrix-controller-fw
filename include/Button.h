#pragma once

#include "Event.h"

namespace hardware
{
    enum class ButtonState : uint8_t
    {
        Pressed = 1,
        Released
    };

    enum class InputLogic
    {
        HighActive,
        LowActive
    };

    class Button
    {
    public:
        Button() = default;
        Button(int gpio_pin, InputLogic logic);

        void Update(void);

        ButtonState GetState(void);
        event_t GetEvent(void);

    private:
        ButtonState _state;
        EventId _event_id;
        InputLogic _logic;

        int _gpio_pin;
    };
} // namespace hardware
