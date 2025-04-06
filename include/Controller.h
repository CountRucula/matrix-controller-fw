#pragma once

#include <serial/Device.h>
#include <serial/Signals.h>

#include "Poti.h"
#include "Button.h"
#include "Joystick.h"
#include "Event.h"

enum class ControllerCommands
{
    GET_POTI_POS        = 0x10,
    GET_POTI_RAW        = 0x11,
    GET_BTN_STATE       = 0x12,
    GET_JOYSTICK_STATE  = 0x13,

    GET_EVENTS          = 0x20,
    SET_POTI_CALIB      = 0x21,
};

const com::DeviceType_t controller_device_type = 0x01;

class Controller : public com::Device
{
public:
    Controller(com::SerialLink &seriallink, hardware::Poti &poti_left, hardware::Poti &poti_right, hardware::Button &btn, hardware::Joystick &joystick);

    void UpdateInput(void);

protected:
    void HandleCmd(uint8_t cmd, uint8_t *args, const size_t length) override;

private:
    void CmdGetPotiPos(uint8_t* args, const size_t length);
    void CmdGetPotiRaw(uint8_t* args, const size_t length);
    void CmdGetBtnState(uint8_t* args, const size_t length);
    void CmdGetJoystickState(uint8_t* args, const size_t length);

    void CmdGetEvents(uint8_t* args, const size_t length);
    void CmdCalibratePoti(uint8_t* args, const size_t length);

    void ReplyPotiPos(uint8_t poti, float pos);
    void ReplyPotiRaw(uint8_t poti, uint16_t raw);
    void ReplyBtnState(hardware::ButtonState state);
    void ReplyJoystickState(hardware::JoystickState state);
    void ReplyEvents(EventId* events, const uint16_t nbr_events);

    void AddEvent(EventId event);
    void ClearEvents(void);

    hardware::Poti& _poti_left;
    hardware::Poti& _poti_right;
    hardware::Button& _btn;
    hardware::Joystick& _joystick;

    EventId _events[100];
    size_t _events_len = 0;
};
