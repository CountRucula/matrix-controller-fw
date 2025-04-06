#include "Controller.h"
#include "Util.h"

#include <hardware/adc.h>
#include <hardware/gpio.h>

#define POTI_LEFT 0
#define POTI_RIGTH 1

Controller::Controller(com::SerialLink &seriallink, hardware::Poti& poti_left, hardware::Poti& poti_right, hardware::Button& btn0, hardware::Button& btn1, hardware::Joystick& joystick)
    : com::Device(seriallink)
    , _poti_left{poti_left}
    , _poti_right{poti_right}
    , _btn0{btn0}
    , _btn1{btn1}
    , _joystick{joystick}
{
    // set device type
    SetDeviceType(controller_device_type);
}

void Controller::UpdateInput(void)
{
    event_t event;

    _btn0.Update();
    _btn1.Update();
    _joystick.Update();
    _poti_left.Update();
    _poti_right.Update();

    event = _btn0.GetEvent();
    if(event.id != EventId::None) {
        event.data.btn.id = 0;
        AddEvent(event);
    }

    event = _btn1.GetEvent();
    if(event.id != EventId::None) {
        event.data.btn.id = 1;
        AddEvent(event);
    }

    event = _joystick.GetEvent();
    if(event.id != EventId::None) {
        event.data.joystick.id = 0;
        AddEvent(event);
}

    event = _poti_left.GetEvent();
    if(event.id != EventId::None) {
        event.data.poti.id = POTI_LEFT;
        AddEvent(event);
    }

    event = _poti_right.GetEvent();
    if(event.id != EventId::None) {
        event.data.poti.id = POTI_RIGTH;
        AddEvent(event);
    }
}

void Controller::HandleCmd(uint8_t cmd, uint8_t *args, const size_t length)
{

    switch (static_cast<ControllerCommands>(cmd))
    {
    case ControllerCommands::GET_POTI_POS:          CmdGetPotiPos(args, length);        break;
    case ControllerCommands::GET_POTI_RAW:          CmdGetPotiRaw(args, length);        break;
    case ControllerCommands::GET_BTN_STATE:         CmdGetBtnState(args, length);       break;
    case ControllerCommands::GET_JOYSTICK_STATE:    CmdGetJoystickState(args, length);  break;

    case ControllerCommands::GET_EVENTS:            CmdGetEvents(args, length);         break;
    case ControllerCommands::SET_POTI_CALIB:        CmdCalibratePoti(args, length);     break;

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

void Controller::CmdGetPotiRaw(uint8_t* args, const size_t length)
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
    ReplyPotiRaw(poti_nr, poti->Raw());
}

void Controller::CmdCalibratePoti(uint8_t* args, const size_t length)
{
    uint8_t poti_nr;
    hardware::Poti* poti;
    uint16_t min, max;

    // check args length
    if(length < (sizeof(poti_nr) + sizeof(min) + sizeof(max)))
        return; 

    // parse args
    poti_nr = args[0];
    memcpy(&min, args+1, sizeof(min));
    memcpy(&max, args+3, sizeof(max));

    if(poti_nr == POTI_LEFT)        poti = &_poti_left;
    else if(poti_nr == POTI_RIGTH)  poti = &_poti_right;
    else                            return;

    poti->Calibrate(min, max);
}

void Controller::CmdGetBtnState(uint8_t* args, const size_t length)
{
    uint8_t btn_id;

    if(length < sizeof(btn_id))
        return;

    btn_id = args[0];

    if(btn_id == 0) ReplyBtnState(_btn0.GetState());
    if(btn_id == 1) ReplyBtnState(_btn1.GetState());
}

void Controller::CmdGetJoystickState(uint8_t* args, const size_t length)
{
    ReplyJoystickState(_joystick.GetState());
}

void Controller::CmdGetEvents(uint8_t* args, const size_t length)
{
    ReplyEvents(_events, _events_len);
    ClearEvents();
}

void Controller::ReplyPotiPos(uint8_t poti, float pos)
{
    uint8_t payload[6] = {static_cast<uint8_t>(ControllerCommands::GET_POTI_POS), poti};
    
    uint8_t* pos_ptr = reinterpret_cast<uint8_t*>(&pos);
    payload[2] = pos_ptr[0];
    payload[3] = pos_ptr[1];
    payload[4] = pos_ptr[2];
    payload[5] = pos_ptr[3];

    _link.SendFrame(com::FrameType::FRAME_RESPONSE, payload, sizeof(payload));
}

void Controller::ReplyPotiRaw(uint8_t poti, uint16_t raw)
{
    uint8_t payload[] = {static_cast<uint8_t>(ControllerCommands::GET_POTI_RAW), poti, reinterpret_cast<uint8_t*>(&raw)[0], reinterpret_cast<uint8_t*>(&raw)[1]};
    _link.SendFrame(com::FrameType::FRAME_RESPONSE, payload, sizeof(payload));
}

void Controller::ReplyBtnState(hardware::ButtonState state)
{
    uint8_t payload[] = {static_cast<uint8_t>(ControllerCommands::GET_BTN_STATE), static_cast<uint8_t>(state)};
    _link.SendFrame(com::FrameType::FRAME_RESPONSE, payload, sizeof(payload));
}

void Controller::ReplyJoystickState(hardware::JoystickState state)
{
    uint8_t payload[] = {static_cast<uint8_t>(ControllerCommands::GET_JOYSTICK_STATE), static_cast<uint8_t>(state)};
    _link.SendFrame(com::FrameType::FRAME_RESPONSE, payload, sizeof(payload));
}

void Controller::ReplyEvents(event_t* events, uint16_t nbr_events)
{
    uint8_t payload[sizeof(uint8_t) + sizeof(uint16_t) + sizeof(_events)] = {
        static_cast<uint8_t>(ControllerCommands::GET_EVENTS), 
        reinterpret_cast<uint8_t *>(&nbr_events)[0], 
        reinterpret_cast<uint8_t *>(&nbr_events)[1]
    };

    uint8_t *ptr = payload + sizeof(uint8_t) + sizeof(uint16_t);
    for(int i = 0; i < nbr_events; i++) {
        *(ptr++) = static_cast<uint8_t>(events[i].id); 

        switch (events[i].id)
        {
        case EventId::BTN_PRESSED:
        case EventId::BTN_RELEASED:
            *(ptr++) = events[i].data.btn.id; 
            break;

        case EventId::JOYSTICK_CHANGED:
            *(ptr++) = events[i].data.joystick.id;
            *(ptr++) = events[i].data.joystick.state;
            break;

        case EventId::POTI_CHANGED:
            *(ptr++) = events[i].data.poti.id;

            *(ptr++) = reinterpret_cast<uint8_t*>(&events[i].data.poti.val)[0];
            *(ptr++) = reinterpret_cast<uint8_t*>(&events[i].data.poti.val)[1];
            *(ptr++) = reinterpret_cast<uint8_t*>(&events[i].data.poti.val)[2];
            *(ptr++) = reinterpret_cast<uint8_t*>(&events[i].data.poti.val)[3];

            *(ptr++) = reinterpret_cast<uint8_t*>(&events[i].data.poti.raw)[0];
            *(ptr++) = reinterpret_cast<uint8_t*>(&events[i].data.poti.raw)[1];
            break;
        }
    }

    _link.SendFrame(com::FrameType::FRAME_RESPONSE, payload, ptr - payload);
}

void Controller::AddEvent(event_t event)
{
    if(_events_len < ARRAY_LEN(_events))
        _events[_events_len++] = event;
}

void Controller::ClearEvents(void)
{
    _events_len = 0;
}