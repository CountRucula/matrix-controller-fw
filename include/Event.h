#pragma once

#include <stdint.h>

enum class EventId : uint8_t
{
    None,
    BTN_PRESSED,
    BTN_RELEASED,
    JOYSTICK_TO_MIDDLE,
    JOYSTICK_TO_LEFT,
    JOYSTICK_TO_RIGHT,
    JOYSTICK_TO_TOP,
    JOYSTICK_TO_BOTTOM,
};