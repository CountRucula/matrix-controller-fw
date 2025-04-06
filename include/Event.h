#pragma once

#include <stdint.h>
#include <variant>

enum class EventId : uint8_t
{
    None,
    BTN_PRESSED,
    BTN_RELEASED,
    JOYSTICK_CHANGED,
    POTI_CHANGED,
};

struct ButtonData   { uint8_t id; };
struct JoystickData { uint8_t id; uint8_t state; };
struct PotiData     { uint8_t id; float val; };

typedef struct
{
    EventId     id;
    union {
        ButtonData btn;
        JoystickData joystick;
        PotiData poti;
    } data;
} event_t;