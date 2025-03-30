#include "MatrixCom.hpp"


namespace matrix{
    MatrixCOM::MatrixCOM(MatrixLink &matrixlink)
        : _link{matrixlink}
    {
    }

    void MatrixCOM::HandleFrame(FrameType_t type, uint8_t *payload, const size_t length)
    {
        Command cmd;
        uint16_t data_length;
        uint8_t *led_data;

        switch (type)
        {
        case FrameType_t::FRAME_COMMAND:
            cmd = static_cast<Command>(payload[0]);

            HandleCmd(cmd, payload+1, length-1);
            break;

        case FrameType_t::FRAME_DATA:

            memcpy(&data_length, payload, sizeof(data_length)); // copy instead of casting => avoid unaligned memory access!
            led_data = payload + sizeof(data_length);

            _signals.LedData.emit(led_data, data_length);
            break;
        
        default:
            break;
        }
    }


    void MatrixCOM::HandleCmd(Command cmd, uint8_t *args, const size_t length)
    {
        uint32_t baudrate;

        switch (cmd)
        {
        case Command::SET_BAUDRATE:
            if (length < 4)
                return;
        
            memcpy(&baudrate, args, sizeof(4));
            _signals.SetBaudrate.emit(baudrate);
            break;

        case Command::GET_FW_VERSION:
            _signals.GetFwVersion.emit(); 
            break;

        case Command::GET_SIZE:
            _signals.GetSize.emit();
            break;

        case Command::CLR_FRAME:
            _signals.ClrFrame.emit();
            break;

        default:
            break;
        }
    }

    MatrixSignals_t& MatrixCOM::Signals()
    {
        return _signals;
    }

    void MatrixCOM::RespondGetFwVersion(uint8_t major, uint8_t minor)
    {
        uint8_t payload[3] = {static_cast<uint8_t>(matrix::Command::GET_FW_VERSION) ,major, minor};
        _link.SendFrame(matrix::FrameType_t::FRAME_RESPONSE, payload, sizeof(payload));
    }

    void MatrixCOM::RespondGetSize(uint8_t width, uint8_t height)
    {
        uint8_t payload[3] = {static_cast<uint8_t>(matrix::Command::GET_SIZE) ,width, height};
        _link.SendFrame(matrix::FrameType_t::FRAME_RESPONSE, payload, sizeof(payload));
    }
}