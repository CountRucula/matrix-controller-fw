#include <hardware/gpio.h>

#include "Joystick.h"

namespace hardware
{
    Joystick::Joystick(JoystickBtns_t btns)
        : _btns{btns}
    {
    }

    void Joystick::Update(void)
    {
        JoystickState new_state;

        // read pins
        _btns.left.Update();
        _btns.right.Update();
        _btns.top.Update();
        _btns.bottom.Update();

        // check pin states
        if(_btns.left.GetState() == ButtonState::Pressed)
            new_state = JoystickState::Left;

        else if(_btns.right.GetState() == ButtonState::Pressed)
            new_state = JoystickState::Right;

        else if(_btns.top.GetState() == ButtonState::Pressed)
            new_state = JoystickState::Top;

        else if(_btns.bottom.GetState() == ButtonState::Pressed)
            new_state = JoystickState::Bottom;

        else
            new_state = JoystickState::Middle;

        if(new_state != _state) {
            switch (new_state)
            {
            case JoystickState::Middle:     _event = EventId::JOYSTICK_TO_MIDDLE;   break;
            case JoystickState::Left:       _event = EventId::JOYSTICK_TO_LEFT;     break;
            case JoystickState::Right:      _event = EventId::JOYSTICK_TO_RIGHT;    break;
            case JoystickState::Top:        _event = EventId::JOYSTICK_TO_TOP;      break;
            case JoystickState::Bottom:     _event = EventId::JOYSTICK_TO_BOTTOM;   break;
            }
        }

        _state = new_state;
    }

    JoystickState Joystick::GetState(void)
    {
        return _state;
    }

    EventId Joystick::GetEvent(void)
    {
        EventId event = _event;

        _event = EventId::None;

        return event;
    }
} // namespace hardware
