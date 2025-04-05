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
        adc_select_input(_adc_channel);
        return adc_read();
    }

    float Poti::Read(void)
    {
        uint16_t raw = Raw();
        return Saturate((raw - _min) * _gain);
    }

    void Poti::CalibrateMax(void)
    {
        _max = Raw();
        CalcGain();
    }

    void Poti::CalibrateMin(void)
    {
        _min = Raw();
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