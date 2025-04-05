#pragma once

namespace hardware
{
    enum class ButtonEvent : uint8_t
    {
        None,
        Press,
        Release,
    };

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
        ButtonEvent GetEvent(void);

    private:
        ButtonState _state;
        ButtonEvent _event;
        InputLogic _logic;

        int _gpio_pin;
    };
} // namespace hardware
