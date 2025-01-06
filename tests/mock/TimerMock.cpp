#include <TimerMock.hpp>

TimerMock::TimerMock(TIM_TypeDef* instance) : 
    ITimer(instance), 
    prescaler(0), 
    period(0), 
    raw_cycles(0), 
    cycles(0), 
    interrupt_counter(0),
    running(false)
{ }

void TimerMock::set_prescaler(uint32_t prescaler)
{
    this->prescaler = prescaler;
}

void TimerMock::set_period(uint32_t period)
{
    this->period = period;
}

void TimerMock::init()
{
    cycles = 0;
    raw_cycles = 0;
    running = false;
}

void TimerMock::deinit()
{
    cycles = 0;
    raw_cycles = 0;
    running = false;
}

void TimerMock::start()
{
    running = true;
}

void TimerMock::stop()
{
    running = false;
}

void TimerMock::reset()
{
    cycles = 0;
}

void TimerMock::callback()
{
    if (callback_fct != nullptr) {
        callback_fct();
    }
}

void TimerMock::tick()
{
    if (running) {
        raw_cycles++;

        if (raw_cycles % prescaler == 0) {
            raw_cycles = 0;
            cycles++;
        }

        if (cycles >= period) {
            cycles = 0;
            callback();
            interrupt_counter++;
        }
    }
}

void TimerMock::tick(uint32_t ticks)
{
    for (uint32_t i = 0; i < ticks; i++) {
        tick();
    }
}