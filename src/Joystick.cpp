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
            new_state = JoystickState::Right;

        else if(_btns.bottom.GetState() == ButtonState::Pressed)
            new_state = JoystickState::Right;

        else
            new_state = JoystickState::Middle;

        if(new_state != _state) {
            switch (new_state)
            {
            case JoystickState::Middle:     _event = JoystickEvent::ToMiddle;   break;
            case JoystickState::Left:       _event = JoystickEvent::ToLeft;     break;
            case JoystickState::Right:      _event = JoystickEvent::ToRight;    break;
            case JoystickState::Top:        _event = JoystickEvent::ToTop;      break;
            case JoystickState::Bottom:     _event = JoystickEvent::ToBottom;   break;
            }
        }

        _state = new_state;
    }

    JoystickState Joystick::GetState(void)
    {
        return _state;
    }

    JoystickEvent Joystick::GetEvent(void)
    {
        JoystickEvent event = _event;

        _event = JoystickEvent::None;

        return event;
    }
} // namespace hardware
