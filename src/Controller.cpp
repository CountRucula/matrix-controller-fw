#include "Controller.h"
#include "Util.h"

#include <hardware/adc.h>
#include <hardware/gpio.h>

#define POTI_CALIBRATE_MAX 0
#define POTI_CALIBRATE_MIN 1

#define POTI_LEFT 0
#define POTI_RIGTH 1

Controller::Controller(com::SerialLink &seriallink, hardware::Poti& poti_left, hardware::Poti& poti_right, hardware::Button& btn, hardware::Joystick& joystick)
    : com::Device(seriallink)
    , _poti_left{poti_left}
    , _poti_right{poti_right}
    , _btn{btn}
    , _joystick{joystick}
{
    // set device type
    SetDeviceType(controller_device_type);
}

void Controller::HandleCmd(uint8_t cmd, uint8_t *args, const size_t length)
{

    switch (static_cast<ControllerCommands>(cmd))
    {
    case ControllerCommands::GET_BTN_STATE:         CmdGetBtnState(args, length);       break;
    case ControllerCommands::GET_BTN_EVENT:         CmdGetBtnEvent(args, length);       break;
    case ControllerCommands::GET_JOYSTICK_STATE:    CmdGetJoystickState(args, length);  break;
    case ControllerCommands::GET_JOYSTICK_EVENT:    CmdGetJoystickEvent(args, length);  break;
    case ControllerCommands::GET_POTI_POS:          CmdGetPotiPos(args, length);        break;
    case ControllerCommands::CALIBRATE_POTI:        CmdCalibratePoti(args, length);     break;

    default:
        com::Device::HandleCmd(cmd, args, length); 
        break;
    }
}

void Controller::CmdGetPotiPos(uint8_t* args, const size_t length)
{
    uint8_t poti_nr;
    hardware::Poti* poti;

    // check args length
    if(length < sizeof(poti_nr))
        return; 

    // select poti
    poti_nr = args[0];

    if(poti_nr == POTI_LEFT)        poti = &_poti_left;
    else if(poti_nr == POTI_RIGTH)  poti = &_poti_right;
    else                            return;

    // read & reply
    ReplyPotiPos(poti_nr, poti->Read());
}

void Controller::CmdCalibratePoti(uint8_t* args, const size_t length)
{
    uint8_t poti_nr;
    hardware::Poti* poti;
    uint8_t cal_type;

    // check args length
    if(length < (sizeof(poti_nr) + sizeof(uint8_t)))
        return; 

    // select poti
    poti_nr = args[0];

    if(poti_nr == POTI_LEFT)        poti = &_poti_left;
    else if(poti_nr == POTI_RIGTH)  poti = &_poti_right;
    else                            return;

    cal_type = args[1];

    if(cal_type == POTI_CALIBRATE_MIN)      poti->CalibrateMin();
    else if(cal_type == POTI_CALIBRATE_MAX) poti->CalibrateMax();
}

void Controller::CmdGetBtnState(uint8_t* args, const size_t length)
{
    ReplyBtnState(_btn.GetState());
}

void Controller::CmdGetBtnEvent(uint8_t* args, const size_t length)
{
    ReplyBtnEvent(_btn.GetEvent());
}
    
void Controller::CmdGetJoystickState(uint8_t* args, const size_t length)
{
    ReplyJoystickState(_joystick.GetState());
}

void Controller::CmdGetJoystickEvent(uint8_t* args, const size_t length)
{
    ReplyJoystickEvent(_joystick.GetEvent());
}

void Controller::ReplyPotiPos(uint8_t poti, float pos)
{
    uint8_t payload[6] = {static_cast<uint8_t>(ControllerCommands::GET_POTI_POS), poti};
    
    uint8_t* pos_ptr = reinterpret_cast<uint8_t*>(&pos);
    payload[2] = pos_ptr[3];
    payload[3] = pos_ptr[2];
    payload[4] = pos_ptr[1];
    payload[5] = pos_ptr[0];

    _link.SendFrame(com::FrameType::FRAME_RESPONSE, payload, sizeof(payload));
}

void Controller::ReplyBtnState(hardware::ButtonState state)
{
    uint8_t payload[] = {static_cast<uint8_t>(ControllerCommands::GET_BTN_STATE), static_cast<uint8_t>(state)};
    _link.SendFrame(com::FrameType::FRAME_RESPONSE, payload, sizeof(payload));
}

void Controller::ReplyBtnEvent(hardware::ButtonEvent event)
{
    uint8_t payload[] = {static_cast<uint8_t>(ControllerCommands::GET_BTN_EVENT), static_cast<uint8_t>(event)};
    _link.SendFrame(com::FrameType::FRAME_RESPONSE, payload, sizeof(payload));
}

void Controller::ReplyJoystickState(hardware::JoystickState state)
{
    uint8_t payload[] = {static_cast<uint8_t>(ControllerCommands::GET_JOYSTICK_STATE), static_cast<uint8_t>(state)};
    _link.SendFrame(com::FrameType::FRAME_RESPONSE, payload, sizeof(payload));
}

void Controller::ReplyJoystickEvent(hardware::JoystickEvent event)
{
    uint8_t payload[] = {static_cast<uint8_t>(ControllerCommands::GET_JOYSTICK_EVENT), static_cast<uint8_t>(event)};
    _link.SendFrame(com::FrameType::FRAME_RESPONSE, payload, sizeof(payload));
}