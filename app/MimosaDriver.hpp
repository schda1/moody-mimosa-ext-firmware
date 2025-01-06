
#pragma once

#include <HalWrapper/IDigitalOutput.hpp>
#include <HalWrapper/ITimer.hpp>
#include <HalWrapper/IExtiInput.hpp>
#include <HalWrapper/PwmOutput.hpp>
#include <Parameter/ParameterHandler.hpp>
#include <Parameter.hpp>
#include <PinObserver.hpp>

class MimosaDriver
{

public:
    MimosaDriver(PwmOutput* clk, IExtiInput *exti , IDigitalOutput* rst_n, PinObserver* observer, ParameterHandler* handler);
    virtual ~MimosaDriver() {}

    void init();
    void start();
    void stop();
    void pause();
    void set_period_ms(uint32_t period_ms);

    void enable_log(bool enable);

    void callback_exti(void);
    void callback_timer(void);

    void update();

protected:
    void set_period();

    PwmOutput* clk;
    IExtiInput* model_clk_exti;

    IDigitalOutput* rst_n;

    PinObserver* pin_observer;
    ParameterHandler* param_handler;

    uint32_t* period_ms;

    uint8_t status;
    uint32_t counter;
};