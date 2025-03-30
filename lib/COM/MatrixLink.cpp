#include "MatrixLink.hpp"
#include "ErriezCRC32.h"
#include <tusb.h>

#define HEADER 0x7E
#define FOOTER 0x7E
#define ESCAPE 0x7D

namespace matrix
{
    MatrixLink::MatrixLink(Stream &serial)
        : _serial(serial)
    {
    }
  
    void MatrixLink::SendFrame(FrameType_t type, const uint8_t *payload, const size_t length)
    {
        uint8_t ftype = static_cast<uint8_t>(type);

        uint32_t crc = crc32Update(&ftype, 1, CRC32_INITIAL);
        crc = crc32Update(payload, length, crc);
        crc = crc32Final(crc);
        
        _serial.write(HEADER);

        SendEscaped(ftype);
        SendEscaped(payload, length);
        SendEscaped(crc);

        _serial.write(FOOTER);
    }

    void MatrixLink::StartReceiving(uint8_t *buffer, const size_t size)
    {
        _recv_buffer = buffer;
        _recv_buffer_size = size;
    }

    FrameType_t MatrixLink::GetType()
    {
        return static_cast<FrameType_t>(_recv_buffer[0]);
    }

    uint8_t* MatrixLink::GetPayload(size_t &length)
    {
        length = _recv_idx - 4 - 1;
        return _recv_buffer + 1;
    }

    size_t MatrixLink::HandleIncoming()
    {
        int available = 0;

        while(_read_length || (available = _serial.available())) {

            // no bytes left in buffer
            if(_read_length == 0) {
                _read_length = MIN(sizeof(_read_buffer), available);
                _read_length = tud_cdc_read(_read_buffer, _read_length);
                _read_idx = 0;
            }

            // input byte
            uint8_t& byte = _read_buffer[_read_idx++];
            _read_length--;

            switch (_state)
            {
            case STATE_IDLE:
                if (byte == HEADER) {
                    _state = STATE_RECV;
                    _recv_idx = 0;
                    _recv_escaped = false;
                    _recv_dropped = false;
                }
                break;

            case STATE_RECV:
                if(HandleByte(byte) == STATE_IDLE) {
                    _state = STATE_IDLE;

                    if(!_recv_dropped && CheckCRC32()) {
                        return _recv_idx;
                    } 
                }

            default:
                break;
            }
        }

        return 0;
    }

    void MatrixLink::SendEscaped(const uint8_t *buffer, const int length)
    {
        for(int i = 0; i < length; i++) {
            SendEscaped(buffer[i]);
        }
    }
  
    void MatrixLink::SendEscaped(const uint32_t word)
    {
        SendEscaped(reinterpret_cast<const uint8_t*>(&word), sizeof(word));
    }

    void MatrixLink::SendEscaped(const uint8_t byte)
    {
        if((byte == HEADER) || (byte == FOOTER) || (byte == ESCAPE)) {
            _serial.write(ESCAPE);
            _serial.write(byte ^ 0x20);
        } else {
            _serial.write(byte);
        }
    }

    MatrixLink::State_t MatrixLink::HandleByte(uint8_t byte)
    {
        if (_recv_escaped)
        {
            _recv_escaped = false;
            AppendBuffer(byte ^ 0x20);
        }
        else
        {
            if (byte == ESCAPE)
            {
                _recv_escaped = true;
            }
            else if (byte == FOOTER)
            {
                return STATE_IDLE;
            }
            else
            {
                AppendBuffer(byte);
            }
        }

        return STATE_RECV;
    }

    void MatrixLink::AppendBuffer(uint8_t byte)
    {
        if(!_recv_dropped) {
            if(_recv_idx >= _recv_buffer_size) {
                Drop();
                return;
            }

            _recv_buffer[_recv_idx] = byte;
            _recv_idx++;
        }
    }

    void MatrixLink::Drop()
    {
        _recv_dropped = true;
    }

    bool MatrixLink::CheckCRC32() 
    {
        uint32_t received_crc = *((uint32_t*)&_recv_buffer[_recv_idx-4]);
        uint32_t calculated_crc = crc32Buffer(_recv_buffer, _recv_idx-4);

        return received_crc == calculated_crc;
    }

} // namespace matrix