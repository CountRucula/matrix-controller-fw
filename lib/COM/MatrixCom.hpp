#pragma once

#include "MatrixLink.hpp"
#include "Signals.hpp"

namespace matrix
{
    enum class Command
    {
        GET_FW_VERSION,
        GET_SIZE,
        CLR_FRAME,
        SET_BAUDRATE,
    };

    typedef struct {
        Signal<> GetFwVersion;
        Signal<> GetSize;
        Signal<> ClrFrame;
        Signal<uint32_t> SetBaudrate;
        Signal<uint8_t*, size_t> LedData; 
    } MatrixSignals_t ;

    class MatrixCOM
    {
    public:

        MatrixCOM(MatrixLink &matrixlink);

        void HandleFrame(FrameType_t type, uint8_t *payload, const size_t length);
        void HandleCmd(Command cmd, uint8_t *args, const size_t length);

        MatrixSignals_t& Signals();

        void RespondGetFwVersion(uint8_t major, uint8_t minor);
        void RespondGetSize(uint8_t width, uint8_t height);

    private:
        MatrixLink &_link;

        MatrixSignals_t _signals;
    };
}