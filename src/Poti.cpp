#include "Arduino.h"
#include <hardware/adc.h>
#include <hardware/gpio.h>

#include "Poti.h"

namespace hardware
{
    Poti::Poti(int gpio_pin, int adc_channel)
        : _gpio_pin{gpio_pin}, _adc_channel{adc_channel}
    {
        adc_gpio_init(_gpio_pin);
        CalcGain();
    }

    uint16_t Poti::Raw(void)
    {
        return _raw;
    }

    float Poti::Read(void)
    {
        return _val;
    }
    
    void Poti::Update(void)
    {
        adc_select_input(_adc_channel);

        uint16_t raw = adc_read();

        if(abs(raw - _raw) > 5)
            _event_id = EventId::POTI_CHANGED;

        _raw = raw;
        _val = Saturate((_raw - _min) * _gain);

    }
    
    event_t Poti::GetEvent(void)
    {
        event_t event = {
            .id = _event_id,
            .data = {
                .poti = {
                    .id = 0,
                    .val = _val,
                    .raw = _raw
                }
            }
        };

        _event_id = EventId::None;

        return event;
    }

    void Poti::Calibrate(uint16_t min, uint16_t max)
    {
        _min = min;
        _max = max;

        CalcGain();
    }

    void Poti::CalcGain(void)
    {
        _gain = 100.0f / (_max - _min);
    }

    float Poti::Saturate(float value)
    {
        return MAX(MIN(value, 100.0), 0.0);
    }
} // namespace hardware