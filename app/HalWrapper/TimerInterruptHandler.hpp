#pragma once

#include <common.hpp>
#include <HalWrapper/ITimer.hpp>

#define N_TIMER (12)

class TimerInterruptHandler
{
public:
    static TimerInterruptHandler* getInstance()
    {
        if (handler == nullptr) {
            handler = new TimerInterruptHandler();
        }
        return handler;
    }

    void attach(ITimer* timer);
    void detach(ITimer* timer);

    ITimer* get_timer(TIM_TypeDef* instance);

private:
    TimerInterruptHandler();
    TimerInterruptHandler(TimerInterruptHandler const&);
    void operator=(TimerInterruptHandler const&);

    int8_t get_timer_id(TIM_TypeDef* instance);

    static TimerInterruptHandler* handler;
    ITimer* timers[N_TIMER];
};
