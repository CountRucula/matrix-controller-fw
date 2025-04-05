#pragma once

#include <serial/Device.h>
#include <serial/Signals.h>

#include "Poti.h"
#include "Button.h"
#include "Joystick.h"

enum class ControllerCommands
{
    GET_POTI_POS        = 0x10,
    CALIBRATE_POTI      = 0x11,

    GET_BTN_STATE       = 0x20,
    GET_BTN_EVENT       = 0x21,

    GET_JOYSTICK_STATE  = 0x30,
    GET_JOYSTICK_EVENT  = 0x31
};

const com::DeviceType_t controller_device_type = 0x01;

class Controller : public com::Device
{
public:
    Controller(com::SerialLink &seriallink, hardware::Poti &poti_left, hardware::Poti &poti_right, hardware::Button &btn, hardware::Joystick &joystick);

protected:
    void HandleCmd(uint8_t cmd, uint8_t *args, const size_t length) override;

private:
    void CmdGetPotiPos(uint8_t* args, const size_t length);
    void CmdCalibratePoti(uint8_t* args, const size_t length);
    void CmdGetBtnState(uint8_t* args, const size_t length);
    void CmdGetBtnEvent(uint8_t* args, const size_t length);
    void CmdGetJoystickState(uint8_t* args, const size_t length);
    void CmdGetJoystickEvent(uint8_t* args, const size_t length);

    void ReplyPotiPos(uint8_t poti, float pos);
    void ReplyBtnState(hardware::ButtonState state);
    void ReplyBtnEvent(hardware::ButtonEvent event);
    void ReplyJoystickState(hardware::JoystickState state);
    void ReplyJoystickEvent(hardware::JoystickEvent event);

    hardware::Poti& _poti_left;
    hardware::Poti& _poti_right;
    hardware::Button& _btn;
    hardware::Joystick& _joystick;
};
