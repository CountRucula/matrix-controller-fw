#include <hardware/gpio.h>

#include "Button.h"
#include "Event.h"

namespace hardware
{
    Button::Button(int gpio_pin, InputLogic logic)
        : _gpio_pin{gpio_pin}
        , _logic{logic}
    {
        gpio_init(_gpio_pin);
    }

    void Button::Update(void)
    {
        ButtonState new_state;

        if(_logic == InputLogic::HighActive)
            new_state = gpio_get(_gpio_pin) ? ButtonState::Pressed : ButtonState::Released;
        else
            new_state = gpio_get(_gpio_pin) ? ButtonState::Released : ButtonState::Pressed;

        if(new_state != _state) {
            _event = (new_state == ButtonState::Pressed) ? EventId::BTN_PRESSED : EventId::BTN_RELEASED;
        }

        _state = new_state;
    }

    ButtonState Button::GetState(void)
    {
        return _state;
    }

    EventId Button::GetEvent(void)
    {
        EventId event = _event;

        _event = EventId::None;

        return event;
    }
} // namespace hardware
