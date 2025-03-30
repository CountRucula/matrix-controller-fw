#pragma once

#include <Arduino.h>

namespace matrix {
typedef enum {
  FRAME_COMMAND     = 0x00, 
  FRAME_RESPONSE    = 0x01,
  FRAME_DATA        = 0x02, 
} FrameType_t;

class MatrixLink {
public:
  MatrixLink(Stream &serial);

  void SendFrame(FrameType_t type, const uint8_t *payload, const size_t length);
  void StartReceiving(uint8_t* buffer, const size_t size);
  
  FrameType_t GetType();
  uint8_t* GetPayload(size_t &length);

  size_t HandleIncoming();

  Stream& _serial;

  
private:
  typedef enum {STATE_IDLE, STATE_RECV} State_t;

  void SendEscaped(const uint8_t *buffer, const int length);
  void SendEscaped(const uint32_t word);
  void SendEscaped(const uint8_t byte);

  State_t HandleByte(uint8_t byte);
  void AppendBuffer(uint8_t byte);

  void Drop();
  bool CheckCRC32();


  State_t _state;

  uint8_t *_recv_buffer;
  size_t _recv_buffer_size;
  size_t _recv_idx;
  bool _recv_escaped;
  bool _recv_dropped;

    uint8_t _read_buffer[256]; 
    size_t _read_length = 0;
    size_t _read_idx = 0;
};


} // namespace matrix