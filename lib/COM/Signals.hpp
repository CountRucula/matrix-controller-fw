#pragma once

#include <Arduino.h>
#include <functional>

namespace matrix
{
    template <typename... Args>
    class Signal
    {
    public:
        using Callback = std::function<void(Args...)>;

        void connect(Callback callback)
        {
            _callback = callback;
        }

        void emit(Args... args)
        {
            _callback(args...);
        }

    private:
        Callback _callback;
    };

} // namespace signals
