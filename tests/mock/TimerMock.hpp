#pragma once 

#include <common.hpp>
#include <HalWrapper/ITimer.hpp>
#include "stm32g4xx_hal.h"

class TimerMock : public ITimer
{
    public: 
        TimerMock(TIM_TypeDef* instance);
        virtual ~TimerMock() {};

        virtual void set_prescaler(uint32_t prescaler);
        virtual void set_period(uint32_t period);

        virtual void init() override;
        virtual void deinit() override;
        virtual void start() override;
        virtual void stop() override;
        virtual void reset() override;

        virtual void callback() override;

        void tick();
        void tick(uint32_t ticks);

        uint32_t get_cycles() const { return cycles; }
        uint32_t get_raw_cycles() const { return raw_cycles; }
        bool is_running() const { return running; }
        uint32_t get_interrupt_counter() const { return interrupt_counter; }

    protected:
        uint32_t prescaler;
        uint32_t period;
        uint32_t raw_cycles;
        uint32_t cycles;
        uint32_t interrupt_counter;
        bool running;
};