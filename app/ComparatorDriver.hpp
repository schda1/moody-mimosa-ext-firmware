#pragma once

#include <HalWrapper/ITimer.hpp>
#include <HalWrapper/IDigitalOutput.hpp>
#include <PwmHandler.hpp>
#include <Parameter/ParameterHandler.hpp>

#define PORT_CMP (GPIOA)
#define PIN_CMP_COLD (7)
#define PIN_CMP_HOT (8)
#define PIN_CMP_CALM (9)
#define PIN_CMP_LOUD (10)
#define PIN_CMP_DARK (11)
#define PIN_CMP_BRIGHT (12)

class ComparatorDriver
{
public:
    ComparatorDriver(PwmHandler* pwm_handler, ParameterHandler* parameter_handler);
    ~ComparatorDriver();

    void set_duty_cycle(uint8_t channel_nr, uint16_t duty_cycle);

    void init();
    void enable(bool enable);
    void print();

    void set_cold_thresh(uint16_t value);
    void set_hot_thresh(uint16_t value);
    void set_quiet_thresh(uint16_t value);
    void set_loud_thresh(uint16_t value);
    void set_dark_thresh(uint16_t value);
    void set_bright_thresh(uint16_t value);

private:
    PwmHandler* pwm_handler;
    ParameterHandler* parameter_handler;
    IDigitalOutput* outputs[6];
    uint16_t* duty_cycles[6];
};