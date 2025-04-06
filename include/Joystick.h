#pragma once

#include <stdint.h>
#include "Event.h"
#include "Button.h"

namespace hardware
{
    enum class JoystickState : uint8_t
    {
        Middle = 1,
        Left,
        Right,
        Top,
        Bottom
    };

    typedef struct 
    {
        Button left, right, top, bottom;
    } JoystickBtns_t;

    class Joystick
    {
    public:
        Joystick() = default;
        Joystick(JoystickBtns_t btns);

        void Update(void);

        JoystickState GetState(void);
        EventId GetEvent(void);

    private:
        JoystickState _state;
        EventId _event;

        JoystickBtns_t _btns;
    };
} // namespace hardware
