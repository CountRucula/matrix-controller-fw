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
            _event_id = EventId::JOYSTICK_CHANGED;
        }

        _state = new_state;
    }

    JoystickState Joystick::GetState(void)
    {
        return _state;
    }

    event_t Joystick::GetEvent(void)
    {
        event_t event = {
            .id = _event_id,
            .data = { 
                .joystick = {
                    .id = 0, 
                    .state=static_cast<uint8_t>(_state)
                }
            }
        };

        _event_id = EventId::None;

        return event;
    }
} // namespace hardware
