#pragma once

#include <stdint.h>
#include "Event.h"

namespace hardware
{
    class Poti
    {
    public:
        Poti() = default;
        Poti(int gpio_pin, int adc_channel);

        uint16_t Raw(void);
        float Read(void);

        void Update(void);
        event_t GetEvent(void);

        void Calibrate(uint16_t min, uint16_t max);

    private:
        void CalcGain(void);
        float Saturate(float);

        int _gpio_pin;
        int _adc_channel;

        EventId _event_id;

        uint16_t _raw;
        float _val;

        uint16_t _min = 0;
        uint16_t _max = 4095;
        float _gain = 1.0;
    };
} // namespace hardware
