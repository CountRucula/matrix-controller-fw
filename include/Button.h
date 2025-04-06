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
        EventId GetEvent(void);

    private:
        ButtonState _state;
        EventId _event;
        InputLogic _logic;

        int _gpio_pin;
    };
} // namespace hardware
