#pragma once

#include <functional>
#include <common.hpp>

class ITimer
{
public:
    using TimerCallback = std::function<void(void)>;

    ITimer(TIM_TypeDef* instance) : instance(instance) {}
    virtual ~ITimer() = default;

    virtual void set_prescaler(uint32_t prescaler) = 0;
    virtual void set_period(uint32_t period) = 0;

    virtual void init() = 0;
    virtual void deinit() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void reset() = 0;

    virtual void callback() = 0;

    virtual TIM_TypeDef* get_instance() { return instance; }
    virtual TIM_HandleTypeDef* get_handle() { return &htim; }

    virtual void set_callback(TimerCallback callback_fct) { this->callback_fct = callback_fct; }

protected: 
    TIM_TypeDef* instance;
    TIM_HandleTypeDef htim;
    TimerCallback callback_fct;

};